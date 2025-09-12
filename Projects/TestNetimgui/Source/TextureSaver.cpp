#include "TextureSaver.hpp"
#include <cstdint>
#include <cstdio>
#include <vector>



#pragma pack(push,1)
    struct TgaHeader
    {
        uint8_t  idLength = 0;
        uint8_t  colorMapType = 0;
        uint8_t  imageType = 2;
        uint16_t colorMapFirst = 0;
        uint16_t colorMapLength = 0;
        uint8_t  colorMapDepth = 0;
        uint16_t xOrigin = 0;
        uint16_t yOrigin = 0;
        uint16_t width = 0;
        uint16_t height = 0;
        uint8_t  bpp = 32;
        uint8_t  descriptor = 0x20;
    };
#pragma pack(pop)

    static bool writeTga(const char* path,
                         const uint8_t* rgbaOrBgra,
                         uint16_t w,
                         uint16_t h,
                         bool isBGRA,
                         bool withAlpha)
    {
        FILE* f = std::fopen(path, "wb");
        if (!f) return false;

        TgaHeader hdr{};
        hdr.width  = w;
        hdr.height = h;
        hdr.bpp    = withAlpha ? 32 : 24;
        hdr.descriptor = 0x20 | (withAlpha ? 8 : 0);

        std::fwrite(&hdr, sizeof(hdr), 1, f);

        const uint32_t srcStride = w * (withAlpha ? 4u : 3u);
        std::vector<uint8_t> line;
        if (!isBGRA || !withAlpha)
            line.resize(srcStride);

        for (uint16_t y = 0; y < h; ++y)
        {
            const uint8_t* src = rgbaOrBgra + y * w * 4;
            const uint8_t* out = src;

            if (!isBGRA)
            {
                uint8_t* dst = line.data();
                if (withAlpha)
                {
                    for (uint16_t x = 0; x < w; ++x)
                    {
                        const uint8_t r = src[0], g = src[1], b = src[2], a = src[3];
                        dst[0] = b; dst[1] = g; dst[2] = r; dst[3] = a;
                        src += 4; dst += 4;
                    }
                }
                else
                {
                    for (uint16_t x = 0; x < w; ++x)
                    {
                        const uint8_t r = src[0], g = src[1], b = src[2];
                        dst[0] = b; dst[1] = g; dst[2] = r;
                        src += 4; dst += 3;
                    }
                }
                out = line.data();
            }
            else if (!withAlpha)
            {
                uint8_t* dst = line.data();
                for (uint16_t x = 0; x < w; ++x)
                {
                    dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2];
                    src += 4; dst += 3;
                }
                out = line.data();
            }

            std::fwrite(out, 1, withAlpha ? w * 4u : w * 3u, f);
        }

        std::fclose(f);
        return true;
    }

// Save a texture handle to a .tga file.
// Supports RGBA8 and BGRA8 directly. Other formats are blitted to BGRA8.
// Only works for 2D, single-layer, non-cubemap textures.
bool TextureSaver::SaveTGA(bgfx::TextureHandle handle,
                           const char* path,
                           uint16_t w, uint16_t h,
                           bgfx::TextureFormat::Enum format,
                           bool withAlpha)
{
    if (!bgfx::isValid(handle))
        return false;

    const bool isRGBA8 = (format == bgfx::TextureFormat::RGBA8);
    const bool isBGRA8 = (format == bgfx::TextureFormat::BGRA8);

    bgfx::TextureHandle readTarget = handle;
   bool dataIsBGRA = isBGRA8;

    // If not RGBA8/BGRA8, create staging texture and blit.
    bgfx::TextureHandle staging = BGFX_INVALID_HANDLE;
    if (!isRGBA8 && !isBGRA8)
    {
        const uint64_t flags = BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK;
        staging = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::BGRA8, flags);
        if (!bgfx::isValid(staging)) return false;

        bgfx::blit(0, staging, 0, 0, handle, 0, 0, w, h);
        bgfx::frame(); // Ensure blit completes

        readTarget = staging;
        dataIsBGRA = true;
    }

    // Read pixels
    const uint32_t byteCount = w * h * 4u;
    std::vector<uint8_t> pixels(byteCount);
    const uint32_t readBytes = bgfx::readTexture(readTarget, pixels.data(), 0 /*mip*/);
    if (readBytes != byteCount)
    {
        if (bgfx::isValid(staging)) bgfx::destroy(staging);
        return false;
    }

    const bool ok = writeTga(path, pixels.data(), w, h, dataIsBGRA, withAlpha);

    if (bgfx::isValid(staging))
        bgfx::destroy(staging);

    return ok;
}
