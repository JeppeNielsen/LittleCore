//
// Created by Jeppe Nielsen on 13/09/2025.
//

#include "NetimguiServerController.hpp"
#include "NetImguiServer_App.h"
#include "NetImguiServer_UI.h"
#include "NetImguiServer_RemoteClient.h"
#include "SokolDirect.hpp"
#include <cstring>
#include <vector>

using namespace LittleCore;


static ImGuiController* staticUIController = 0;

static inline sg_image toTexHandle(void* p) {
    return {static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p))};
}

static inline sg_attachments toFbHandle(void* p) {
    return {static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p))};
}

static inline void* fromTexHandle(sg_image h) {
    return reinterpret_cast<void*>(static_cast<uintptr_t>(h.id));
}

static inline void* fromFbHandle(sg_attachments h) {
    return reinterpret_cast<void*>(static_cast<uintptr_t>(h.id));
}

namespace NetImguiServer {
    namespace App {

        bool HAL_Startup(const char* CmdLine) {
            return true;
        }

        void HAL_Shutdown() {

        }

        bool HAL_GetSocketInfo(NetImgui::Internal::Network::SocketInfo* pClientSocket, char* pOutHostname, size_t HostNameLen, int& outPort)
        {
            return false;
        }

        const char* HAL_GetUserSettingFolder()
        {
            return nullptr;
        }

        bool HAL_GetClipboardUpdated()
        {
            return false;
        }

        void HAL_RenderDrawData(RemoteClient::Client& client, ImDrawData* pDrawData) {
            if (!client.mpHAL_AreaRT) {
                return;
            }

            const sg_attachments fb = toFbHandle(client.mpHAL_AreaRT);
            if (!lc_sg_valid(fb)) {
                return;
            }

            sg_pass_action passAction{};
            passAction.colors[0].load_action = SG_LOADACTION_CLEAR;
            passAction.colors[0].store_action = SG_STOREACTION_STORE;
            passAction.colors[0].clear_value = {
                    client.mBGSettings.mClearColor[0],
                    client.mBGSettings.mClearColor[1],
                    client.mBGSettings.mClearColor[2],
                    client.mBGSettings.mClearColor[3]
            };
            sg_pass pass{};
            pass.action = passAction;
            pass.attachments = fb;
            sg_begin_pass(pass);
            {
                void* mainBackend = ImGui::GetIO().BackendRendererUserData;
                NetImgui::Internal::ScopedImguiContext scopedCtx(client.mpBGContext);
                ImGui::GetIO().BackendRendererUserData = mainBackend;
                staticUIController->Draw(0, ImGui::GetDrawData());
            }
            if (pDrawData) {
                staticUIController->Draw(0, pDrawData);
            }
            sg_end_pass();
        }

        bool HAL_CreateRenderTarget(uint16_t Width, uint16_t Height, void*& pOutRT, void*& pOutTexture)
        {
            HAL_DestroyRenderTarget(pOutRT, pOutTexture);

            sg_image_desc imageDesc{};
            imageDesc.render_target = true;
            imageDesc.width = Width;
            imageDesc.height = Height;
            imageDesc.pixel_format = SG_PIXELFORMAT_BGRA8;
            sg_image tex = sg_make_image(imageDesc);

            if (!lc_sg_valid(tex)) {
                return false;
            }

            sg_attachments_desc attachmentsDesc{};
            attachmentsDesc.colors[0].image = tex;
            sg_attachments fb = sg_make_attachments(attachmentsDesc);

            if (!lc_sg_valid(fb)) {
                lc_sg_destroy(tex);
                return false;
            }

            pOutRT      = fromFbHandle(fb);
            pOutTexture = fromTexHandle(tex);
            return true;
        }

        void HAL_DestroyRenderTarget(void*& pRT, void*& pTexture)
        {
            if (pRT != nullptr) {
                sg_attachments fb = toFbHandle(pRT);
                lc_sg_destroy(fb);
                pRT = nullptr;
            }
            if (pTexture != nullptr) {
                sg_image tex = toTexHandle(pTexture);
                lc_sg_destroy(tex);
                pTexture = nullptr;
            }
        }

        bool HAL_CreateTexture(uint16_t Width, uint16_t Height, NetImgui::eTexFormat Format, const uint8_t* pPixelData, ServerTexture& OutTexture)
        {
            NetImguiServer::App::EnqueueHALTextureDestroy(OutTexture);

            const size_t texelCount = size_t(Width) * size_t(Height);

            const uint8_t* uploadPtr = pPixelData;
            std::vector<uint32_t> a8ToRgbaScratch;

            switch (Format)
            {
                case NetImgui::eTexFormat::kTexFmtA8:
                {
                    a8ToRgbaScratch.resize(texelCount);
                    for (size_t i = 0; i < texelCount; ++i) {
                        const uint32_t a = static_cast<uint32_t>(pPixelData[i]);
                        a8ToRgbaScratch[i] = (a << 24) | 0x00FFFFFFu;
                    }
                    uploadPtr = reinterpret_cast<const uint8_t*>(a8ToRgbaScratch.data());
                    break;
                }

                case NetImgui::eTexFormat::kTexFmtRGBA8:
                    // already RGBA8 tightly packed
                    break;

                default:
                    // Unsupported format
                    return false;
            }

            sg_image_desc imageDesc{};
            imageDesc.width = Width;
            imageDesc.height = Height;
            imageDesc.pixel_format = SG_PIXELFORMAT_RGBA8;
            imageDesc.usage = SG_USAGE_IMMUTABLE;
            imageDesc.data.subimage[0][0] = {uploadPtr, texelCount * 4u};
            sg_image tex = sg_make_image(imageDesc);

            if (!lc_sg_valid(tex)) {
                return false;
            }

            OutTexture.mpHAL_Texture = fromTexHandle(tex);
            return true;
        }

        void HAL_DestroyTexture(ServerTexture& OutTexture)
        {
            if (OutTexture.mpHAL_Texture != nullptr) {
                sg_image tex = toTexHandle(OutTexture.mpHAL_Texture);
                lc_sg_destroy(tex);
            }

            std::memset(&OutTexture, 0, sizeof(OutTexture));
        }

        bool CreateTexture_Custom( ServerTexture& serverTexture, const NetImgui::Internal::CmdTexture& cmdTexture, uint32_t customDataSize )
        {
            return false;
        }

        bool DestroyTexture_Custom( ServerTexture& serverTexture, const NetImgui::Internal::CmdTexture& cmdTexture, uint32_t customDataSize )
        {
            return false;
        }
    }}




NetimguiServerController::NetimguiServerController(ImGuiController& guiController) : guiController(guiController) {
    staticUIController = &guiController;
}

bool NetimguiServerController::Start() {
    std::string cmdArgs;
    return NetImguiServer::App::Startup(cmdArgs.c_str());
}

void NetimguiServerController::Update() {
    NetImguiServer::App::UpdateRemoteContent();
}

void NetimguiServerController::Draw() {
    NetImguiServer::UI::DrawImguiContent();
}

NetimguiServerController::~NetimguiServerController() {
    NetImguiServer::App::Shutdown();
    staticUIController = nullptr;
}
