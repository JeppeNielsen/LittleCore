//
// Created by Jeppe Nielsen on 03/03/2024.
//

#include "ShaderResourceLoader.hpp"
#include <glm/glm.hpp>
using namespace LittleCore;

namespace {
    sg_shader CreateDefaultMeshShaderVariant(const char* vsSource,
                                             const char* fsSource,
                                             const char* vsEntry,
                                             const char* fsEntry) {
        sg_shader_desc shdDesc = {};
        shdDesc.attrs[0].glsl_name = "position";
        shdDesc.attrs[1].glsl_name = "color0";
        shdDesc.attrs[2].glsl_name = "uv0";
        shdDesc.vertex_func.source = vsSource;
        shdDesc.fragment_func.source = fsSource;
        shdDesc.vertex_func.entry = vsEntry;
        shdDesc.fragment_func.entry = fsEntry;

        sg_shader_uniform_block* ub = &shdDesc.uniform_blocks[0];
        ub->stage = SG_SHADERSTAGE_VERTEX;
        ub->size = sizeof(glm::mat4);
        ub->layout = SG_UNIFORMLAYOUT_NATIVE;
        ub->msl_buffer_n = 0;
        ub->glsl_uniforms[0].glsl_name = "mvp";
        ub->glsl_uniforms[0].type = SG_UNIFORMTYPE_MAT4;
        ub->glsl_uniforms[0].array_count = 1;

        sg_shader_image* image = &shdDesc.images[0];
        image->stage = SG_SHADERSTAGE_FRAGMENT;
        image->image_type = SG_IMAGETYPE_2D;
        image->sample_type = SG_IMAGESAMPLETYPE_FLOAT;
        image->msl_texture_n = 0;

        sg_shader_sampler* sampler = &shdDesc.samplers[0];
        sampler->stage = SG_SHADERSTAGE_FRAGMENT;
        sampler->sampler_type = SG_SAMPLERTYPE_FILTERING;
        sampler->msl_sampler_n = 0;

        sg_shader_image_sampler_pair* imageSamplerPair = &shdDesc.image_sampler_pairs[0];
        imageSamplerPair->stage = SG_SHADERSTAGE_FRAGMENT;
        imageSamplerPair->image_slot = 0;
        imageSamplerPair->sampler_slot = 0;
        imageSamplerPair->glsl_name = "colorTexture";

        return sg_make_shader(shdDesc);
    }

    sg_shader CreateDefaultMeshShaderMetal() {
        const char* vsSrcA =
            "#include <metal_stdlib>\n"
            "using namespace metal;\n"
            "struct VertexIn {\n"
            "    float3 position [[attribute(0)]];\n"
            "    float4 color0 [[attribute(1)]];\n"
            "    float2 uv0 [[attribute(2)]];\n"
            "};\n"
            "struct VsParams {\n"
            "    float4x4 mvp;\n"
            "};\n"
            "struct StageData {\n"
            "    float4 position [[position]];\n"
            "    float4 color;\n"
            "    float2 uv;\n"
            "};\n"
            "vertex StageData vs_main(VertexIn in [[stage_in]], constant VsParams& params [[buffer(0)]]) {\n"
            "    StageData out;\n"
            "    out.position = params.mvp * float4(in.position, 1.0);\n"
            "    out.color = in.color0;\n"
            "    out.uv = in.uv0;\n"
            "    return out;\n"
            "}\n";
        const char* fsSrcA =
            "#include <metal_stdlib>\n"
            "using namespace metal;\n"
            "struct StageData {\n"
            "    float4 position [[position]];\n"
            "    float4 color;\n"
            "    float2 uv;\n"
            "};\n"
            "fragment float4 fs_main(StageData in [[stage_in]], texture2d<float> colorTexture [[texture(0)]], sampler colorSampler [[sampler(0)]]) {\n"
            "    float4 col = in.color * colorTexture.sample(colorSampler, in.uv);\n"
            "    return col;\n" //\n"
            "}\n";

        const char* vsSrcB =
            "#include <metal_stdlib>\n"
            "using namespace metal;\n"
            "struct VertexIn {\n"
            "    float3 position [[attribute(0)]];\n"
            "    float4 color0 [[attribute(1)]];\n"
            "    float2 uv0 [[attribute(2)]];\n"
            "};\n"
            "struct VsParams {\n"
            "    float4x4 mvp;\n"
            "};\n"
            "struct VertexOut {\n"
            "    float4 color [[user(locn0)]];\n"
            "    float2 uv [[user(locn1)]];\n"
            "    float4 position [[position]];\n"
            "};\n"
            "vertex VertexOut vs_main(VertexIn in [[stage_in]], constant VsParams& params [[buffer(0)]]) {\n"
            "    VertexOut out;\n"
            "    out.position = params.mvp * float4(in.position, 1.0);\n"
            "    out.color = in.color0;\n"
            "    out.uv = in.uv0;\n"
            "    return out;\n"
            "}\n";
        const char* fsSrcB =
            "#include <metal_stdlib>\n"
            "using namespace metal;\n"
            "struct FragmentIn {\n"
            "    float4 color [[user(locn0)]];\n"
            "    float2 uv [[user(locn1)]];\n"
            "};\n"
            "fragment float4 fs_main(FragmentIn in [[stage_in]], texture2d<float> colorTexture [[texture(0)]], sampler colorSampler [[sampler(0)]]) {\n"
            "    return float4(0,1,0,1);\n"//in.color * colorTexture.sample(colorSampler, in.uv);\n"
            "}\n";

        struct ShaderVariant {
            const char* vertexSource;
            const char* fragmentSource;
        };
        const ShaderVariant variants[] = {
            {vsSrcA, fsSrcA},
            {vsSrcB, fsSrcB}
        };

        for (const auto& variant : variants) {
            const sg_shader shader = CreateDefaultMeshShaderVariant(variant.vertexSource, variant.fragmentSource, "vs_main", "fs_main");
            const sg_resource_state state = sg_query_shader_state(shader);
            if (state == SG_RESOURCESTATE_VALID || state == SG_RESOURCESTATE_ALLOC) {
                return shader;
            }
            if (shader.id != SG_INVALID_ID) {
                sg_destroy_shader(shader);
            }
        }
        return {SG_INVALID_ID};
    }

