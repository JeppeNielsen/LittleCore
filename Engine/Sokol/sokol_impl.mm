#define SOKOL_NO_ENTRY
#define SOKOL_IMPL
#include <sokol_app.h>
#include <sokol_gfx.h>
#include <sokol_glue.h>
#include <cstdio>

static sg_image g_lc_dummy_color = {SG_INVALID_ID};
static sg_view g_lc_dummy_color_view = {SG_INVALID_ID};
static bool g_lc_has_window_pass = false;
static uint32_t g_lc_frame_index = 0;

static void lc_sg_log(const char* tag,
                      uint32_t log_level,
                      uint32_t log_item_id,
                      const char* message_or_null,
                      uint32_t line_nr,
                      const char* filename_or_null,
                      void*) {
    const char* level = "info";
    switch (log_level) {
        case 0: level = "panic"; break;
        case 1: level = "error"; break;
        case 2: level = "warn"; break;
        case 3: level = "info"; break;
        default: break;
    }
    std::fprintf(stderr,
                 "[%s][%s] id=%u %s (%s:%u)\n",
                 tag ? tag : "sg",
                 level,
                 log_item_id,
                 message_or_null ? message_or_null : "<no message>",
                 filename_or_null ? filename_or_null : "<unknown>",
                 line_nr);
}

bool lc_sg_setup() {
    sg_desc desc{};
    desc.environment = sglue_environment();
    desc.logger.func = lc_sg_log;
    desc.buffer_pool_size = 512;
    desc.image_pool_size = 512;
    desc.pipeline_pool_size = 256;
    desc.sampler_pool_size = 128;
    desc.view_pool_size = 512;
    sg_setup(desc);
    if (!sg_isvalid()) {
        return false;
    }

    sg_image_desc color_desc{};
    color_desc.usage.color_attachment = true;
    color_desc.usage.immutable = false;
    color_desc.width = 1;
    color_desc.height = 1;
    color_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    g_lc_dummy_color = sg_make_image(color_desc);
    if (g_lc_dummy_color.id == SG_INVALID_ID) {
        sg_shutdown();
        return false;
    }

    sg_view_desc view_desc{};
    view_desc.color_attachment.image = g_lc_dummy_color;
    g_lc_dummy_color_view = sg_make_view(view_desc);
    if (g_lc_dummy_color_view.id == SG_INVALID_ID) {
        sg_destroy_image(g_lc_dummy_color);
        g_lc_dummy_color = {SG_INVALID_ID};
        sg_shutdown();
        return false;
    }

    return true;
}

void lc_sg_shutdown() {
    g_lc_has_window_pass = false;

    if (g_lc_dummy_color_view.id != SG_INVALID_ID) {
        sg_destroy_view(g_lc_dummy_color_view);
        g_lc_dummy_color_view = {SG_INVALID_ID};
    }
    if (g_lc_dummy_color.id != SG_INVALID_ID) {
        sg_destroy_image(g_lc_dummy_color);
        g_lc_dummy_color = {SG_INVALID_ID};
    }
    sg_shutdown();
}

void lc_sg_commit_frame() {
    if (!g_lc_has_window_pass && g_lc_dummy_color_view.id != SG_INVALID_ID) {
        sg_pass_action pass_action{};
        pass_action.colors[0].load_action = SG_LOADACTION_DONTCARE;
        pass_action.colors[0].store_action = SG_STOREACTION_DONTCARE;
        sg_pass pass{};
        pass.action = pass_action;
        pass.attachments.colors[0] = g_lc_dummy_color_view;
        sg_begin_pass(pass);
        sg_end_pass();
    }
    sg_commit();
    g_lc_frame_index++;
}

uint32_t lc_sg_frame_index() {
    return g_lc_frame_index;
}

bool lc_sg_begin_window_pass() {
    if (!sg_isvalid()) {
        return false;
    }

    const int width = sapp_width();
    const int height = sapp_height();
    if (width <= 0 || height <= 0) {
        return false;
    }

    sg_pass_action pass_action{};
    pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    pass_action.colors[0].store_action = SG_STOREACTION_STORE;
    pass_action.colors[0].clear_value = {0.12f, 0.12f, 0.12f, 1.0f};
    pass_action.depth.load_action = SG_LOADACTION_CLEAR;
    pass_action.depth.store_action = SG_STOREACTION_DONTCARE;
    pass_action.depth.clear_value = 1.0f;

    sg_pass pass{};
    pass.action = pass_action;
    pass.swapchain = sglue_swapchain();
    sg_begin_pass(pass);
    g_lc_has_window_pass = true;
    return true;
}

void lc_sg_end_pass() {
    if (!g_lc_has_window_pass) {
        return;
    }
    sg_end_pass();
    g_lc_has_window_pass = false;
}
