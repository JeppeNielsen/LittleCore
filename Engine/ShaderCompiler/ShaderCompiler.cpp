//
// Created by Jeppe Nielsen on 12/02/2024.
//

#include "ShaderCompiler.hpp"

#include <array>
#include <filesystem>
#include <string>
#include <vector>

#include "args.h"
#include "bytecode.h"
#include "generators/generate.h"
#include "input.h"
#include "reflection.h"
#include "spirv.h"
#include "spirvcross.h"

using namespace LittleCore;

namespace {
    struct SpirvToolProcessScope {
        SpirvToolProcessScope() {
            shdc::Spirv::initialize_spirv_tools();
        }
        ~SpirvToolProcessScope() {
            shdc::Spirv::finalize_spirv_tools();
        }
    };

    void SetError(std::string* errorMessage, const std::string& message) {
        if (errorMessage != nullptr) {
            *errorMessage = message;
        }
    }

    bool TryParseSlang(const std::string& slangName, shdc::Slang::Enum& outSlang) {
        for (int i = 0; i < shdc::Slang::Num; ++i) {
            const auto slang = shdc::Slang::from_index(i);
            if (slangName == shdc::Slang::to_str(slang)) {
                outSlang = slang;
                return true;
            }
        }
        return false;
    }

    bool HasErrorMessages(const std::vector<shdc::ErrMsg>& messages, shdc::ErrMsg::Format format, std::string* errorMessage) {
        for (const auto& message : messages) {
            if (message.type == shdc::ErrMsg::ERROR) {
                SetError(errorMessage, message.as_string(format));
                return true;
            }
        }
        return false;
    }
}

bool ShaderCompiler::CompileSokolBare(const std::string& inputPath,
                                      const std::string& outputPath,
                                      const std::string& slang,
                                      std::string* errorMessage) {
    if (inputPath.empty() || outputPath.empty() || slang.empty()) {
        SetError(errorMessage, "invalid shader compiler input arguments");
        return false;
    }

    shdc::Slang::Enum outputSlang = shdc::Slang::REFLECTION;
    if (!TryParseSlang(slang, outputSlang) || outputSlang == shdc::Slang::REFLECTION) {
        SetError(errorMessage, "unsupported shader slang: " + slang);
        return false;
    }

    shdc::Args args;
    args.valid = true;
    args.exit_code = 0;
    args.input = inputPath;
    args.output = outputPath;
    args.slang = shdc::Slang::bit(outputSlang);
    args.byte_code = false;
    args.output_format = shdc::Format::BARE;
    args.error_format = shdc::ErrMsg::GCC;
    args.no_log_cmdline = true;

    const std::filesystem::path outputPathFs(outputPath);
    const std::filesystem::path outputDirectory = outputPathFs.parent_path();
    if (!outputDirectory.empty()) {
        std::error_code ec;
        std::filesystem::create_directories(outputDirectory, ec);
        if (ec) {
            SetError(errorMessage, "failed to create shader output directory: " + outputDirectory.string());
            return false;
        }
    }

    args.tmpdir = outputDirectory.empty() ? "." : outputDirectory.string();
    if (!args.tmpdir.empty() && args.tmpdir.back() != '/' && args.tmpdir.back() != '\\') {
        args.tmpdir += "/";
    }

    SpirvToolProcessScope spirvScope;

    shdc::Input inp = shdc::Input::load_and_parse(args.input, args.module);
    if (inp.out_error.valid()) {
        SetError(errorMessage, inp.out_error.as_string(args.error_format));
        return false;
    }

    std::array<shdc::Spirv, shdc::Slang::Num> spirv;
    for (int i = 0; i < shdc::Slang::Num; ++i) {
        const auto targetSlang = shdc::Slang::from_index(i);
        if ((args.slang & shdc::Slang::bit(targetSlang)) == 0) {
            continue;
        }

        spirv[i] = shdc::Spirv::compile_glsl_and_extract_bindings(inp, targetSlang, args.defines);
        if (HasErrorMessages(spirv[i].errors, args.error_format, errorMessage)) {
            return false;
        }
    }

    std::array<shdc::Spirvcross, shdc::Slang::Num> spirvcross;
    for (int i = 0; i < shdc::Slang::Num; ++i) {
        const auto targetSlang = shdc::Slang::from_index(i);
        if ((args.slang & shdc::Slang::bit(targetSlang)) == 0) {
            continue;
        }

        spirvcross[i] = shdc::Spirvcross::translate(inp, spirv[i], targetSlang);
        if (spirvcross[i].error.valid()) {
            SetError(errorMessage, spirvcross[i].error.as_string(args.error_format));
            return false;
        }
    }

    std::array<shdc::Bytecode, shdc::Slang::Num> bytecode;
    for (int i = 0; i < shdc::Slang::Num; ++i) {
        const auto targetSlang = shdc::Slang::from_index(i);
        if ((args.slang & shdc::Slang::bit(targetSlang)) == 0) {
            continue;
        }
        if (!args.byte_code && !shdc::Slang::is_spirv(targetSlang)) {
            continue;
        }

        bytecode[i] = shdc::Bytecode::compile(args, inp, spirvcross[i], targetSlang);
        if (HasErrorMessages(bytecode[i].errors, args.error_format, errorMessage)) {
            return false;
        }
    }

    const shdc::refl::Reflection reflection = shdc::refl::Reflection::build(args, inp, spirvcross);
    if (reflection.error.valid()) {
        SetError(errorMessage, reflection.error.as_string(args.error_format));
        return false;
    }

    const shdc::gen::GenInput genInput(args, inp, spirvcross, bytecode, reflection);
    const shdc::ErrMsg generateError = shdc::gen::generate(args.output_format, genInput);
    if (generateError.valid()) {
        SetError(errorMessage, generateError.as_string(args.error_format));
        return false;
    }

    return true;
}
