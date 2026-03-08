#include <sokol_app.h>
#include "SokolDirect.hpp"

struct AppState {
    bool ready = false;
};

static void init(void* userData) {
    auto& app = *static_cast<AppState*>(userData);
    app.ready = lc_sg_setup();
    if (!app.ready) {
        sapp_request_quit();
    }
}

static void frame(void* userData) {
    auto& app = *static_cast<AppState*>(userData);
    if (app.ready) {
        lc_sg_commit_frame();
    }
}

static void cleanup(void* userData) {
    auto& app = *static_cast<AppState*>(userData);
    if (app.ready) {
        lc_sg_shutdown();
        app.ready = false;
    }
}

int main() {
    AppState app{};

    sapp_desc desc{};
    desc.init_userdata_cb = init;
    desc.frame_userdata_cb = frame;
    desc.cleanup_userdata_cb = cleanup;
    desc.user_data = &app;
    desc.width = 800;
    desc.height = 600;
    desc.high_dpi = true;
    desc.window_title = "sokol";

    sapp_run(desc);
    return 0;
}
