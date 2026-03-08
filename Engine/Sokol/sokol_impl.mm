#define SOKOL_NO_ENTRY
#define SOKOL_IMPL
#include <sokol_app.h>
#include <sokol_gfx.h>
#include <sokol_glue.h>

static sg_image g_lc_dummy_color = {SG_INVALID_ID};
static sg_attachments g_lc_dummy_attachments = {SG_INVALID_ID};
static bool g_lc_has_window_pass = false;

bool lc_sg_setup() {
    sg_desc desc{};
    desc.environment = sglue_environment();
    sg_setup(desc);
    if (!sg_isvalid()) {
        return false;
    }

    sg_image_desc color_desc{};
    color_desc.render_target = true;
    color_desc.width = 1;
    color_desc.height = 1;
    color_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    g_lc_dummy_color = sg_make_image(color_desc);
    if (g_lc_dummy_color.id == SG_INVALID_ID) {
        sg_shutdown();
        return false;
    }

    sg_attachments_desc attachments_desc{};
    attachments_desc.colors[0].image = g_lc_dummy_color;
    g_lc_dummy_attachments = sg_make_attachments(attachments_desc);
    if (g_lc_dummy_attachments.id == SG_INVALID_ID) {
        sg_destroy_image(g_lc_dummy_color);
        g_lc_dummy_color = {SG_INVALID_ID};
        sg_shutdown();
        return false;
    }

    return true;
}

void lc_sg_shutdown() {
    g_lc_has_window_pass = false;

    if (g_lc_dummy_attachments.id != SG_INVALID_ID) {
        sg_destroy_attachments(g_lc_dummy_attachments);
        g_lc_dummy_attachments = {SG_INVALID_ID};
    }
    if (g_lc_dummy_color.id != SG_INVALID_ID) {
        sg_destroy_image(g_lc_dummy_color);
        g_lc_dummy_color = {SG_INVALID_ID};
    }
    sg_shutdown();
}

void lc_sg_commit_frame() {
    if (!g_lc_has_window_pass && g_lc_dummy_attachments.id != SG_INVALID_ID) {
        sg_pass_action pass_action{};
        pass_action.colors[0].load_action = SG_LOADACTION_DONTCARE;
        pass_action.colors[0].store_action = SG_STOREACTION_DONTCARE;
        sg_pass pass{};
        pass.action = pass_action;
        pass.attachments = g_lc_dummy_attachments;
        sg_begin_pass(pass);
        sg_end_pass();
    }
    sg_commit();
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
