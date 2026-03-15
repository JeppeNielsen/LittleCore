//
// Created by Codex on 13/03/2026.
//

#include "gtest/gtest.h"
#include "RenderableUniforms.hpp"

#include <cstring>
#include <string_view>

using namespace LittleCore;

namespace {
    const RenderableUniforms::UniformEntry* FindUniform(const RenderableUniforms& uniforms, const std::string_view id) {
        for (const auto& uniform : uniforms.GetUniforms()) {
            if (uniform.id == id) {
                return &uniform;
            }
        }
        return nullptr;
    }

    template <typename T>
    T ReadUniformValue(const RenderableUniforms::UniformEntry& uniform) {
        T value{};
        std::memcpy(&value, uniform.data.data(), sizeof(T));
        return value;
    }

    void ExpectVec2Eq(const vec2& lhs, const vec2& rhs) {
        EXPECT_FLOAT_EQ(lhs.x, rhs.x);
        EXPECT_FLOAT_EQ(lhs.y, rhs.y);
    }

    void ExpectVec3Eq(const vec3& lhs, const vec3& rhs) {
        EXPECT_FLOAT_EQ(lhs.x, rhs.x);
        EXPECT_FLOAT_EQ(lhs.y, rhs.y);
        EXPECT_FLOAT_EQ(lhs.z, rhs.z);
    }

    void ExpectVec4Eq(const vec4& lhs, const vec4& rhs) {
        EXPECT_FLOAT_EQ(lhs.x, rhs.x);
        EXPECT_FLOAT_EQ(lhs.y, rhs.y);
        EXPECT_FLOAT_EQ(lhs.z, rhs.z);
        EXPECT_FLOAT_EQ(lhs.w, rhs.w);
    }

    void ExpectIVec2Eq(const ivec2& lhs, const ivec2& rhs) {
        EXPECT_EQ(lhs.x, rhs.x);
        EXPECT_EQ(lhs.y, rhs.y);
    }

    void ExpectIVec3Eq(const ivec3& lhs, const ivec3& rhs) {
        EXPECT_EQ(lhs.x, rhs.x);
        EXPECT_EQ(lhs.y, rhs.y);
        EXPECT_EQ(lhs.z, rhs.z);
    }

    void ExpectIVec4Eq(const ivec4& lhs, const ivec4& rhs) {
        EXPECT_EQ(lhs.x, rhs.x);
        EXPECT_EQ(lhs.y, rhs.y);
        EXPECT_EQ(lhs.z, rhs.z);
        EXPECT_EQ(lhs.w, rhs.w);
    }

    void ExpectMat4Eq(const mat4x4& lhs, const mat4x4& rhs) {
        for (int column = 0; column < 4; ++column) {
            for (int row = 0; row < 4; ++row) {
                EXPECT_FLOAT_EQ(lhs[column][row], rhs[column][row]);
            }
        }
    }