    sg_shader CreateDefaultMeshShaderGles3() {
        const char* vsSrc =
            "#version 300 es\n"
            "layout(location=0) in vec3 position;\n"
            "layout(location=1) in vec4 color0;\n"
            "layout(location=2) in vec2 uv0;\n"
            "uniform mat4 mvp;\n"
            "out vec4 color;\n"
            "out vec2 uv;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(position, 1.0);\n"
            "    color = color0;\n"
            "    uv = uv0;\n"
            "}\n";
        const char* fsSrc =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec4 color;\n"
            "in vec2 uv;\n"
            "uniform sampler2D colorTexture;\n"
            "out vec4 frag_color;\n"
            "void main() {\n"
            "    frag_color = color * texture(colorTexture, uv);\n"
            "}\n";
        return CreateDefaultMeshShaderVariant(vsSrc, fsSrc, "main", "main");
    }

    sg_shader CreateDefaultMeshShaderGlCore() {
        const char* vsSrc =
                "#version 330\n"
                "layout(location=0) in vec3 position;\n"
                "layout(location=1) in vec4 color0;\n"
                "layout(location=2) in vec2 uv0;\n"
                "uniform mat4 mvp;\n"
                "out vec4 color;\n"
                "out vec2 uv;\n"
                "void main() {\n"
                "    gl_Position = mvp * vec4(position, 1.0);\n"
                "    color = color0;\n"
                "    uv = uv0;\n"
                "}\n";
        const char* fsSrc =
                "#version 330\n"
                "in vec4 color;\n"
                "in vec2 uv;\n"
                "uniform sampler2D colorTexture;\n"
                "out vec4 frag_color;\n"
                "void main() {\n"
                "    frag_color = color * texture(colorTexture, uv);\n"
                "}\n";
        return CreateDefaultMeshShaderVariant(vsSrc, fsSrc, "main", "main");
    }
}

sg_shader CreateDefaultMeshShader() {
    switch (sg_query_backend()) {
        case SG_BACKEND_METAL_IOS:
        case SG_BACKEND_METAL_MACOS:
        case SG_BACKEND_METAL_SIMULATOR:
            return CreateDefaultMeshShaderMetal();
        case SG_BACKEND_GLES3:
            return CreateDefaultMeshShaderGles3();
        case SG_BACKEND_GLCORE:
        case SG_BACKEND_DUMMY:
            return CreateDefaultMeshShaderGlCore();
        default:
            return {SG_INVALID_ID};
    }
}

void ShaderResourceLoader::Load(ShaderResource& resource) {
    // Direct sokol pipeline integration is pending (sokol-shdc pipeline rewrite).
    resource.handle = CreateDefaultMeshShader();
    const sg_resource_state shaderState = sg_query_shader_state(resource.handle);
    if (shaderState == SG_RESOURCESTATE_FAILED || shaderState == SG_RESOURCESTATE_INVALID) {
        sg_destroy_shader(resource.handle);
        resource.handle = {SG_INVALID_ID};
    }
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
