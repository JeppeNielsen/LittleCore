//
// Created by Jeppe Nielsen on 13/04/2024.
//
#include "Engine.hpp"
#include "ImGuiController.hpp"
#include "imgui.h"
#include <vector>
#include "NetImguiServer_App.h"
#include "NetImguiServer_UI.h"
#include "NetImguiServer_RemoteClient.h"
#include <iostream>
#include <bgfx/bgfx.h>
#include "TextureSaver.hpp"

using namespace LittleCore;

static ImGuiController* staticUIController = 0;

static inline bgfx::TextureHandle  toTexHandle (void* p) { return { (uint16_t)((uintptr_t)p) }; }
static inline bgfx::FrameBufferHandle toFbHandle(void* p) { return { (uint16_t)((uintptr_t)p) }; }
static inline void* fromTexHandle (bgfx::TextureHandle h)  { return (void*)(uintptr_t)(h.idx); }
static inline void* fromFbHandle  (bgfx::FrameBufferHandle h){ return (void*)(uintptr_t)(h.idx); }

// Choose/provide a view id per client. If you already track one on the client, use that.
static inline bgfx::ViewId getClientViewId(const NetImguiServer::RemoteClient::Client& client) {
    // If you have client.mViewId, return it here instead.
    return client.mClientIndex;//(bgfx::ViewId)((uint64_t)&client & 0x7F) + 128; // example stable per-client view id
}

// Helper to set up view for a given framebuffer and viewport size
static void setupViewForRT(bgfx::ViewId viewId,
                           bgfx::FrameBufferHandle fb,
                           uint16_t width,
                           uint16_t height,
                           const float clearColor[4])
{
    bgfx::setViewFrameBuffer(viewId, fb);
    //bgfx::setViewRect(viewId, 0, 0, width, height);
    const uint32_t rgba =
            (uint32_t(clearColor[3] * 255.f) << 24) |
            (uint32_t(clearColor[0] * 255.f) << 16) |
            (uint32_t(clearColor[1] * 255.f) <<  8) |
            (uint32_t(clearColor[2] * 255.f) <<  0);
    bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR, rgba);
    // Touch to ensure clear happens even if no draw calls
    bgfx::touch(viewId);
}

namespace NetImguiServer {
    namespace App {

//=================================================================================================
// HAL STARTUP
// Additional initialisation that are platform specific
//=================================================================================================
        bool HAL_Startup(const char* CmdLine) {
            return true;
        }

//=================================================================================================
// HAL SHUTDOWN
// Prepare for shutdown of application, with platform specific code
//=================================================================================================
        void HAL_Shutdown() {

        }

        bool HAL_GetSocketInfo(NetImgui::Internal::Network::SocketInfo* pClientSocket, char* pOutHostname, size_t HostNameLen, int& outPort)
        {
            return false;
        }

//=================================================================================================
// HAL GET USER SETTING FOLDER
// Request the directory where to the 'shared config' clients should be saved
// Return 'nullptr' to disable this feature
//=================================================================================================
        const char* HAL_GetUserSettingFolder()
        {
            return nullptr;
        }

//=================================================================================================
// HAL GET CLIPBOARD UPDATED
// Detect when clipboard had a content change and we should refetch it on the Server and
// forward it to the Clients
//
// Note: We rely on Dear ImGui for Clipboard Get/Set but want to avoid constantly reading then
// converting it to a UTF8 text. If the Server platform doesn't support tracking change,
// return true. If the Server platform doesn't support any clipboard, return false;
//=================================================================================================
        bool HAL_GetClipboardUpdated()
        {

            return false;
        }

//=================================================================================================
// HAL RENDER DRAW DATA
// The drawing of remote clients is handled normally by the standard rendering backend,
// but output is redirected to an allocated client texture  instead default swapchain
//=================================================================================================

