#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui_impl_sdl_bgfx.hpp"

#include "fs_ocornut_imgui.bin.h"
#include "vs_ocornut_imgui.bin.h"

#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>
#include <bx/math.h>
#include <bx/timer.h>
#include "imgui.h"
#include <SDL3/SDL.h>
#include <build_config/SDL_build_config.h>
#include "backends/imgui_impl_sdl3.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>

//using namespace LittleCore;

static uint8_t main_view_id{255};
static bool is_init{false};
static bgfx::TextureHandle font_texture = BGFX_INVALID_HANDLE;
static bgfx::ProgramHandle shader_handle = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle uniform_texture = BGFX_INVALID_HANDLE;
static bgfx::VertexLayout vertex_layout;
static std::vector<bgfx::ViewId> free_view_ids;
static bgfx::ViewId sub_view_id = 200;
static std::function<void()> renderFunction;
static int lastWindowWidth = 0;
static int lastWindowHeight = 0;


static bgfx::ViewId allocate_view_id()
{
  if (!free_view_ids.empty())
  {
    const bgfx::ViewId id = free_view_ids.back();
    free_view_ids.pop_back();
    return id;
  }
  return sub_view_id++;
}

static void free_view_id(bgfx::ViewId id)
{
  free_view_ids.push_back(id);
}

static const bgfx::EmbeddedShader s_embeddedShaders[] = {BGFX_EMBEDDED_SHADER(vs_ocornut_imgui),
                                                         BGFX_EMBEDDED_SHADER(fs_ocornut_imgui),
                                                         BGFX_EMBEDDED_SHADER_END()};

bool checkAvailTransientBuffers(uint32_t _numVertices, const bgfx::VertexLayout &_layout,
                                uint32_t _numIndices)
{
  return _numVertices == bgfx::getAvailTransientVertexBuffer(_numVertices, _layout) &&
         (0 == _numIndices || _numIndices == bgfx::getAvailTransientIndexBuffer(_numIndices));
}

enum class BgfxTextureFlags : uint32_t
{
  Opaque = 1u << 31,
  PointSampler = 1u << 30,
  All = Opaque | PointSampler,
};

unsigned long native_window_handle(ImGuiViewport* viewport, SDL_Window* window)
{
#if defined(__WIN32__) && !defined(__WINRT__)
  return (unsigned long)SDL_GetProperty(SDL_GetWindowProperties(window), "SDL.window.win32.hwnd", NULL);
#elif defined(__APPLE__) && defined(SDL_VIDEO_DRIVER_COCOA)
  return (unsigned long)SDL_GetProperty(SDL_GetWindowProperties(window), "SDL.window.cocoa.window", NULL);
#elif defined(__LINUX__) && defined(SDL_VIDEO_DRIVER_X11)
  auto handle = SDL_GetNumberProperty(SDL_GetWindowProperties(window), "SDL.window.x11.window", 0);
  return handle;
#endif
  return 0;
}

struct imgui_viewport_data
{
  bgfx::FrameBufferHandle frameBufferHandle;
  bgfx::ViewId viewId = 0;
  uint16_t width = 0;
  uint16_t height = 0;

};

    static void ImguiBgfxOnRenderWindow(ImGuiViewport *viewport, void *);

static std::map<SDL_Window*, ImGuiViewport*> windowToViewports;
static void ImguiBgfxOnSetWindowSize(ImGuiViewport *viewport, ImVec2 size);

static bool fromHere;

static int Filter(void* userData, SDL_Event* event) {

    if (event->type != SDL_EVENT_WINDOW_RESIZED) {
        return 1;
    }

    if (fromHere) {
        return 1;
    }

    fromHere = true;

    ImGui_ImplSDL3_ProcessEvent(event);

    SDL_Window* window = SDL_GetWindowFromID(event->window.windowID);

    auto it = windowToViewports.find(window);
    if (it != windowToViewports.end()) {
        auto *viewport = it->second;
        auto data = (imgui_viewport_data *) viewport->RendererUserData;

        int width;
        int height;
        SDL_GetWindowSizeInPixels(window, &width, &height);
        data->width = width;
        data->height = height;

        viewport->Size = {(float) width, (float) height};
        ImguiBgfxOnSetWindowSize(viewport,{0,0});
        ImguiBgfxOnRenderWindow(viewport, 0);
    }

    renderFunction();

    fromHere = false;
    return 1;
}

