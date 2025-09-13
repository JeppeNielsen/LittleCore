//
// Created by Jeppe Nielsen on 13/09/2025.
//

#include "NetimguiController.hpp"
#include "NetImguiServer_App.h"
#include "NetImguiServer_UI.h"
#include "NetImguiServer_RemoteClient.h"
#include <bgfx/bgfx.h>

using namespace LittleCore;


static ImGuiController* staticUIController = 0;

static inline bgfx::TextureHandle  toTexHandle (void* p) { return { (uint16_t)((uintptr_t)p) }; }
static inline bgfx::FrameBufferHandle toFbHandle(void* p) { return { (uint16_t)((uintptr_t)p) }; }
static inline void* fromTexHandle (bgfx::TextureHandle h)  { return (void*)(uintptr_t)(h.idx); }
static inline void* fromFbHandle  (bgfx::FrameBufferHandle h){ return (void*)(uintptr_t)(h.idx); }

static inline bgfx::ViewId getClientViewId(const NetImguiServer::RemoteClient::Client& client) {
    return client.mClientIndex;//(bgfx::ViewId)((uint64_t)&client & 0x7F) + 128; // example stable per-client view id
}

static void setupViewForRT(bgfx::ViewId viewId,
                           bgfx::FrameBufferHandle fb,
                           uint16_t width,
                           uint16_t height,
                           const float clearColor[4])
{
    bgfx::setViewFrameBuffer(viewId, fb);
    const uint32_t rgba =
            (uint32_t(clearColor[3] * 255.f) << 24) |
            (uint32_t(clearColor[0] * 255.f) << 16) |
            (uint32_t(clearColor[1] * 255.f) <<  8) |
            (uint32_t(clearColor[2] * 255.f) <<  0);
    bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR, rgba);
    bgfx::touch(viewId);
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

            const bgfx::FrameBufferHandle fb = toFbHandle(client.mpHAL_AreaRT);
            if (!bgfx::isValid(fb)) {
                return;
            }

            setupViewForRT(0, fb, client.mAreaSizeX, client.mAreaSizeY, client.mBGSettings.mClearColor);
            {
                void* mainBackend = ImGui::GetIO().BackendRendererUserData;
                NetImgui::Internal::ScopedImguiContext scopedCtx(client.mpBGContext);
                ImGui::GetIO().BackendRendererUserData = mainBackend;
                staticUIController->Draw(0, ImGui::GetDrawData());
                bgfx::touch(0);
                bgfx::frame();
            }
            if (pDrawData) {
                staticUIController->Draw(0, pDrawData);
                bgfx::touch(0);
                bgfx::frame();
            }
            bgfx::setViewFrameBuffer(0, BGFX_INVALID_HANDLE);
        }

        bool HAL_CreateRenderTarget(uint16_t Width, uint16_t Height, void*& pOutRT, void*& pOutTexture)
        {
            HAL_DestroyRenderTarget(pOutRT, pOutTexture);

            const bgfx::TextureFormat::Enum kFormat = bgfx::TextureFormat::RGBA8;
            const uint64_t kTexFlags = BGFX_TEXTURE_RT; // RT texture; sampler filtering is set when binding.

            bgfx::TextureHandle tex = bgfx::createTexture2D(
                    Width,
                    Height,
                    false,
                    1,
                    kFormat,
                    kTexFlags
            );

            if (!bgfx::isValid(tex)) {
                return false;
            }

            bgfx::FrameBufferHandle fb = bgfx::createFrameBuffer(1, &tex, /* destroyTextures = */ false);

            if (!bgfx::isValid(fb)) {
                bgfx::destroy(tex);
                return false;
            }

            pOutRT      = fromFbHandle(fb);
            pOutTexture = fromTexHandle(tex);
            return true;
        }

        void HAL_DestroyRenderTarget(void*& pRT, void*& pTexture)
        {
            if (pRT != nullptr) {
                bgfx::FrameBufferHandle fb = toFbHandle(pRT);
                if (bgfx::isValid(fb)) { bgfx::destroy(fb); }
                pRT = nullptr;
            }
            if (pTexture != nullptr) {
                bgfx::TextureHandle tex = toTexHandle(pTexture);
                if (bgfx::isValid(tex)) { bgfx::destroy(tex); }
                pTexture = nullptr;
            }
        }

        bool HAL_CreateTexture(uint16_t Width, uint16_t Height, NetImgui::eTexFormat Format, const uint8_t* pPixelData, ServerTexture& OutTexture)
        {
            NetImguiServer::App::EnqueueHALTextureDestroy(OutTexture);

            const size_t texelCount = size_t(Width) * size_t(Height);
            const bgfx::TextureFormat::Enum kFormat = bgfx::TextureFormat::RGBA8;

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

            const uint64_t texFlags = 0;
            const uint32_t byteSize  = static_cast<uint32_t>(texelCount * 4u);

            const bgfx::Memory* mem = bgfx::copy(uploadPtr, byteSize);
            if (mem == nullptr) {
                return false;
            }

            bgfx::TextureHandle tex = bgfx::createTexture2D(
                    Width,
                    Height,
                    false,
                    1,
                    kFormat,
                    texFlags,
                    mem
            );

            if (!bgfx::isValid(tex)) {
                return false;
            }

            OutTexture.mpHAL_Texture = fromTexHandle(tex);
            return true;
        }

        void HAL_DestroyTexture(ServerTexture& OutTexture)
        {
            if (OutTexture.mpHAL_Texture != nullptr) {
                bgfx::TextureHandle tex = toTexHandle(OutTexture.mpHAL_Texture);
                if (bgfx::isValid(tex)) {
                    bgfx::destroy(tex);
                }
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




NetimguiController::NetimguiController(ImGuiController& guiController) : guiController(guiController) {
    staticUIController = &guiController;
}

bool NetimguiController::Start() {
    std::string cmdArgs;
    return NetImguiServer::App::Startup(cmdArgs.c_str());
}

void NetimguiController::Update() {
    NetImguiServer::App::UpdateRemoteContent();
}

void NetimguiController::Draw() {
    NetImguiServer::UI::DrawImguiContent();
}

NetimguiController::~NetimguiController() {
    NetImguiServer::App::Shutdown();
    staticUIController = nullptr;
}