        void HAL_RenderDrawData(RemoteClient::Client& client, ImDrawData* pDrawData) {
            if (!client.mpHAL_AreaRT) {
                return;
            }

            const bgfx::FrameBufferHandle fb = toFbHandle(client.mpHAL_AreaRT);
            if (!bgfx::isValid(fb)) {
                return;
            }

            uint16_t rtW = client.mAreaSizeX;
            uint16_t rtH = client.mAreaSizeY;

            setupViewForRT(0, fb, rtW, rtH, client.mBGSettings.mClearColor);
            {
                void* mainBackend = ImGui::GetIO().BackendRendererUserData;
                NetImgui::Internal::ScopedImguiContext scopedCtx(client.mpBGContext);
                ImGui::GetIO().BackendRendererUserData = mainBackend;
                staticUIController->ScheduleDrawData(0, ImGui::GetDrawData());
                bgfx::touch(0);
                bgfx::frame();
            }
            if (pDrawData) {
                staticUIController->ScheduleDrawData(0, pDrawData);
                bgfx::touch(0);
                bgfx::frame();
            }
            bgfx::setViewFrameBuffer(0, BGFX_INVALID_HANDLE);
        }



        void HAL_RenderDrawData_old(RemoteClient::Client& client, ImDrawData* pDrawData)
        {
            //if (!client.mpHAL_AreaRT) {
            //    return;
            //}

            const bgfx::FrameBufferHandle fb = toFbHandle(client.mpHAL_AreaRT);
            if (!bgfx::isValid(fb)) {
                return;
            }

            // Determine target size. Prefer the draw data’s display size; fall back to a stored client size if you have one.
            ImDrawData* mainDD = ImGui::GetDrawData();
            const ImVec2 mainSize = mainDD ? mainDD->DisplaySize : ImVec2(0, 0);
            const ImVec2 altSize  = pDrawData ? pDrawData->DisplaySize : mainSize;

            // Pick something sensible (you can replace with your stored RT width/height if you track them)
            uint16_t rtW = client.mAreaSizeX;//(uint16_t)std::max(0.0f, altSize.x);
            uint16_t rtH = client.mAreaSizeY;//(uint16_t)std::max(0.0f, altSize.y);



            const bgfx::ViewId viewId = getClientViewId(client);

            // Clear the RT
            setupViewForRT(viewId, fb, rtW, rtH, client.mBGSettings.mClearColor);

            // Render ImGui into that view/framebuffer.
            {
                // Use the client's ImGui context while reusing the bgfx backend
                void* mainBackend = ImGui::GetIO().BackendRendererUserData;
                NetImgui::Internal::ScopedImguiContext scopedCtx(client.mpBGContext);

                ImGui::GetIO().BackendRendererUserData = mainBackend;

                // Render whatever is in the *client* context first
                if (ImDrawData* dd = ImGui::GetDrawData()) {
                    staticUIController->ScheduleDrawData(viewId, dd);
                }
            }

            // Optionally, render additional provided draw data on top (matching your GL path)
            if (pDrawData) {
                staticUIController->ScheduleDrawData(viewId, pDrawData);
            }

            bgfx::setViewFrameBuffer(0, BGFX_INVALID_HANDLE);
        }

//=================================================================================================
// HAL CREATE RENDER TARGET
// Allocate RenderTargetView / TextureView for each connected remote client.
// The drawing of their menu content will be outputed in it, then displayed normally
// inside our own 'NetImGui application' Imgui drawing
//=================================================================================================
        bool HAL_CreateRenderTarget(uint16_t Width, uint16_t Height, void*& pOutRT, void*& pOutTexture)
        {
            // Match your original behavior: drop any existing resources.
            HAL_DestroyRenderTarget(pOutRT, pOutTexture);

            // Create a 2D render-target texture (RGBA8, single mip, no MSAA).
            const bgfx::TextureFormat::Enum kFormat = bgfx::TextureFormat::RGBA8;
            const uint64_t kTexFlags = BGFX_TEXTURE_RT; // RT texture; sampler filtering is set when binding.

            bgfx::TextureHandle tex = bgfx::createTexture2D(
                    Width,
                    Height,
                    /* hasMips = */ false,
                    /* numLayers = */ 1,
                    kFormat,
                    kTexFlags
            );

            if (!bgfx::isValid(tex)) {
                return false;
            }

            // Create a framebuffer from that texture (color attachment 0).
            // Pass 'false' so the FB does NOT own/destroy the texture; we return both separately like your API.
            bgfx::FrameBufferHandle fb = bgfx::createFrameBuffer(1, &tex, /* destroyTextures = */ false);

            if (!bgfx::isValid(fb)) {
                bgfx::destroy(tex);
                return false;
            }

            // Success: box the handle indices into your void* outputs.
            pOutRT      = fromFbHandle(fb);
            pOutTexture = fromTexHandle(tex);
            return true;
        }

//=================================================================================================
// HAL DESTROY RENDER TARGET
// Free up allocated resources tried to a RenderTarget
//=================================================================================================
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

//=================================================================================================
// HAL CREATE TEXTURE
// Receive info on a Texture to allocate. At the moment, 'Dear ImGui' default rendering backend
// only support RGBA8 format, so first convert any input format to a RGBA8 that we can use
//=================================================================================================
        bool HAL_CreateTexture(uint16_t Width, uint16_t Height, NetImgui::eTexFormat Format, const uint8_t* pPixelData, ServerTexture& OutTexture)
        {
            // Match original behavior: queue destruction of any existing texture.
            NetImguiServer::App::EnqueueHALTextureDestroy(OutTexture);

            // ---- Normalize to RGBA8 ----
            const size_t texelCount = size_t(Width) * size_t(Height);
            const bgfx::TextureFormat::Enum kFormat = bgfx::TextureFormat::RGBA8;

            const uint8_t* uploadPtr = pPixelData;
            std::vector<uint32_t> a8ToRgbaScratch; // used only for A8

            switch (Format)
            {
                case NetImgui::eTexFormat::kTexFmtA8:
                {
                    a8ToRgbaScratch.resize(texelCount);
                    // Expand A8 to RGBA8 as opaque white tinted by alpha in MSB (A,R,G,B = a,255,255,255)
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

            // ---- Create and upload texture ----
            const uint64_t texFlags = 0; // sampling/filtering is set when binding via BGFX_SAMPLER_* flags
            const uint32_t byteSize  = static_cast<uint32_t>(texelCount * 4u);

            const bgfx::Memory* mem = bgfx::copy(uploadPtr, byteSize);
            if (mem == nullptr) {
                return false;
            }

            bgfx::TextureHandle tex = bgfx::createTexture2D(
                    Width,
                    Height,
                    /*hasMips*/ false,
                    /*numLayers*/ 1,
                    kFormat,
                    texFlags,
                    mem
            );

            if (!bgfx::isValid(tex)) {
                return false;
            }

            // Success: store boxed handle (idx) like your GL path did.
            OutTexture.mpHAL_Texture = fromTexHandle(tex);
            return true;
        }

//=================================================================================================
// HAL DESTROY TEXTURE
// Free up allocated resources tried to a Texture
//=================================================================================================
        void HAL_DestroyTexture(ServerTexture& OutTexture)
        {
            if (OutTexture.mpHAL_Texture != nullptr) {
                bgfx::TextureHandle tex = toTexHandle(OutTexture.mpHAL_Texture);
                if (bgfx::isValid(tex)) {
                    bgfx::destroy(tex);
                }
            }

            // Reset the struct to zeroed state like original code
            std::memset(&OutTexture, 0, sizeof(OutTexture));
        }

        bool CreateTexture_Custom( ServerTexture& serverTexture, const NetImgui::Internal::CmdTexture& cmdTexture, uint32_t customDataSize )
        {
            return false;
        }

//=================================================================================================
// Return true if the command was handled
        bool DestroyTexture_Custom( ServerTexture& serverTexture, const NetImgui::Internal::CmdTexture& cmdTexture, uint32_t customDataSize )
        {

            return false;
        }




    }} //namespace NetImguiServer { namespace App












// Example data structure for nodes
struct TreeNode {
    std::string name;
    std::vector<TreeNode*> children;
    TreeNode* parent;
};

static TreeNode* root = nullptr;

void ShowDragAndDropTree(TreeNode* node, TreeNode* parent = nullptr) {
    if (ImGui::TreeNode(node->name.c_str())) {

        // Drag source
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None | ImGuiDragDropFlags_AcceptBeforeDelivery)) {
            node->parent = parent;
            ImGui::SetDragDropPayload("DND_TREE_NODE", &node, sizeof(TreeNode*)); // Set payload
            ImGui::Text("Dragging: %s", node->name.c_str());
            ImGui::EndDragDropSource();
        }