static void ImguiBgfxOnCreateWindow(ImGuiViewport *viewport) {
    auto data = new imgui_viewport_data();
    viewport->RendererUserData = data;
    // Setup view id and size
    data->viewId = allocate_view_id();
    data->width = bx::max<uint16_t>((uint16_t) viewport->Size.x, 1);
    data->height = bx::max<uint16_t>((uint16_t) viewport->Size.y, 1);
    // Create frame buffer
    data->frameBufferHandle =
            bgfx::createFrameBuffer((void *) native_window_handle(viewport, (SDL_Window *) viewport->PlatformHandle),
                                    data->width * viewport->DrawData->FramebufferScale.x,
                                    data->height * viewport->DrawData->FramebufferScale.y);
    // Set frame buffer
    bgfx::setViewFrameBuffer(data->viewId, data->frameBufferHandle);
    bgfx::setViewClear(data->viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);

    windowToViewports[(SDL_Window *) viewport->PlatformHandle] = viewport;
}

static void ImguiBgfxOnDestroyWindow(ImGuiViewport *viewport)
{
  if (auto data = (imgui_viewport_data *)viewport->RendererUserData; data)
  {
      for (auto it = windowToViewports.begin(); it != windowToViewports.end();) {
          if (it->second == viewport) {
              windowToViewports.erase(it);
              break;
          }
          ++it;
      }

    viewport->RendererUserData = nullptr;
    free_view_id(data->viewId);
    bgfx::destroy(data->frameBufferHandle);
    data->frameBufferHandle.idx = bgfx::kInvalidHandle;
    delete data;



  }
}

static void ImguiBgfxOnSetWindowSize(ImGuiViewport *viewport, ImVec2 size)
{
  ImguiBgfxOnDestroyWindow(viewport);
  ImguiBgfxOnCreateWindow(viewport);
}

static void ImguiBgfxOnRenderWindow(ImGuiViewport *viewport, void *)
{
  if (auto data = (imgui_viewport_data *)viewport->RendererUserData; data)
  {
    ImGui_Impl_sdl_bgfx_Render(data->viewId, viewport->DrawData,
                               !(viewport->Flags & ImGuiViewportFlags_NoRendererClear) ? 0x000000ff :
                                                                                         0);
  }
}

void ImGui_Impl_sdl_bgfx_Resize(SDL_Window *window)
{
  int drawable_width{0};
  int drawable_height{0};
  SDL_GetWindowSizeInPixels(window, &drawable_width, &drawable_height);
  ImGuiIO &io = ImGui::GetIO();
  io.DisplaySize = ImVec2((float)drawable_width, (float)drawable_height);
  bgfx::reset(drawable_width, drawable_height, BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4);

  if (drawable_width!=lastWindowWidth ||
      drawable_height!=lastWindowHeight) {

      auto viewPorts = windowToViewports;
      for(auto o : viewPorts) {
          ImguiBgfxOnSetWindowSize(o.second, {0,0});
      }
  }
  
    lastWindowWidth = drawable_width;
    lastWindowHeight = drawable_height;
}

