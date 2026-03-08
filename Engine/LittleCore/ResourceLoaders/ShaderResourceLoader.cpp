//
// Created by Jeppe Nielsen on 03/03/2024.
//

#include "ShaderResourceLoader.hpp"
#include <glm/glm.hpp>
using namespace LittleCore;

sg_shader CreateDefaultMeshShader() {
    sg_shader_desc shdDesc = {};
    shdDesc.attrs[0].glsl_name = "position";
    shdDesc.attrs[1].glsl_name = "color0";

#if defined(SOKOL_METAL)
    const char* vsSrc =
        "#include <metal_stdlib>\n"
        "using namespace metal;\n"
        "struct VertexIn {\n"
        "    float3 position [[attribute(0)]];\n"
        "    float4 color0 [[attribute(1)]];\n"
        "};\n"
        "struct VertexOut {\n"
        "    float4 color [[user(locn0)]];\n"
        "    float4 position [[position]];\n"
        "};\n"
        "struct VsParams {\n"
        "    float4x4 mvp;\n"
        "};\n"
        "vertex VertexOut vs_main(VertexIn in [[stage_in]], constant VsParams& params [[buffer(0)]]) {\n"
        "    VertexOut out;\n"
        "    out.position = params.mvp * float4(in.position, 1.0);\n"
        "    out.color = in.color0;\n"
        "    return out;\n"
        "}\n";
    const char* fsSrc =
        "#include <metal_stdlib>\n"
        "using namespace metal;\n"
        "struct FragmentIn {\n"
        "    float4 color [[user(locn0)]];\n"
        "};\n"
        "fragment float4 fs_main(FragmentIn in [[stage_in]]) {\n"
        "    return in.color;\n"
        "}\n";
#elif defined(SOKOL_GLES3)
    const char* vsSrc =
        "#version 300 es\n"
        "layout(location=0) in vec3 position;\n"
        "layout(location=1) in vec4 color0;\n"
        "uniform mat4 mvp;\n"
        "out vec4 color;\n"
        "void main() {\n"
        "    gl_Position = mvp * vec4(position, 1.0);\n"
        "    color = color0;\n"
        "}\n";
    const char* fsSrc =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec4 color;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "    frag_color = color;\n"
        "}\n";
#else
    const char* vsSrc =
            "#version 330\n"
            "layout(location=0) in vec3 position;\n"
            "layout(location=1) in vec4 color0;\n"
            "uniform mat4 mvp;\n"
            "out vec4 color;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(position, 1.0);\n"
            "    color = color0;\n"
            "}\n";
    const char* fsSrc =
            "#version 330\n"
            "in vec4 color;\n"
            "out vec4 frag_color;\n"
            "void main() {\n"
            "    frag_color = color;\n"
            "}\n";
#endif

    shdDesc.vertex_func.source = vsSrc;
    shdDesc.fragment_func.source = fsSrc;
#if defined(SOKOL_METAL)
    shdDesc.vertex_func.entry = "vs_main";
    shdDesc.fragment_func.entry = "fs_main";
#else
    shdDesc.vertex_func.entry = "main";
    shdDesc.fragment_func.entry = "main";
#endif

    sg_shader_uniform_block* ub = &shdDesc.uniform_blocks[0];
    ub->stage = SG_SHADERSTAGE_VERTEX;
    ub->size = sizeof(glm::mat4);
    ub->layout = SG_UNIFORMLAYOUT_NATIVE;
    ub->msl_buffer_n = 0;
    ub->glsl_uniforms[0].glsl_name = "mvp";
    ub->glsl_uniforms[0].type = SG_UNIFORMTYPE_MAT4;
    ub->glsl_uniforms[0].array_count = 1;

    return sg_make_shader(shdDesc);
}

void ShaderResourceLoader::Load(ShaderResource& resource) {
    // Direct sokol pipeline integration is pending (sokol-shdc pipeline rewrite).
    resource.handle = CreateDefaultMeshShader();
}

void ShaderResourceLoader::Unload(ShaderResource& resource) {
    if (resource.handle.id != SG_INVALID_ID) {
        sg_destroy_shader(resource.handle);
        resource.handle = {SG_INVALID_ID};
    }
}

bool ShaderResourceLoader::IsLoaded() {
    return true;
}

void ShaderResourceLoader::Reload(ShaderResource& resource) {
    Unload(resource);
    Load(resource);
}