        // Drop target
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TREE_NODE")) {
                TreeNode* draggedNode = *(TreeNode**)payload->Data;
                if (draggedNode != node) {
                    if (draggedNode->parent!= nullptr) {
                        auto it = std::find(draggedNode->parent->children.begin(), draggedNode->parent->children.end(),draggedNode);
                        draggedNode->parent->children.erase(it);
                    }
                    node->children.push_back(draggedNode); // Add the dragged node to this node's children
                }
            }
            ImGui::EndDragDropTarget();
        }

        // Recursively draw child nodes
        for (auto child : node->children) {
            ShowDragAndDropTree(child, node);
        }

        ImGui::TreePop();
    }
}

void ShowTreeWithDragAndDrop() {
    if (root == nullptr) {

        root = new TreeNode();
        root->name = "Root";

        auto child1 = new TreeNode();
        child1->name = "Child1";

        auto child2 = new TreeNode();
        child2->name = "Child2";

        auto subChild1 = new TreeNode();
        subChild1->name = "SubChild1";

        auto subChild2 = new TreeNode();
        subChild2->name = "SubChild2";

        root->children.push_back(child1);
        root->children.push_back(child2);

        child1->children.push_back(subChild1);
        child2->children.push_back(subChild2);




    }

    if (ImGui::Begin("Drag and Drop Tree View")) {
        ShowDragAndDropTree(root);
    }
    ImGui::End();
}


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
struct ImguiTest : IState {

