//
// Created by Jeppe Nielsen on 18/10/2025.
//

#include "MeshLoader.hpp"
#include <vector>
#include <cstdint>
#include <cstdio>
#include <cmath>
#include "ufbx.h"

using namespace LittleCore;


//------------------------------------------------------------------------------
// Color packing
// NOTE: bgfx usually expects colors in ABGR when feeding a raw uint32_t field.
// If your pipeline expects RGBA instead, flip the macro.
#ifndef PACK_ABGR_FOR_BGFX
#define PACK_ABGR_FOR_BGFX 1
#endif

inline uint32_t pack_color_rgba_to_u32(float r, float g, float b, float a = 1.0f)
{
    auto to8 = [](float v)->uint32_t {
        v = v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
        return (uint32_t)std::lround(v * 255.0f);
    };
    uint32_t R = to8(r), G = to8(g), B = to8(b), A = to8(a);
#if PACK_ABGR_FOR_BGFX
    // ABGR byte order (least-significant byte R on little-endian)
    // Matches bgfx's common packed color convention.
    return (A<<24) | (B<<16) | (G<<8) | (R);
#else
    // RGBA byte order
    return (R<<24) | (G<<16) | (B<<8) | (A);
#endif
}

//------------------------------------------------------------------------------
// Loader

static bool append_mesh_part_16bit(ufbx_mesh *mesh,
                                   const ufbx_mesh_part &part,
                                   LittleCore::Mesh &dst,
                                   bool flip_v,
                                   bool flip_winding)          // <— NEW
{
    std::vector<uint32_t> tri_corner_indices(mesh->max_face_triangles * 3);

    std::vector<Vertex> tmp_vertices;
    tmp_vertices.reserve(part.num_triangles * 3);

    const bool has_uv    = mesh->vertex_uv.exists;
    const bool has_color = mesh->vertex_color.exists;

    for (uint32_t face_index : part.face_indices) {
        ufbx_face face = mesh->faces[face_index];
        uint32_t num_tris = ufbx_triangulate_face(tri_corner_indices.data(),
                                                  (uint32_t)tri_corner_indices.size(),
                                                  mesh, face);
        for (uint32_t i = 0; i < num_tris * 3; ++i) {
            uint32_t vi = tri_corner_indices[i];
            Vertex v{};
            const ufbx_vec3 p = mesh->vertex_position[vi];
            v.position = { (float)p.x, (float)p.y, (float)p.z };

            if (has_uv) {
                const ufbx_vec2 t = mesh->vertex_uv[vi];
                v.uv = flip_v ? glm::vec2((float)t.x, 1.0f - (float)t.y)
                              : glm::vec2((float)t.x, (float)t.y);
            } else {
                v.uv = glm::vec2(0.0f, 0.0f);
            }

            if (has_color) {
                const ufbx_vec4 c = mesh->vertex_color[vi];
                v.color = pack_color_rgba_to_u32((float)c.x, (float)c.y, (float)c.z, (float)c.w);
            } else {
                v.color = 0xFFFFFFFFu;
            }

            tmp_vertices.push_back(v);
        }
    }

    // Deduplicate
    std::vector<uint32_t> idx32(part.num_triangles * 3);
    ufbx_vertex_stream streams[] = { { tmp_vertices.data(), tmp_vertices.size(), sizeof(Vertex) } };
    size_t unique = ufbx_generate_indices(streams, 1, idx32.data(), idx32.size(), nullptr, nullptr);
    if (unique == SIZE_MAX) return false;
    tmp_vertices.resize(unique);

    // 16-bit budget
    size_t base = dst.vertices.size();
    if (base + tmp_vertices.size() > 65535) return false;

    // Append vertices
    dst.vertices.insert(dst.vertices.end(), tmp_vertices.begin(), tmp_vertices.end());

    // Append indices with optional winding flip (swap 1 and 2)
    dst.triangles.reserve(dst.triangles.size() + idx32.size());
    for (size_t t = 0; t < idx32.size(); t += 3) {
        uint32_t i0 = (uint32_t)base + idx32[t + 0];
        uint32_t i1 = (uint32_t)base + idx32[t + 1];
        uint32_t i2 = (uint32_t)base + idx32[t + 2];

        if (flip_winding) std::swap(i1, i2); // <— flip CW↔CCW

        dst.triangles.push_back((uint16_t)i0);
        dst.triangles.push_back((uint16_t)i1);
        dst.triangles.push_back((uint16_t)i2);
    }

    return true;
}

// Public entry
bool LoadFBXIntoLittleCoreMesh(const char *path,
                               LittleCore::Mesh &out,
                               bool flip_v = true,
                               bool flip_winding = false)   // <— NEW (default off)
{
    out.vertices.clear();
    out.triangles.clear();

    ufbx_load_opts opts = {};
    opts.target_axes = ufbx_axes_right_handed_y_up;
    opts.target_unit_meters = 1.0f;

    ufbx_error err = {};
    ufbx_scene *scene = ufbx_load_file(path, &opts, &err);
    if (!scene) return false;

    ufbx_mesh *mesh = nullptr;
    for (ufbx_node *n : scene->nodes) if (n->mesh) { mesh = n->mesh; break; }
    if (!mesh) { ufbx_free_scene(scene); return false; }

    bool ok = true;
    for (ufbx_mesh_part &part : mesh->material_parts) {
        if (!append_mesh_part_16bit(mesh, part, out, flip_v, flip_winding)) { ok = false; break; }
    }

    ufbx_free_scene(scene);
    return ok;
}


void MeshLoader::LoadMesh(const std::string& path, Mesh& mesh) {
    LoadFBXIntoLittleCoreMesh(path.c_str(), mesh, true, true);
}

bool MeshLoader::IsValidMesh(const std::string& path) {
    ufbx_load_opts opts = {};           // default
    // Optional: speed up “just validate” by skipping heavy parts:
    opts.ignore_geometry = true;        // don't parse mesh data
    opts.ignore_animation = true;       // don't parse anim data

    ufbx_error err = {};
    ufbx_scene* scene = ufbx_load_file(path.c_str(), &opts, &err);
    if (!scene) {
        // err.type != UFBX_ERROR_NONE, err.description.data explains why
        std::fprintf(stderr, "Not a valid/parseable FBX: %s\n",
                     err.description.data ? err.description.data : "(no message)");
        return false;
    }
    ufbx_free_scene(scene);
    return true;
}
