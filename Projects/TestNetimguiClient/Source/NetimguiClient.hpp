//
// Created by Jeppe Nielsen on 13/09/2025.
//


#pragma once
#include <string>

namespace LittleCore {

    class NetimguiClient {

    public:

        ~NetimguiClient();

        bool Start();
        void Connect(const std::string& name, const std::string& serverHost);
        void ShutDown();

        bool IsConnectionPending();
        bool IsConnected();

    };


}