    ImGuiController gui;

    Color backgroundColor;

    bool showWindow = false;
    
    int value;

    void Initialize() override {

        staticUIController = &gui;

        std::string cmdArgs;

        gui.Initialize(mainWindow, [this]() {
            OnGUI();
        });

        gui.LoadFont("/Users/jeppe/Jeppes/LittleCore/Projects/TestImGui/Source/Fonts/LucidaG.ttf", 12);

        bool ok = NetImguiServer::App::Startup(cmdArgs.c_str());

        if (!ok) {
            std::cout << "NetIMGUI lib failed\n";
        }

    }

    void HandleEvent(void* event) override {
        gui.HandleEvent(event);
    }

    void OnGUI() {



        ImGui::DockSpaceOverViewport();




        ImGui::Begin("My test window");


        if (ImGui::Button("Show extra window")) {
            showWindow = !showWindow;
        }

        if( ImGui::IsItemHovered() )
            ImGui::SetTooltip("Terminate this sample.");

        ImGui::End();

/*
        ImGui::Begin("Window 2");

        ImGui::ColorEdit4("Color", &backgroundColor.r);

        ImGui::End();
        
        ImGui::Begin("Window 3");

        ImGui::ColorEdit4("Color", &backgroundColor.r);

        ImGui::End();
		
        if (showWindow) {
            ImGui::Begin("Extra window");
            
            ImGui::DragInt("Test int", &value);

            ImGui::End();
        }
*/
        //ImGui::ShowDemoWindow();

        //ShowTreeWithDragAndDrop();

        NetImguiServer::UI::DrawImguiContent();



    }

    void Update(float dt) override {

    }

    void Render() override {
        NetImguiServer::App::UpdateRemoteContent();

        gui.Render();
    }

    ~ImguiTest() {
        NetImguiServer::App::Shutdown();
    }

};

int main() {
    Engine e({"Test Netimgui"});
    e.Start<ImguiTest>();
    return 0;
}