    TEST(RenderableUniforms, StoresAllSokolUniformTypes) {
        RenderableUniforms uniforms;

        const float scalar = 1.25f;
        const vec2 float2 = {2.0f, 3.0f};
        const vec3 float3 = {4.0f, 5.0f, 6.0f};
        const vec4 float4 = {7.0f, 8.0f, 9.0f, 10.0f};
        const int integer = 11;
        const ivec2 int2 = {12, 13};
        const ivec3 int3 = {14, 15, 16};
        const ivec4 int4 = {17, 18, 19, 20};
        const mat4x4 matrix = glm::translate(glm::identity<mat4x4>(), {1.0f, 2.0f, 3.0f});

        uniforms.Set("scalar", scalar);
        uniforms.Set("float2", float2);
        uniforms.Set("float3", float3);
        uniforms.Set("float4", float4);
        uniforms.Set("integer", integer);
        uniforms.Set("int2", int2);
        uniforms.Set("int3", int3);
        uniforms.Set("int4", int4);
        uniforms.Set("matrix", matrix);

        ASSERT_EQ(9u, uniforms.GetUniforms().size());

        const auto* scalarUniform = FindUniform(uniforms, "scalar");
        ASSERT_NE(nullptr, scalarUniform);
        EXPECT_EQ(RenderableUniforms::UniformEntry::Kind::Value, scalarUniform->kind);
        EXPECT_EQ(SG_UNIFORMTYPE_FLOAT, scalarUniform->type);
        EXPECT_EQ(1, scalarUniform->arrayCount);
        EXPECT_FLOAT_EQ(scalar, ReadUniformValue<float>(*scalarUniform));

        const auto* float2Uniform = FindUniform(uniforms, "float2");
        ASSERT_NE(nullptr, float2Uniform);
        EXPECT_EQ(SG_UNIFORMTYPE_FLOAT2, float2Uniform->type);
        ExpectVec2Eq(float2, ReadUniformValue<vec2>(*float2Uniform));

        const auto* float3Uniform = FindUniform(uniforms, "float3");
        ASSERT_NE(nullptr, float3Uniform);
        EXPECT_EQ(SG_UNIFORMTYPE_FLOAT3, float3Uniform->type);
        ExpectVec3Eq(float3, ReadUniformValue<vec3>(*float3Uniform));

        const auto* float4Uniform = FindUniform(uniforms, "float4");
        ASSERT_NE(nullptr, float4Uniform);
        EXPECT_EQ(SG_UNIFORMTYPE_FLOAT4, float4Uniform->type);
        ExpectVec4Eq(float4, ReadUniformValue<vec4>(*float4Uniform));

        const auto* intUniform = FindUniform(uniforms, "integer");
        ASSERT_NE(nullptr, intUniform);
        EXPECT_EQ(SG_UNIFORMTYPE_INT, intUniform->type);
        EXPECT_EQ(integer, ReadUniformValue<int>(*intUniform));

        const auto* int2Uniform = FindUniform(uniforms, "int2");
        ASSERT_NE(nullptr, int2Uniform);
        EXPECT_EQ(SG_UNIFORMTYPE_INT2, int2Uniform->type);
        ExpectIVec2Eq(int2, ReadUniformValue<ivec2>(*int2Uniform));

        const auto* int3Uniform = FindUniform(uniforms, "int3");
        ASSERT_NE(nullptr, int3Uniform);
        EXPECT_EQ(SG_UNIFORMTYPE_INT3, int3Uniform->type);
        ExpectIVec3Eq(int3, ReadUniformValue<ivec3>(*int3Uniform));

        const auto* int4Uniform = FindUniform(uniforms, "int4");
        ASSERT_NE(nullptr, int4Uniform);
        EXPECT_EQ(SG_UNIFORMTYPE_INT4, int4Uniform->type);
        ExpectIVec4Eq(int4, ReadUniformValue<ivec4>(*int4Uniform));

        const auto* matrixUniform = FindUniform(uniforms, "matrix");
        ASSERT_NE(nullptr, matrixUniform);
        EXPECT_EQ(SG_UNIFORMTYPE_MAT4, matrixUniform->type);
        ExpectMat4Eq(matrix, ReadUniformValue<mat4x4>(*matrixUniform));
    }

    TEST(RenderableUniforms, ReplacesExistingEntriesById) {
        RenderableUniforms uniforms;

        uniforms.Set("value", vec4(1.0f, 2.0f, 3.0f, 4.0f));
        uniforms.Set("value", 42);

        ASSERT_EQ(1u, uniforms.GetUniforms().size());

        const auto* valueUniform = FindUniform(uniforms, "value");
        ASSERT_NE(nullptr, valueUniform);
        EXPECT_EQ(RenderableUniforms::UniformEntry::Kind::Value, valueUniform->kind);
        EXPECT_EQ(SG_UNIFORMTYPE_INT, valueUniform->type);
        EXPECT_EQ(42, ReadUniformValue<int>(*valueUniform));

        sg_image texture = {99};
        uniforms.Set("value", texture);

        ASSERT_EQ(1u, uniforms.GetUniforms().size());
        valueUniform = FindUniform(uniforms, "value");
        ASSERT_NE(nullptr, valueUniform);
        EXPECT_EQ(RenderableUniforms::UniformEntry::Kind::Texture, valueUniform->kind);
        EXPECT_EQ(texture.id, valueUniform->texture.id);
        EXPECT_TRUE(valueUniform->data.empty());
    }

    TEST(RenderableUniforms, StoresRawArrayPayloads) {
        RenderableUniforms uniforms;

        const vec4 values[] = {
            {1.0f, 2.0f, 3.0f, 4.0f},
            {5.0f, 6.0f, 7.0f, 8.0f},
        };

        uniforms.SetRaw("values", SG_UNIFORMTYPE_FLOAT4, values, 2);

        const auto* uniform = FindUniform(uniforms, "values");
        ASSERT_NE(nullptr, uniform);
        EXPECT_EQ(RenderableUniforms::UniformEntry::Kind::Value, uniform->kind);
        EXPECT_EQ(SG_UNIFORMTYPE_FLOAT4, uniform->type);
        EXPECT_EQ(2, uniform->arrayCount);
        ASSERT_EQ(sizeof(values), uniform->data.size());
        EXPECT_EQ(0, std::memcmp(uniform->data.data(), values, sizeof(values)));
    }
}
