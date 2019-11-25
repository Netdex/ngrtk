#include "dither.h"
#include "vga_palette.h"

namespace gfx {
    namespace {
        inline float adjust(float val, float contrast, float brightness) {
            return std::max(std::min(contrast * (val - 0.5f) + 0.5f + brightness, 1.0f), 0.0f);
        }
    }

    void vga_dither(unsigned char *data, vga_surface &surface, const vga_palette &palette, float contrast,
                    float brightness) {
        float float_data[vga::kVgaHeight][vga::kVgaWidth][3];
        for (int y = 0; y < vga::kVgaHeight; ++y) {
            for (int x = 0; x < vga::kVgaWidth; ++x) {
                unsigned char *pixel = data + y * (vga::kVgaWidth * 3) + x * 3;
                float_data[y][x][0] = adjust(pixel[0] / 255.f,contrast,brightness);
                float_data[y][x][1] = adjust(pixel[1] / 255.f,contrast,brightness);
                float_data[y][x][2] = adjust(pixel[2] / 255.f,contrast,brightness);
            }
        }

        uint8_t *surface_data = surface.data();
        for (int y = 0; y < vga::kVgaHeight; ++y) {
            for (int x = 0; x < vga::kVgaWidth; ++x) {
                float &r = float_data[y][x][0];
                float &g = float_data[y][x][1];
                float &b = float_data[y][x][2];

                float old_r = r;
                float old_g = g;
                float old_b = b;

                uint8_t new_pixel = palette.nearest(old_r, old_g, old_b);
                float new_r, new_g, new_b;
                palette.get(new_pixel, new_r, new_g, new_b);

                r = new_r;
                g = new_g;
                b = new_b;

                surface_data[y * vga::kVgaWidth + x] = new_pixel;
                float err_r = old_r - new_r;
                float err_g = old_g - new_g;
                float err_b = old_b - new_b;

                if (y < vga::kVgaHeight - 1) {
                    float_data[y + 1][x][0] = float_data[y + 1][x][0] + err_r * 5 / 16;
                    float_data[y + 1][x][1] = float_data[y + 1][x][1] + err_g * 5 / 16;
                    float_data[y + 1][x][2] = float_data[y + 1][x][2] + err_b * 5 / 16;
                    if (x > 0) {
                        float_data[y + 1][x - 1][0] = float_data[y + 1][x - 1][0] + err_r * 3 / 16;
                        float_data[y + 1][x - 1][1] = float_data[y + 1][x - 1][1] + err_g * 3 / 16;
                        float_data[y + 1][x - 1][2] = float_data[y + 1][x - 1][2] + err_b * 3 / 16;
                    }
                }
                if (x < vga::kVgaWidth - 1) {
                    float_data[y][x + 1][0] = float_data[y][x + 1][0] + err_r * 7 / 16;
                    float_data[y][x + 1][1] = float_data[y][x + 1][1] + err_g * 7 / 16;
                    float_data[y][x + 1][2] = float_data[y][x + 1][2] + err_b * 7 / 16;
                    if (y < vga::kVgaHeight - 1) {
                        float_data[y + 1][x + 1][0] = float_data[y + 1][x + 1][0] + err_r * 1 / 16;
                        float_data[y + 1][x + 1][1] = float_data[y + 1][x + 1][1] + err_g * 1 / 16;
                        float_data[y + 1][x + 1][2] = float_data[y + 1][x + 1][2] + err_b * 1 / 16;
                    }
                }
            }
        }
    }
}
