//
// Created by Jeppe Nielsen on 13/04/2024.
//
#include "Engine.hpp"
#include "ImGuiController.hpp"
#include "imgui.h"
#include "TextEditor.hpp"
#include "FileHelper.hpp"
#include "Autocompleter.hpp"
#include <iostream>
#include "Timer.hpp"

using namespace LittleCore;

struct Color {
    float r,g,b,a;

    uint32_t GetUint() {
        uint8_t red = (255)*r;
        uint8_t green = (255)*g;
        uint8_t blue = (255)*b;
        uint8_t alpha = (255)*a;

        return red << 24 | green << 16 | blue << 8 | alpha;
    }

};

/*
int CreateTranslationUnit() {
    std::string clang = clangPath + " ";

    for (auto cppFile : context.cppFiles) {
        clang += cppFile + " ";
    }

    if (!context.hppFiles.empty()) {
        clang += " ";
        for (auto hppFile: context.hppFiles) {
            clang += "-I" + hppFile + " ";
        }
    }

    clang += "-isysroot `xcrun -sdk macosx --show-sdk-path` ";
    clang += "-std=c++11 -dynamiclib -Wno-return-type-c-linkage ";
    clang += "-o " + context.outputPath + " ";

    CXIndex index = clang_createIndex(0,1);
    CXTranslationUnit tu  = clang_parseTranslationUnit(index, path.c_str(), &arguments[0], (int)arguments.size(), NULL, 0, 0);


}
 */

struct ImguiTest : IState {

    ImGuiController gui;

    Color backgroundColor;

    bool showWindow = false;

    TextEditor editor;
    ImFont* editorFont;

    void Initialize() override {

        gui.Initialize(mainWindow, [this]() {
            OnGUI();
        });

        gui.LoadFont("/Users/jeppe/Jeppes/LittleCore/Projects/TestTextEditor/Source/Fonts/LucidaG.ttf", 12);
        editorFont = gui.LoadFont("/Users/jeppe/Jeppes/LittleCore/Projects/TestTextEditor/Source/Fonts/JetBrainsMono-Regular.ttf", 16);

        editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());

        auto text = FileHelper::ReadAllText("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Scripts/ConsoleWindow/Console.cpp");
        editor.SetText(text);

        TextEditor::ErrorMarkers errorMarkers;
        errorMarkers[30] = "syntax error";

