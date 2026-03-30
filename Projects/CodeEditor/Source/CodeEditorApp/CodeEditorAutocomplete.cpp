//
// Created by Jeppe Nielsen on 30/03/2026.
//

#include "CodeEditorAutocomplete.hpp"
#include <clang-c/Index.h>
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace {
    constexpr std::size_t MaxCompletionItems = 64;
    constexpr unsigned ParseOptions = CXTranslationUnit_PrecompiledPreamble |
                                      CXTranslationUnit_Incomplete |
                                      CXTranslationUnit_KeepGoing |
                                      CXTranslationUnit_CacheCompletionResults;
    const unsigned CompletionOptions = clang_defaultCodeCompleteOptions() | CXCodeComplete_SkipPreamble;

    struct CompletionContext {
        LittleCore::TextEditor::Coordinates cursorPosition;
        std::string prefix;
    };

    struct CachedTranslationUnit {
        std::string filePath;
        std::vector<std::string> arguments;
        CXTranslationUnit translationUnit = nullptr;
    };

    struct CurrentFileSymbolInfo {
        bool isCurrentFileSymbol = false;
        bool isLocalVariable = false;
        bool isField = false;
    };

    struct CurrentFileSymbolCollector {
        std::string normalizedFilePath;
        std::unordered_map<std::string, CurrentFileSymbolInfo> symbols;
    };

    std::string ToString(CXString value) {
        const char* text = clang_getCString(value);
        std::string result = text != nullptr ? text : "";
        clang_disposeString(value);
        return result;
    }

    void AddPath(std::set<std::string>& paths, const std::filesystem::path& path) {
        paths.insert(path.lexically_normal().generic_string());
    }

    std::string RunCommand(const char* command) {
        FILE* pipe = popen(command, "r");
        if (pipe == nullptr) {
            return "";
        }

        std::array<char, 512> buffer{};
        std::string output;
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
            output += buffer.data();
        }
        pclose(pipe);

        while (!output.empty() && (output.back() == '\n' || output.back() == '\r' || output.back() == ' ' || output.back() == '\t')) {
            output.pop_back();
        }

        return output;
    }

    std::string ResolveMacOsSdkPath() {
        static const std::string sdkPath = RunCommand("xcrun -sdk macosx --show-sdk-path");
        return sdkPath;
    }

    std::string NormalizePath(const std::string& path) {
        return std::filesystem::path(path).lexically_normal().generic_string();
    }

    bool IsIdentifierCharacter(char value) {
        const auto c = static_cast<unsigned char>(value);
        return std::isalnum(c) != 0 || c == '_';
    }

    std::string ToLower(std::string value) {
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return value;
    }

    bool StartsWithInsensitive(const std::string& value, const std::string& prefix) {
        if (prefix.size() > value.size()) {
            return false;
        }

        for (std::size_t i = 0; i < prefix.size(); ++i) {
            const auto a = static_cast<unsigned char>(value[i]);
            const auto b = static_cast<unsigned char>(prefix[i]);
            if (std::tolower(a) != std::tolower(b)) {
                return false;
            }
        }

        return true;
    }

    std::size_t CursorOffsetFromCoordinates(const std::string& text, const LittleCore::TextEditor::Coordinates& coordinates) {
        std::size_t offset = 0;
        int currentLine = 0;

        while (currentLine < coordinates.mLine && offset < text.size()) {
            const auto nextLine = text.find('\n', offset);
            if (nextLine == std::string::npos) {
                return text.size();
            }

            offset = nextLine + 1;
            ++currentLine;
        }

        const auto lineEnd = text.find('\n', offset);
        const auto clampedLineEnd = lineEnd == std::string::npos ? text.size() : lineEnd;
        return std::min(offset + static_cast<std::size_t>(coordinates.mColumn), clampedLineEnd);
    }

    CompletionContext BuildCompletionContext(const std::string& text,
                                             const LittleCore::TextEditor::Coordinates& cursorPosition) {
        CompletionContext context;
        context.cursorPosition = cursorPosition;

        auto cursorOffset = CursorOffsetFromCoordinates(text, context.cursorPosition);
        auto prefixStart = cursorOffset;

        while (prefixStart > 0 && IsIdentifierCharacter(text[prefixStart - 1])) {
            --prefixStart;
        }

        context.prefix = text.substr(prefixStart, cursorOffset - prefixStart);
        return context;
    }

    std::vector<CodeEditorCompletionCandidate> FilterAndSortCandidates(std::vector<CodeEditorCompletionCandidate> candidates,
                                                                       const std::string& prefix) {
        if (!prefix.empty()) {
            candidates.erase(std::remove_if(candidates.begin(), candidates.end(), [&prefix](const CodeEditorCompletionCandidate& candidate) {
                return !StartsWithInsensitive(candidate.insertText, prefix);
            }), candidates.end());
        }

        std::sort(candidates.begin(), candidates.end(), [&prefix](const CodeEditorCompletionCandidate& left,
                                                                  const CodeEditorCompletionCandidate& right) {
            const bool leftExact = !prefix.empty() && left.insertText == prefix;
            const bool rightExact = !prefix.empty() && right.insertText == prefix;
            if (leftExact != rightExact) {
                return leftExact;
            }

            if (left.isLocalVariable != right.isLocalVariable) {
                return left.isLocalVariable;
            }

            if (left.isField != right.isField) {
                return left.isField;
            }

            if (left.isCurrentFileSymbol != right.isCurrentFileSymbol) {
                return left.isCurrentFileSymbol;
            }

            if (left.clangPriority != right.clangPriority) {
                return left.clangPriority < right.clangPriority;
            }

            const auto leftLower = ToLower(left.insertText);
            const auto rightLower = ToLower(right.insertText);
            if (leftLower != rightLower) {
                return leftLower < rightLower;
            }

            return left.displayText < right.displayText;
        });

        if (candidates.size() > MaxCompletionItems) {
            candidates.resize(MaxCompletionItems);
        }

        return candidates;
    }

    bool IsReservedIdentifier(const std::string& value) {
        if (value.empty() || value[0] != '_') {
            return false;
        }

        if (value.size() >= 2 && value[1] == '_') {
            return true;
        }

        return value.size() >= 2 && std::isupper(static_cast<unsigned char>(value[1])) != 0;
    }

    bool ShouldHideCandidate(CXCursorKind cursorKind, const CodeEditorCompletionCandidate& candidate) {
        if (candidate.insertText.starts_with("__")) {
            return true;
        }

        return cursorKind == CXCursor_MacroDefinition && IsReservedIdentifier(candidate.insertText);
    }

    bool ShouldAppendOpeningParenthesis(CXCompletionChunkKind chunkKind) {
        return chunkKind == CXCompletionChunk_LeftParen;
    }

    bool IsFunctionLikeCursorKind(CXCursorKind kind) {
        switch (kind) {
            case CXCursor_FunctionDecl:
            case CXCursor_CXXMethod:
            case CXCursor_Constructor:
            case CXCursor_Destructor:
            case CXCursor_ConversionFunction:
            case CXCursor_FunctionTemplate:
                return true;
            default:
                return false;
        }
    }

    bool IsRelevantCurrentFileSymbolKind(CXCursorKind kind) {
        switch (kind) {
            case CXCursor_FieldDecl:
            case CXCursor_VarDecl:
            case CXCursor_ParmDecl:
            case CXCursor_FunctionDecl:
            case CXCursor_CXXMethod:
            case CXCursor_Constructor:
            case CXCursor_Destructor:
            case CXCursor_FunctionTemplate:
            case CXCursor_EnumConstantDecl:
            case CXCursor_TypedefDecl:
            case CXCursor_TypeAliasDecl:
            case CXCursor_StructDecl:
            case CXCursor_ClassDecl:
            case CXCursor_EnumDecl:
            case CXCursor_UnionDecl:
                return true;
            default:
                return false;
        }
    }

    bool IsCursorInFile(CXCursor cursor, const std::string& normalizedFilePath) {
        CXFile file = nullptr;
        clang_getExpansionLocation(clang_getCursorLocation(cursor), &file, nullptr, nullptr, nullptr);
        if (file == nullptr) {
            return false;
        }

        return NormalizePath(ToString(clang_getFileName(file))) == normalizedFilePath;
    }

    CXChildVisitResult CollectCurrentFileSymbols(CXCursor cursor, CXCursor, CXClientData clientData) {
        auto& collector = *static_cast<CurrentFileSymbolCollector*>(clientData);
        if (!IsCursorInFile(cursor, collector.normalizedFilePath)) {
            return CXChildVisit_Continue;
        }

        const auto kind = clang_getCursorKind(cursor);
        if (IsRelevantCurrentFileSymbolKind(kind)) {
            const auto spelling = ToString(clang_getCursorSpelling(cursor));
            if (!spelling.empty()) {
                auto& info = collector.symbols[spelling];
                info.isCurrentFileSymbol = true;
                info.isField = info.isField || kind == CXCursor_FieldDecl;

                const auto parentKind = clang_getCursorKind(clang_getCursorSemanticParent(cursor));
                info.isLocalVariable = info.isLocalVariable ||
                                       kind == CXCursor_ParmDecl ||
                                       (kind == CXCursor_VarDecl && IsFunctionLikeCursorKind(parentKind));
            }
        }

        return CXChildVisit_Recurse;
    }

    std::unordered_map<std::string, CurrentFileSymbolInfo> BuildCurrentFileSymbolIndex(CXTranslationUnit translationUnit,
                                                                                        const std::string& filePath) {
        CurrentFileSymbolCollector collector{
                .normalizedFilePath = NormalizePath(filePath)
        };

        clang_visitChildren(clang_getTranslationUnitCursor(translationUnit), CollectCurrentFileSymbols, &collector);
        return collector.symbols;
    }

    std::vector<std::string> BuildCodeIncludeDirectories(const std::vector<std::string>& codeFiles) {
        std::set<std::string> includePaths;

        for (const auto& filePath : codeFiles) {
            AddPath(includePaths, std::filesystem::path(filePath).parent_path());
        }

        return {includePaths.begin(), includePaths.end()};
    }

    std::vector<std::string> BuildClangArguments(const std::string& workspaceRoot,
                                                 const std::string& projectRoot,
                                                 const std::vector<std::string>& codeIncludeDirectories,
                                                 const std::string& sourcePath) {
        std::set<std::string> includePaths;
        AddPath(includePaths, projectRoot);
        AddPath(includePaths, std::filesystem::path(sourcePath).parent_path());

        for (const auto& includeDirectory : codeIncludeDirectories) {
            AddPath(includePaths, includeDirectory);
        }

        static const char* const sharedIncludePaths[] = {
                "/Users/jeppe/Jeppes/Scripting/clang18/include",
                "External/imgui",
                "External/imgui/misc/cpp",
                "External/netImgui/Code/Client",
                "External/entt/src",
                "External/glm",
                "External/glaze/include",
                "External/stb",
                "External/ImGuizmo",
                "Engine/Application/State",
                "Engine/ImGui",
                "Engine/LittleCore/Core",
                "Engine/LittleCore/Fibers",
                "Engine/LittleCore/Files",
                "Engine/LittleCore/Math",
                "Engine/LittleCore/Meta",
                "Engine/LittleCore/Reflection",
                "Engine/LittleCore/ResourceManagement",
                "Engine/LittleCore/Utilities",
                "External/sokol",
                "Engine/Sokol"
        };

        for (const auto* relativePath : sharedIncludePaths) {
            if (relativePath[0] == '/') {
                AddPath(includePaths, relativePath);
            } else {
                AddPath(includePaths, std::filesystem::path(workspaceRoot) / relativePath);
            }
        }

        const std::string clangRoot = "/Users/jeppe/Jeppes/Scripting/clang18";
        const auto sdkPath = ResolveMacOsSdkPath();

        std::vector<std::string> arguments = {
                "-x", "c++",
                "-std=c++23",
                "-stdlib=libc++",
                "-DNDEBUG",
                "-DXWIN_COCOA=1",
                "-ferror-limit=0",
                "-fparse-all-comments",
                "-resource-dir", clangRoot + "/lib/clang/18",
                "-nostdinc++",
                "-isystem", clangRoot + "/include/c++/v1"
        };

        if (!sdkPath.empty()) {
            arguments.push_back("-isysroot");
            arguments.push_back(sdkPath);
        }

        for (const auto& includePath : includePaths) {
            arguments.push_back("-I" + includePath);
        }

        return arguments;
    }

    std::vector<const char*> BuildRawArguments(const std::vector<std::string>& arguments) {
        std::vector<const char*> rawArguments;
        rawArguments.reserve(arguments.size());
        for (const auto& argument : arguments) {
            rawArguments.push_back(argument.c_str());
        }
        return rawArguments;
    }

    void DisposeCachedTranslationUnit(CachedTranslationUnit& cache) {
        if (cache.translationUnit != nullptr) {
            clang_disposeTranslationUnit(cache.translationUnit);
            cache.translationUnit = nullptr;
        }

        cache.filePath.clear();
        cache.arguments.clear();
    }

    bool CreateTranslationUnit(CXIndex index,
                               CachedTranslationUnit& cache,
                               const std::string& filePath,
                               const std::vector<std::string>& arguments,
                               const CXUnsavedFile& unsavedFile) {
        auto rawArguments = BuildRawArguments(arguments);
        auto* translationUnit = clang_parseTranslationUnit(
                index,
                filePath.c_str(),
                rawArguments.data(),
                static_cast<int>(rawArguments.size()),
                const_cast<CXUnsavedFile*>(&unsavedFile),
                1,
                ParseOptions);

        if (translationUnit == nullptr) {
            return false;
        }

        cache.filePath = filePath;
        cache.arguments = arguments;
        cache.translationUnit = translationUnit;
        return true;
    }

    CXTranslationUnit GetOrCreateTranslationUnit(CXIndex index,
                                                 CachedTranslationUnit& cache,
                                                 const std::string& workspaceRoot,
                                                 const std::string& projectRoot,
                                                 const std::vector<std::string>& codeIncludeDirectories,
                                                 const std::string& filePath,
                                                 const CXUnsavedFile& unsavedFile) {
        const auto arguments = BuildClangArguments(workspaceRoot, projectRoot, codeIncludeDirectories, filePath);
        const bool canReuseTranslationUnit = cache.translationUnit != nullptr &&
                                             cache.filePath == filePath &&
                                             cache.arguments == arguments;

        if (!canReuseTranslationUnit) {
            DisposeCachedTranslationUnit(cache);
            if (!CreateTranslationUnit(index, cache, filePath, arguments, unsavedFile)) {
                return nullptr;
            }

            return cache.translationUnit;
        }

        if (clang_reparseTranslationUnit(
                cache.translationUnit,
                1,
                const_cast<CXUnsavedFile*>(&unsavedFile),
                clang_defaultReparseOptions(cache.translationUnit)) != 0) {
            DisposeCachedTranslationUnit(cache);
            if (!CreateTranslationUnit(index, cache, filePath, arguments, unsavedFile)) {
                return nullptr;
            }
        }

        return cache.translationUnit;
    }

    std::vector<CodeEditorCompletionCandidate> Complete(CXIndex index,
                                                        CachedTranslationUnit& cache,
                                                        const std::string& workspaceRoot,
                                                        const std::string& projectRoot,
                                                        const std::vector<std::string>& codeIncludeDirectories,
                                                        const std::string& filePath,
                                                        const std::string& sourceText,
                                                        const LittleCore::TextEditor::Coordinates& cursorPosition,
                                                        const std::string& prefix) {
        if (filePath.empty() || workspaceRoot.empty()) {
            return {};
        }

        CXUnsavedFile unsavedFile;
        unsavedFile.Filename = filePath.c_str();
        unsavedFile.Contents = sourceText.c_str();
        unsavedFile.Length = sourceText.size();

        auto* translationUnit = GetOrCreateTranslationUnit(index, cache, workspaceRoot, projectRoot, codeIncludeDirectories, filePath, unsavedFile);
        if (translationUnit == nullptr) {
            return {};
        }

        const auto currentFileSymbols = BuildCurrentFileSymbolIndex(translationUnit, filePath);

        CXCodeCompleteResults* results = clang_codeCompleteAt(
                translationUnit,
                filePath.c_str(),
                static_cast<unsigned>(cursorPosition.mLine + 1),
                static_cast<unsigned>(cursorPosition.mColumn + 1),
                &unsavedFile,
                1,
                CompletionOptions);

        std::vector<CodeEditorCompletionCandidate> candidates;
        if (results != nullptr) {
            clang_sortCodeCompletionResults(results->Results, results->NumResults);
            std::unordered_map<std::string, std::size_t> seenIndices;

            for (unsigned i = 0; i < results->NumResults; ++i) {
                const auto completionString = results->Results[i].CompletionString;
                const auto cursorKind = results->Results[i].CursorKind;
                const auto availability = clang_getCompletionAvailability(completionString);
                if (availability == CXAvailability_NotAccessible || availability == CXAvailability_NotAvailable) {
                    continue;
                }

                CodeEditorCompletionCandidate candidate;
                candidate.clangPriority = clang_getCompletionPriority(completionString);
                const auto chunkCount = clang_getNumCompletionChunks(completionString);
                for (unsigned chunkIndex = 0; chunkIndex < chunkCount; ++chunkIndex) {
                    const auto kind = clang_getCompletionChunkKind(completionString, chunkIndex);
                    const auto text = ToString(clang_getCompletionChunkText(completionString, chunkIndex));

                    if (kind == CXCompletionChunk_ResultType) {
                        candidate.returnType = text;
                        continue;
                    }

                    if (kind == CXCompletionChunk_Optional) {
                        continue;
                    }

                    if (kind == CXCompletionChunk_TypedText) {
                        candidate.insertText = text;
                    }

                    if (ShouldAppendOpeningParenthesis(kind)) {
                        candidate.appendOpeningParenthesis = true;
                    }

                    candidate.displayText += text;
                }

                if (candidate.insertText.empty()) {
                    continue;
                }

                if (candidate.displayText.empty()) {
                    candidate.displayText = candidate.insertText;
                }

                if (ShouldHideCandidate(cursorKind, candidate)) {
                    continue;
                }

                if (const auto symbolIt = currentFileSymbols.find(candidate.insertText); symbolIt != currentFileSymbols.end()) {
                    candidate.isCurrentFileSymbol = symbolIt->second.isCurrentFileSymbol;
                    candidate.isLocalVariable = symbolIt->second.isLocalVariable;
                    candidate.isField = symbolIt->second.isField;
                } else {
                    candidate.isField = cursorKind == CXCursor_FieldDecl;
                    candidate.isLocalVariable = cursorKind == CXCursor_ParmDecl;
                }

                const auto seenIt = seenIndices.find(candidate.insertText);
                if (seenIt != seenIndices.end()) {
                    auto& existing = candidates[seenIt->second];
                    existing.clangPriority = std::min(existing.clangPriority, candidate.clangPriority);
                    existing.isCurrentFileSymbol = existing.isCurrentFileSymbol || candidate.isCurrentFileSymbol;
                    existing.isLocalVariable = existing.isLocalVariable || candidate.isLocalVariable;
                    existing.isField = existing.isField || candidate.isField;
                    existing.appendOpeningParenthesis = existing.appendOpeningParenthesis || candidate.appendOpeningParenthesis;
                    continue;
                }

                seenIndices.emplace(candidate.insertText, candidates.size());
                candidates.push_back(std::move(candidate));
            }

            clang_disposeCodeCompleteResults(results);
        }

        return FilterAndSortCandidates(std::move(candidates), prefix);
    }
}

