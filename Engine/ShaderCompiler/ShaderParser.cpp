//
// Created by Jeppe Nielsen on 12/02/2024.
//

#include "ShaderParser.hpp"
#include <iostream>

using namespace LittleCore;

bool TryParseShader(const std::string &shader, int startOffset, int &startIndex, int &endIndex) {
    const char startToken = '{';
    const char endToken = '}';

    int depth = 0;
    for (int i=startOffset; i<shader.size(); ++i) {
        if (shader[i]==startToken) {
            ++depth;
            if (depth == 1) {
                startIndex = i;
            }
        } else if (shader[i]==endToken) {
            --depth;
            if (depth == 0) {
                endIndex = i;
                return true;
            }
        }
        //std::cout << shader[i];
    }
    return false;
}

ShaderParserResult ShaderParser::TryParse(const std::string &shaderSource) {
    ShaderParserResult result;
    result.succes = false;

    int fragmentOffset = 0;
    {
        int startIndex = 0;
        int endIndex = 0;
        if (!TryParseShader(shaderSource, 0, startIndex, endIndex)) {
            std::cout << "Could not parse shader file"<< std::endl;
            return result;
        }
        result.varyings = shaderSource.substr(startIndex+1, (endIndex - startIndex)-2);
        fragmentOffset = endIndex + 1;
    }

    {
        int startIndex = 0;
        int endIndex = 0;
        if (!TryParseShader(shaderSource, fragmentOffset, startIndex, endIndex)) {
            std::cout << "Could not parse shader file"<< std::endl;
            return result;
        }
        result.vertex = shaderSource.substr(startIndex+1, (endIndex - startIndex)-2);
        fragmentOffset = endIndex + 1;
    }

    {
        int startIndex = 0;
        int endIndex = 0;
        if (!TryParseShader(shaderSource, fragmentOffset, startIndex, endIndex)) {
            std::cout << "Could not parse shader file"<< std::endl;
            return result;
        }
        result.fragment = shaderSource.substr(startIndex+1, (endIndex - startIndex)-2);
    }

    result.succes = true;
    return result;
}