void ImGui_Impl_sdl_bgfx_Render(const bgfx::ViewId view_id, ImDrawData *draw_data, uint32_t clearColor)
{
  if (ImGuiIO &io = ImGui::GetIO(); io.DisplaySize.x <= 0 || io.DisplaySize.y <= 0)
  {
    return;
  }

  if (clearColor)
  {
    bgfx::setViewClear(view_id, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, clearColor, 1.0f, 0);
  }
  bgfx::touch(view_id);
  bgfx::setViewName(view_id, "ImGui");
  bgfx::setViewMode(view_id, bgfx::ViewMode::Sequential);

  // (0,0) unless using multi-viewports
  const auto clip_position = draw_data->DisplayPos;
  const auto clip_size = draw_data->DisplaySize;
  // (1,1) unless using retina display which are often (2,2)
  const ImVec2 clip_scale = draw_data->FramebufferScale;
  const auto framebuffer_size = clip_size * clip_scale;

  const bgfx::Caps *caps = bgfx::getCaps();
  {
    const auto L = clip_position.x;
    const auto R = L + clip_size.x;
    const auto T = clip_position.y;
    const auto B = T + clip_size.y;
    float ortho[16];
    bx::mtxOrtho(ortho, L, R, B, T, 0.0f, 1000.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(view_id, nullptr, ortho);
    bgfx::setViewRect(view_id, 0, 0, static_cast<uint16_t>(clip_size.x * clip_scale.x),
                      static_cast<uint16_t>(clip_size.y * clip_scale.y));
  }

  // draw_data->ScaleClipRects(clipScale);
  // Render command lists
  for (int32_t ii = 0, num = draw_data->CmdListsCount; ii < num; ++ii)
  {
    bgfx::TransientVertexBuffer tvb;
    bgfx::TransientIndexBuffer tib;

    const ImDrawList *drawList = draw_data->CmdLists[ii];
    uint32_t numVertices = (uint32_t)drawList->VtxBuffer.size();
    uint32_t numIndices = (uint32_t)drawList->IdxBuffer.size();

    if (!checkAvailTransientBuffers(numVertices, vertex_layout, numIndices))
    {
      // not enough space in transient buffer just quit drawing the rest...
      break;
    }

    bgfx::allocTransientVertexBuffer(&tvb, numVertices, vertex_layout);
    bgfx::allocTransientIndexBuffer(&tib, numIndices, sizeof(ImDrawIdx) == 4);

    ImDrawVert *verts = (ImDrawVert *)tvb.data;
    bx::memCopy(verts, drawList->VtxBuffer.begin(), numVertices * sizeof(ImDrawVert));

    ImDrawIdx *indices = (ImDrawIdx *)tib.data;
    bx::memCopy(indices, drawList->IdxBuffer.begin(), numIndices * sizeof(ImDrawIdx));

    bgfx::Encoder *encoder = bgfx::begin();

    uint32_t offset = 0;
    for (const ImDrawCmd *cmd = drawList->CmdBuffer.begin(), *cmdEnd = drawList->CmdBuffer.end();
         cmd != cmdEnd; ++cmd)
    {
      if (cmd->UserCallback)
      {
        cmd->UserCallback(drawList, cmd);
      }
      else if (0 != cmd->ElemCount)
      {
        uint64_t state = 0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA;
        uint32_t sampler_state = 0;

        bgfx::TextureHandle texture_handle = font_texture;
        bgfx::ProgramHandle program = shader_handle;

        auto alphaBlend = true;
        if (cmd->TextureId != nullptr)
        {
          auto textureInfo = (uintptr_t)cmd->TextureId;
          if (textureInfo & (uint32_t)BgfxTextureFlags::Opaque)
          {
            alphaBlend = false;
          }
          if (textureInfo & (uint32_t)BgfxTextureFlags::PointSampler)
          {
            sampler_state = BGFX_SAMPLER_POINT;
          }
          textureInfo &= ~(uint32_t)BgfxTextureFlags::All;
          texture_handle = {(uint16_t)textureInfo};
        }
        if (alphaBlend)
        {
          state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
        }

        // Project scissor/clipping rectangles into framebuffer space
        ImVec4 clipRect;
        clipRect.x = (cmd->ClipRect.x - clip_position.x) * clip_scale.x;
        clipRect.y = (cmd->ClipRect.y - clip_position.y) * clip_scale.y;
        clipRect.z = (cmd->ClipRect.z - clip_position.x) * clip_scale.x;
        clipRect.w = (cmd->ClipRect.w - clip_position.y) * clip_scale.y;

        if (clipRect.x < framebuffer_size.x && clipRect.y < framebuffer_size.y && clipRect.z >= 0.0f &&
            clipRect.w >= 0.0f)
        {
          const uint16_t x = (uint16_t)bx::max<float>(cmd->ClipRect.x - clip_position.x, 0.0f);
          const uint16_t y(bx::max<float>(cmd->ClipRect.y - clip_position.y, 0.0f));
          const uint16_t width(bx::min<float>(cmd->ClipRect.z - clip_position.x - x, 65535.0f));
          const uint16_t height(bx::min<float>(cmd->ClipRect.w - clip_position.y - y, 65535.0f));
          encoder->setScissor(x * clip_scale.x, y * clip_scale.x, width * clip_scale.x,
                              height * clip_scale.x);

          encoder->setState(state);
          encoder->setTexture(0, uniform_texture, texture_handle, sampler_state);
          encoder->setVertexBuffer(0, &tvb, 0, numVertices);
          encoder->setIndexBuffer(&tib, offset, cmd->ElemCount);
          encoder->submit(view_id, program);
        }
      }

      offset += cmd->ElemCount;
    }

    bgfx::end(encoder);
  }
}

void ImGui_Implbgfx_CreateDeviceObjects()
{
  const auto type = bgfx::getRendererType();
  shader_handle =
    bgfx::createProgram(bgfx::createEmbeddedShader(s_embeddedShaders, type, "vs_ocornut_imgui"),
                        bgfx::createEmbeddedShader(s_embeddedShaders, type, "fs_ocornut_imgui"), true);

  vertex_layout.begin()
    .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
    .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
    .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
    .end();

  uniform_texture = bgfx::createUniform("s_tex", bgfx::UniformType::Sampler);

  // Build texture atlas
  ImGuiIO &io = ImGui::GetIO();
  unsigned char *pixels;
  int width, height;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
  // Upload texture to graphics system
  font_texture =
    bgfx::createTexture2D((uint16_t)width, (uint16_t)height, false, 1, bgfx::TextureFormat::BGRA8, 0,
                          bgfx::copy(pixels, width * height * 4));
  is_init = bgfx::isValid(font_texture);
  // Store our identifier
  io.Fonts->TexID = (void *)(intptr_t)font_texture.idx;
}

void ImGui_Implbgfx_InvalidateDeviceObjects()
{
  if (bgfx::isValid(shader_handle))
  {
    bgfx::destroy(shader_handle);
    shader_handle.idx = bgfx::kInvalidHandle;
  }

  if (isValid(font_texture))
  {
    bgfx::destroy(font_texture);
    ImGui::GetIO().Fonts->TexID = 0;
    font_texture.idx = bgfx::kInvalidHandle;
  }
}

void ImGui_Impl_sdl_bgfx_Init(int view)
{
  main_view_id = (uint8_t)(view & 0xff);
  ImGuiPlatformIO &platform_io = ImGui::GetPlatformIO();
  platform_io.Renderer_CreateWindow = ImguiBgfxOnCreateWindow;
  platform_io.Renderer_DestroyWindow = ImguiBgfxOnDestroyWindow;
  platform_io.Renderer_SetWindowSize = ImguiBgfxOnSetWindowSize;
  platform_io.Renderer_RenderWindow = ImguiBgfxOnRenderWindow;

  SDL_SetEventFilter(&Filter, nullptr);
}

void ImGui_Impl_sdl_bgfx_Shutdown()
{
  ImGui_Implbgfx_InvalidateDeviceObjects();
}

void ImGui_Impl_sdl_bgfx_NewFrame()
{
  if (!is_init)
  {
    ImGui_Implbgfx_CreateDeviceObjects();
  }
}

void SetRenderFunction(std::function<void()> renderFunction_) {
    renderFunction = renderFunction_;
}