CodeEditorAutocomplete::CodeEditorAutocomplete(std::string workspaceRoot)
        : workspaceRoot(std::move(workspaceRoot))
        , worker(&CodeEditorAutocomplete::WorkerMain, this) {
}

CodeEditorAutocomplete::~CodeEditorAutocomplete() {
    {
        std::lock_guard lock(mutex);
        stopRequested = true;
    }

    condition.notify_all();
    if (worker.joinable()) {
        worker.join();
    }
}

void CodeEditorAutocomplete::SetWorkspaceRoot(std::string value) {
    std::lock_guard lock(mutex);
    workspaceRoot = std::move(value);
}

void CodeEditorAutocomplete::SetProjectRoot(std::string value) {
    std::lock_guard lock(mutex);
    projectRoot = std::move(value);
}

void CodeEditorAutocomplete::SetCodeFiles(const std::vector<std::string>& files) {
    std::lock_guard lock(mutex);
    codeIncludeDirectories = BuildCodeIncludeDirectories(files);
}

std::uint64_t CodeEditorAutocomplete::QueueCompletion(const std::string& filePath,
                                                      const LittleCore::TextEditor& editor) {
    if (filePath.empty()) {
        return 0;
    }

    CompletionRequest request;
    request.filePath = filePath;
    request.sourceText = editor.GetText();
    request.cursorPosition = editor.GetCursorPosition();
    request.prefix = BuildCompletionContext(request.sourceText, request.cursorPosition).prefix;

    std::lock_guard lock(mutex);
    if (workspaceRoot.empty()) {
        return 0;
    }

    request.requestId = nextRequestId++;
    const auto requestId = request.requestId;
    pendingRequest = std::move(request);
    condition.notify_one();
    return requestId;
}

