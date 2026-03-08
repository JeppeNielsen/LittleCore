#pragma once

#include <sokol_gfx.h>

bool lc_sg_setup();
void lc_sg_shutdown();
void lc_sg_commit_frame();
bool lc_sg_begin_window_pass();
void lc_sg_end_pass();

inline bool lc_sg_valid(sg_image h) { return h.id != SG_INVALID_ID; }
inline bool lc_sg_valid(sg_shader h) { return h.id != SG_INVALID_ID; }
inline bool lc_sg_valid(sg_pipeline h) { return h.id != SG_INVALID_ID; }
inline bool lc_sg_valid(sg_buffer h) { return h.id != SG_INVALID_ID; }
inline bool lc_sg_valid(sg_attachments h) { return h.id != SG_INVALID_ID; }

inline void lc_sg_destroy(sg_image& h) {
    if (lc_sg_valid(h)) {
        sg_destroy_image(h);
        h = {SG_INVALID_ID};
    }
}

inline void lc_sg_destroy(sg_shader& h) {
    if (lc_sg_valid(h)) {
        sg_destroy_shader(h);
        h = {SG_INVALID_ID};
    }
}

inline void lc_sg_destroy(sg_pipeline& h) {
    if (lc_sg_valid(h)) {
        sg_destroy_pipeline(h);
        h = {SG_INVALID_ID};
    }
}

inline void lc_sg_destroy(sg_buffer& h) {
    if (lc_sg_valid(h)) {
        sg_destroy_buffer(h);
        h = {SG_INVALID_ID};
    }
}

inline void lc_sg_destroy(sg_attachments& h) {
    if (lc_sg_valid(h)) {
        sg_destroy_attachments(h);
        h = {SG_INVALID_ID};
    }
}
