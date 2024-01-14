//
// Created by Jeppe Nielsen on 14/01/2024.
//

#pragma once



struct ModuleStateFactory {

    template<typename T, typename...Args>
    T* CreateState(Args&& ...args) {
        return new T(args...);
    }

};
