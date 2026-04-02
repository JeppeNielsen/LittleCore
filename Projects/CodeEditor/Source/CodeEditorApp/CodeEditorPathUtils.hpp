//
// Created by Codex on 02/04/2026.
//

#pragma once
#include <filesystem>
#include <string>

namespace CodeEditorPathUtils {
    inline std::string NormalizePath(const std::string& path) {
        return std::filesystem::path(path).lexically_normal().generic_string();
    }

    inline bool IsPathInsideRoot(const std::filesystem::path& path, const std::filesystem::path& root) {
        const auto relative = path.lexically_relative(root);
        if (relative.empty()) {
            return false;
        }

        for (const auto& part : relative) {
            if (part == "..") {
                return false;
            }
        }

        return true;
    }

    inline std::string MakeDisplayPath(const std::string& path, const std::string& rootPath) {
        if (path.empty()) {
            return {};
        }

        if (rootPath.empty()) {
            return NormalizePath(path);
        }

        const auto normalizedPath = std::filesystem::path(path).lexically_normal();
        const auto normalizedRoot = std::filesystem::path(rootPath).lexically_normal();

        if (normalizedPath == normalizedRoot) {
            return ".";
        }

        if (IsPathInsideRoot(normalizedPath, normalizedRoot)) {
            return normalizedPath.lexically_relative(normalizedRoot).generic_string();
        }

        return normalizedPath.generic_string();
    }
}