        editor.SetErrorMarkers(errorMarkers);
        editor.SetPalette(TextEditor::GetLightPalette());
    }

    void HandleEvent(void* event) override {
        gui.HandleEvent(event);
    }

    void OnGUI() {

        ImGui::DockSpaceOverViewport();

        ImGui::Begin("My test window");


        ImGui::PushFont(editorFont);
        editor.Render("Editor");
        ImGui::PopFont();

        ImGui::End();

        ImGui::Begin("Set errors");

        /*if (ImGui::Button("Show extra window")) {
            showWindow = !showWindow;

            TextEditor::ErrorMarkers errorMarkers;
            errorMarkers[50] = "Extra line";

            editor.SetErrorMarkers(errorMarkers);

            TextEditor::Breakpoints breakPoints;
            breakPoints.insert(30);
            breakPoints.insert(60);

            editor.SetBreakpoints(breakPoints);
        }
         */

        if (ImGui::Button("Test")) {

            /*
            std::vector<std::string> hppFiles;

            hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/");
            hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Scripts/Files");

            hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/");
            hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/misc/cpp/");

            std::string path = "/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Scripts/ConsoleWindow/Console.cpp";

            std::string clang = "";

            if (!hppFiles.empty()) {
                clang += " ";
                for (auto hppFile : hppFiles) {
                    clang += "-I" + hppFile + " ";
                }
            }

            clang += "-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX12.3.sdk ";
            clang += "-std=c++11 -dynamiclib -Wno-return-type-c-linkage ";
             */

            std::string path = "/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Scripts/ConsoleWindow/Console.cpp";
            std::string pathUnsaved = "/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Scripts/ConsoleWindow/Console_unsaved.cpp";


            /*
            const char* arguments[7];
            arguments[0] = "-isysroot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX12.3.sdk";
            arguments[1] = "-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX12.3.sdk/System/Library/Frameworks/Kernel.framework/Headers/";
            arguments[2] = "-std=c++11";
            arguments[3] = "-I/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/";
            arguments[4] = "-I/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Scripts/Files/";
            arguments[5] = "-I/Users/jeppe/Jeppes/LittleCore/External/imgui/";
            arguments[6] = "-I/Users/jeppe/Jeppes/LittleCore/External/imgui/misc/cpp/";
            */

            std::vector<const char*> arguments = {
                //"-isysroot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk",
                "-std=c++14",
                "-stdlib=libc++",
                "-I/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/",
                "-I/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Scripts/Files/",
                "-I/Users/jeppe/Jeppes/LittleCore/External/imgui/",
                "-I/Users/jeppe/Jeppes/LittleCore/External/imgui/misc/cpp/",
                "-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib",
                "-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include",
                "-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/c++/v1",
                "-I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/13.1.6/include",
                    //"-I/Users/jeppe/Jeppes/Scripting/clang/include/c++/v1",
                //"-I/Users/jeppe/Jeppes/Scripting/clang/lib/clang/16/lib/darwin",
                //"-I/Users/jeppe/Jeppes/Scripting/clang/lib",

                //"-_LIBCPP_USING_IF_EXISTS=1"

                //"-isysroot/Users/jeppe/Jeppes/Scripting/clang",
                "-v"

                //"-I/Users/jeppe/Jeppes/Scripting/clang/include/",


            };

            /*
            std::vector<const char*> arguments = {
                    "-std=c++17",
                   "-stdlib=libc++",

                    "-xc++",
                    "-DMLIR_CUDA_CONVERSIONS_ENABLED=1",
                    "-DMLIR_ROCM_CONVERSIONS_ENABLED=1",
                    "-DNDEBUG",
                    "-D_DEBUG",
                    "-D_GLIBCXX_ASSERTIONS",
                    "-D_LIBCPP_ENABLE_HARDENED_MODE",
                    "-D__STDC_CONSTANT_MACROS",
                    "-D__STDC_FORMAT_MACROS",
                    "-D__STDC_LIMIT_MACROS",


                    "-I/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/",
                    "-I/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Scripts/Files/",
                    "-I/Users/jeppe/Jeppes/LittleCore/External/imgui/",
                    "-I/Users/jeppe/Jeppes/LittleCore/External/imgui/misc/cpp/",

                    "-I/Users/jeppe/Jeppes/Scripting/clang/include/",
                    "-I/Users/jeppe/Jeppes/Scripting/clang/lib/clang/16/lib/darwin",
                    "-I/Users/jeppe/Jeppes/Scripting/clang/include/c++/v1",
                    "-isysroot/Users/jeppe/Jeppes/Scripting/clang/",

                    "-arch", "arm64",
                    "-fPIC",
                    "-fcolor-diagnostics",
                    "-fdiagnostics-color",
                    "-fno-exceptions",
                    "-funwind-tables",
                    "-fvisibility-inlines-hidden",
                    "-pedantic",
                    "-v"

            };

             */
            //arguments.push_back("-std=c++17");
            //arguments.push_back("-stdlib=libc++");
            //arguments.push_back("-std=c99");

            //arguments.push_back("-I/Users/jeppe/Jeppes/Scripting/clang/include/c++/v1");
            //arguments.push_back("-I/Users/jeppe/Jeppes/Scripting/clang/lib/clang/16/include");

            //arguments.push_back("-I/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/");
            //arguments.push_back("-I/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Scripts/Files/");
            //arguments.push_back("-I/Users/jeppe/Jeppes/LittleCore/External/imgui/");
            //arguments.push_back("-I/Users/jeppe/Jeppes/LittleCore/External/imgui/misc/cpp/");
            //arguments.push_back("-isysroot $(xcrun --show-sdk-path)");
            //arguments.push_back("-I/Users/jeppe/Jeppes/Scripting/clang/include/c++/v1");
            //arguments.push_back("-I/Users/jeppe/Jeppes/Scripting/clang/include/");
            //arguments.push_back("-L/Users/jeppe/Jeppes/Scripting/clang/lib/");

            //arguments.push_back("-isysroot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk");
            //arguments.push_back("-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include");

            //arguments.push_back("-I/Users/jeppe/Jeppes/Scripting/clang/include/c++/v1");
            //arguments.push_back("-I/Users/jeppe/Jeppes/Scripting/clang/lib/clang/16/include");

            //arguments.push_back("-I/Users/jeppe/Jeppes/Scripting/clang/include/");
            //arguments.push_back("-I/Users/jeppe/Jeppes/Scripting/clang/lib/");
            //arguments.push_back("-isysroot/Users/jeppe/Jeppes/Scripting/clang/lib");
            //arguments.push_back("-L/Users/jeppe/Jeppes/Scripting/clang/lib -lc++ -lc++abi -lm");

            //arguments.push_back("-I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++");
            //arguments.push_back("-I/usr/include");

            //arguments.push_back("-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX12.3.sdk/Developer/SDKs/MacOSX.sdk/usr/include");
            //arguments.push_back("-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX12.3.sdk/System/Library/Frameworks/Kernel.framework/Headers/");
            //arguments.push_back("-I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++");
            //arguments.push_back("-I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1");
            //arguments.push_back("-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include");
            //arguments.push_back("-I/usr/include");
            //arguments.push_back("-I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include");
            //arguments.push_back("-I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++");
            //arguments.push_back("-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include");


            //arguments.push_back("-I/Users/Jeppe/Downloads/clang+llvm-16.0.4-arm64-apple-darwin22.0/include/c++/v1");
            //arguments.push_back("-I/Users/Jeppe/Downloads/clang+llvm-16.0.4-arm64-apple-darwin22.0/lib/clang/16/include");


            CXIndex index = clang_createIndex(0,1);
            CXTranslationUnit tu  = clang_parseTranslationUnit(index, path.c_str(), &arguments[0], (int)arguments.size(), NULL, 0, 0);

            Autocompleter autocompleter;


            //std::vector<Result> DoAutoComplete(CXTranslationUnit translationUnit, const std::string& filename, const std::string& unsavedStr, int lineNo, int columnNo);

            auto unsavedStr = FileHelper::ReadAllText(path);

            {
                Timer timer;
                timer.Start();
                auto results = autocompleter.DoAutoComplete(tu, path, unsavedStr, 42, 22);
                float duration = timer.Stop();


                for (auto result: results) {
                    std::cout << "GetText(): " << result.GetText() << "\n";
                    std::cout << "GetDisplayedText() : " << result.GetDisplayedText() << "\n";
                    for (auto f: result.chunks) {
                        std::cout << "     " << f.text << "\n";
                    }
                }

                std::cout << "Code completion took = " << std::to_string(duration) << " seconds \n";

            }


        }



        ImGui::End();

        ImGui::Begin("Window 2");

        ImGui::ColorEdit4("Color", &backgroundColor.r);

        ImGui::End();

        if (showWindow) {
            ImGui::Begin("Extra window");

            ImGui::End();
        }

        ImGui::ShowDemoWindow();
    }

    void Update(float dt) override {

    }

    void Render() override {
        gui.Render();
    }

};

int main() {
    Engine e({"Test Text Editor"});
    e.Start<ImguiTest>();
    return 0;
}