std::optional<CodeEditorAutocompleteResult> CodeEditorAutocomplete::TakeCompletedResult() {
    std::lock_guard lock(mutex);
    if (!completedResult.has_value()) {
        return std::nullopt;
    }

    auto result = std::move(completedResult);
    completedResult.reset();
    return result;
}

CodeEditorAutocomplete::ContextSnapshot CodeEditorAutocomplete::CreateContextSnapshotLocked() const {
    ContextSnapshot snapshot;
    snapshot.workspaceRoot = workspaceRoot;
    snapshot.projectRoot = projectRoot;
    snapshot.codeIncludeDirectories = codeIncludeDirectories;
    return snapshot;
}

void CodeEditorAutocomplete::WorkerMain() {
    CXIndex index = clang_createIndex(0, 0);
    CachedTranslationUnit cache;

    while (true) {
        CompletionRequest request;
        ContextSnapshot snapshot;

        {
            std::unique_lock lock(mutex);
            condition.wait(lock, [this] {
                return stopRequested || pendingRequest.has_value();
            });

            if (stopRequested) {
                break;
            }

            request = std::move(*pendingRequest);
            pendingRequest.reset();
            snapshot = CreateContextSnapshotLocked();
        }

        auto result = CodeEditorAutocompleteResult{
                .requestId = request.requestId,
                .filePath = request.filePath,
                .cursorPosition = request.cursorPosition,
                .prefix = request.prefix,
                .candidates = Complete(
                        index,
                        cache,
                        snapshot.workspaceRoot,
                        snapshot.projectRoot,
                        snapshot.codeIncludeDirectories,
                        request.filePath,
                        request.sourceText,
                        request.cursorPosition,
                        request.prefix)
        };

        {
            std::lock_guard lock(mutex);
            if (stopRequested) {
                break;
            }

            completedResult = std::move(result);
        }
    }

    DisposeCachedTranslationUnit(cache);
    clang_disposeIndex(index);
}
