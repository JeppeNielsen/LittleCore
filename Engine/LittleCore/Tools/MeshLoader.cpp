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

// Append one material part as indexed triangles (dedup within the part)
bool append_mesh_part_16bit(ufbx_mesh *mesh,
                                   const ufbx_mesh_part &part,
                                   LittleCore::Mesh &dst,
                                   bool flip_v /*=true*/)
{
    // Scratch for triangulation of the largest face
    std::vector<uint32_t> tri_corner_indices(mesh->max_face_triangles * 3);

    // Build an expanded (non-indexed) vertex stream for this part
    std::vector<Vertex> tmp_vertices;
    tmp_vertices.reserve(part.num_triangles * 3);

    const bool has_uv    = mesh->vertex_uv.exists;
    const bool has_color = mesh->vertex_color.exists;

    for (uint32_t face_index : part.face_indices) {
        ufbx_face face = mesh->faces[face_index];

        // Triangulate this polygon into corner indices
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
                // FBX UVs are usually in DCC space (origin top-left). Flip V for GL-like UVs.
                v.uv = flip_v ? glm::vec2((float)t.x, 1.0f - (float)t.y)
                              : glm::vec2((float)t.x, (float)t.y);
            } else {
                v.uv = glm::vec2(0.0f, 0.0f);
            }

            if (has_color) {
                const ufbx_vec4 c = mesh->vertex_color[vi];
                v.color = pack_color_rgba_to_u32((float)c.x, (float)c.y, (float)c.z, (float)c.w);
            } else {
                v.color = 0xFFFFFFFFu; // white
            }

            tmp_vertices.push_back(v);
        }
    }

    // Deduplicate the expanded stream -> index buffer (within this material part)
    std::vector<uint32_t> idx32(part.num_triangles * 3);
    ufbx_vertex_stream streams[] = {
            { tmp_vertices.data(), tmp_vertices.size(), sizeof(Vertex) }
    };

    const size_t unique = ufbx_generate_indices(streams, 1, idx32.data(), idx32.size(), nullptr, nullptr);
    if (unique == SIZE_MAX) {
        std::fprintf(stderr, "ufbx_generate_indices() failed\n");
        return false;
    }
    tmp_vertices.resize(unique);

    // 16-bit budget check
    const size_t base = dst.vertices.size();
    if (base + tmp_vertices.size() > 65535) {
        std::fprintf(stderr, "Mesh exceeds 16-bit vertex limit: %zu + %zu > 65535\n",
                     base, tmp_vertices.size());
        return false;
    }

    // Append vertices
    dst.vertices.insert(dst.vertices.end(), tmp_vertices.begin(), tmp_vertices.end());

    // Append indices (offset by base) as uint16_t
    dst.triangles.reserve(dst.triangles.size() + idx32.size());
    for (uint32_t i : idx32) {
        uint32_t gi = (uint32_t)base + i;
        dst.triangles.push_back((uint16_t)gi);
    }

    return true;
}

// Public entry: load first mesh in the FBX into LittleCore::Mesh
bool LoadFBXIntoLittleCoreMesh(const char *path, LittleCore::Mesh &out, bool flip_v = true)
{
    out.vertices.clear();
    out.triangles.clear();

    ufbx_load_opts opts = {};
    opts.target_axes = ufbx_axes_right_handed_y_up; // consistent coordinates
    opts.target_unit_meters = 1.0f;                 // meters

    ufbx_error err = {};
    ufbx_scene *scene = ufbx_load_file(path, &opts, &err);
    if (!scene) {
        std::fprintf(stderr, "Failed to load FBX '%s': %s\n", path, err.description.data);
        return false;
    }

    // Find the first node with a mesh
    ufbx_mesh *mesh = nullptr;
    for (ufbx_node *n : scene->nodes) {
        if (n->mesh) { mesh = n->mesh; break; }
    }
    if (!mesh) {
        std::fprintf(stderr, "No mesh found in '%s'\n", path);
        ufbx_free_scene(scene);
        return false;
    }

    // Convert all material parts (at least one exists)
    bool ok = true;
    for (ufbx_mesh_part &part : mesh->material_parts) {
        if (!append_mesh_part_16bit(mesh, part, out, flip_v)) { ok = false; break; }
    }

    ufbx_free_scene(scene);
    return ok;
}

void MeshLoader::LoadMesh(std::string path, Mesh& mesh) {
    LoadFBXIntoLittleCoreMesh(path.c_str(), mesh);
}
