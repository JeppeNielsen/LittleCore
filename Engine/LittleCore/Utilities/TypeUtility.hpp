//
// Created by Jeppe Nielsen on 29/12/2024.
//

#pragma once
#include <string>

namespace LittleCore {

    class TypeUtility {
    private:

        template<typename Class>
        static std::string GetClassNameInternal() {
#ifdef WIN32
            const std::string functionName = __FUNCTION__;
#else
            const std::string functionName = __PRETTY_FUNCTION__;
#endif
            const std::string token = "Class = ";
            size_t equal = functionName.find(token) + token.size();
            return functionName.substr(equal, functionName.size() - equal - 1);
        }

    public:

        template<typename Class>
        static std::string& GetClassName() {
            static std::string name = GetClassNameInternal<Class>();
            return name;
        }
    };
}
