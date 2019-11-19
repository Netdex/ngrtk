#include "dither.h"

namespace gfx {

    void vga_dither(unsigned char *data, vga_surface &surface, float contrast = 1.0f, float brightness = 0.0f) {
        float luma_data[vga::kVgaHeight][vga::kVgaWidth];
        // Turn the image into greyscale.
        for (int y = 0; y < vga::kVgaHeight; ++y) {
            for (int x = 0; x < vga::kVgaWidth; ++x) {
                unsigned char *pixel = data + y * (vga::kVgaWidth * 3) + x * 3;
                float r = pixel[0] / 255.f;
                float g = pixel[1] / 255.f;
                float b = pixel[2] / 255.f;
                float luma = 0.2126f * r + 0.7152f * g + 0.0722f * b;
                luma_data[y][x] = std::max(std::min(contrast * (luma - 0.5f) + 0.5f + brightness, 1.0f), 0.0f);
            }
        }

        uint8_t *surface_data = surface.data();
        for (int y = 0; y < vga::kVgaHeight; ++y) {
            for (int x = 0; x < vga::kVgaWidth; ++x) {
                float oldpixel = luma_data[y][x];
                float newpixel = roundf(oldpixel);
                luma_data[y][x] = newpixel;
                surface_data[y * vga::kVgaWidth + x] = newpixel > 0.5f ? 1 : 0;
                float quant_error = oldpixel - newpixel;
                if (y < vga::kVgaHeight - 1) {
                    luma_data[y + 1][x] = luma_data[y + 1][x] + quant_error * 5 / 16;
                    if (x > 0) {
                        luma_data[y + 1][x - 1] = luma_data[y + 1][x - 1] + quant_error * 3 / 16;
                    }
                }
                if (x < vga::kVgaWidth - 1) {
                    luma_data[y][x + 1] = luma_data[y][x + 1] + quant_error * 7 / 16;
                    if (y < vga::kVgaHeight - 1) {
                        luma_data[y + 1][x + 1] = luma_data[y + 1][x + 1] + quant_error * 1 / 16;
                    }
                }
            }
        }


    }

}
