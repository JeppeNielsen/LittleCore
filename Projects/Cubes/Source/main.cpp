
#include <SDL3/SDL.h>
#include <iostream>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>
#include <fstream>
#include <vector>
#include "IState.hpp"
#include "Engine.hpp"
#include "FileHelper.hpp"

using namespace LittleCore;

struct PosColorVertex
{
    float m_x;
    float m_y;
    float m_z;
    uint32_t m_abgr;

    static void init()
    {
        ms_layout
                .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
                .end();
    };

    static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout PosColorVertex::ms_layout;

static PosColorVertex s_cubeVertices[] =
        {
                {-1.0f,  1.0f,  1.0f, 0xff000000 },
                { 1.0f,  1.0f,  1.0f, 0xff0000ff },
                {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
                { 1.0f, -1.0f,  1.0f, 0xff00ffff },
                {-1.0f,  1.0f, -1.0f, 0xffff0000 },
                { 1.0f,  1.0f, -1.0f, 0xffff00ff },
                {-1.0f, -1.0f, -1.0f, 0xffffff00 },
                { 1.0f, -1.0f, -1.0f, 0xffffffff },
        };

static const uint16_t s_cubeTriList[] =
        {
                0, 1, 2, // 0
                1, 3, 2,
                4, 6, 5, // 2
                5, 6, 7,
                0, 2, 4, // 4
                4, 2, 6,
                1, 5, 3, // 6
                5, 7, 3,
                0, 4, 1, // 8
                4, 5, 1,
                2, 3, 6, // 10
                6, 3, 7,
        };

struct Mat {
    float v[16];
    
    
    operator float*() { return v; }
};



struct Cubes : IState {

    bgfx::ProgramHandle program;
    bgfx::VertexBufferHandle vbh;
    bgfx::IndexBufferHandle ibh;
    bgfx::ShaderHandle vertexShader;
    bgfx::ShaderHandle fragmentShader;
    std::vector<uint8_t> vertexShaderSource;
    std::vector<uint8_t> fragShaderSource;


    float time = 0;
    float scale = 1.0f;

    void Initialize() override {
        PosColorVertex::init();

        // Create static vertex buffer.
        vbh = bgfx::createVertexBuffer(
                // Static data can be passed with bgfx::makeRef
                bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices) )
                , PosColorVertex::ms_layout
        );

        // Create static index buffer for triangle list rendering.
        ibh = bgfx::createIndexBuffer(
                // Static data can be passed with bgfx::makeRef
                bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList) )
        );


        vertexShaderSource = FileHelper::ReadData("/Users/jeppe/Jeppes/LittleCore/Projects/Cubes/Shaders/Source/vs_cubes.bin");
        fragShaderSource = FileHelper::ReadData("/Users/jeppe/Jeppes/LittleCore/Projects/Cubes/Shaders/Source/fs_cubes.bin");


        //std::ifstream infile("vs_cubes", std::ios_base::binary);
        //std::vector<uint8_t> buffer( std::istreambuf_iterator<uint8_t>(infile),
        //                         std::istreambuf_iterator<uint8_t>() );

        const bgfx::Memory* vsSource = bgfx::makeRef(&vertexShaderSource[0], vertexShaderSource.size());
        vertexShader = bgfx::createShader(vsSource);

        const bgfx::Memory* fsSource = bgfx::makeRef(&fragShaderSource[0], fragShaderSource.size());
        fragmentShader = bgfx::createShader(fsSource);

        program = bgfx::createProgram(vertexShader, fragmentShader, true);
    }

    void Update(float dt) override {
        time+=dt;
    }

    void Render() override {
        const bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
        const bx::Vec3 eye = { 0.0f, 0.0f, -10.0f };

        // Set view and projection matrix for view 0.
        {
            float view[16];
            bx::mtxLookAt(view, eye, at);

            float proj[16];
            bx::mtxProj(proj, 60.0f, 1.0f, 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
            bgfx::setViewTransform(0, view, proj);
        }

        // This dummy draw call is here to make sure that view 0 is cleared
        // if no other draw calls are submitted to view 0.
        bgfx::touch(0);

        uint64_t state = 0
                         | BGFX_STATE_WRITE_R
                         | BGFX_STATE_WRITE_G
                         | BGFX_STATE_WRITE_B
                         | BGFX_STATE_WRITE_A
                         | BGFX_STATE_WRITE_Z
                         | BGFX_STATE_DEPTH_TEST_LESS
                         | BGFX_STATE_CULL_CW
                         | BGFX_STATE_MSAA
                         | BGFX_STATE_BLEND_ALPHA
        ;

        // Submit 11x11 cubes.
        for (uint32_t yy = 0; yy < 1; ++yy)
        {
            for (uint32_t xx = 0; xx < 1; ++xx)
            {
                Mat mat;

                float mtx[16];
                bx::mtxRotateXY(mat, time + xx*0.1f, time + yy*0.1f);
                mat.v[12] = float(xx)*0.6f;
                mat.v[13] = float(yy)*0.6f;
                mat.v[14] = 0.0f;

                float scMatrix[16];
                bx::mtxScale(scMatrix, scale);

                float res[16];
                bx::mtxMul(res, scMatrix, mat);

                // Set model matrix for rendering.
                bgfx::setTransform(res);

                // Set vertex and index buffer.
                bgfx::setVertexBuffer(0, vbh);
                bgfx::setIndexBuffer(ibh);

                // Set render states.
                bgfx::setState(state);

                // Submit primitive for rendering to view 0.
                bgfx::submit(0, program);
            }
        }
    }

};

int main() {
    Engine engine({"Cubes"});
    engine.Start<Cubes>();
    return 0;
}
