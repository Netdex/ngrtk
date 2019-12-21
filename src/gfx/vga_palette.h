#ifndef GFX_VGA_PALETTE_H
#define GFX_VGA_PALETTE_H

#include <cstring>
#include <cstdint>
#include <cstdlib>
#include "util/logging.h"
#include <cmath>
#include <climits>
#include <cfloat>

namespace gfx {
    inline uint8_t convert_rgb_value(float x) {
        CHECK(x >= 0.0f && x <= 1.0f);
        return static_cast<uint8_t>(x * 255.0f);
    }

    inline float convert_rgb_value(uint8_t x) {
        return x / 255.0f;
    }

    class vga_palette {
    public:
        vga_palette(uint8_t *palette_data, size_t num_colors) : num_colors_(num_colors) {
            memcpy(palette_data_, palette_data, static_cast<size_t>(num_colors * 3) * sizeof(uint8_t));
        }

        void get(uint8_t index, uint8_t &r, uint8_t &g, uint8_t &b) const {
            CHECK(index < num_colors_);
            r = palette_data_[index * 3];
            g = palette_data_[index * 3 + 1];
            b = palette_data_[index * 3 + 2];
        }

        void get(uint8_t index, float &fr, float &fg, float &fb) const {
            CHECK(index < num_colors_);
            fr = convert_rgb_value(palette_data_[index * 3]);
            fg = convert_rgb_value(palette_data_[index * 3 + 1]);
            fb = convert_rgb_value(palette_data_[index * 3 + 2]);
        }

        uint8_t nearest(uint8_t r, uint8_t g, uint8_t b) const {
            float fr = convert_rgb_value(r), fg = convert_rgb_value(g), fb = convert_rgb_value(b);
            return nearest(fr, fg, fb);
        }

        uint8_t nearest(float fr, float fg, float fb) const {
            uint8_t nearest_palette = UINT8_MAX;
            float min_distsq = FLT_MAX;
            for (uint8_t i = 0; i < num_colors_; ++i) {
                float fcr, fcg, fcb;
                get(i, fcr, fcg, fcb);
                float distsq = (fcr - fr) * (fcr - fr) + (fcg - fg) * (fcg - fg) + (fcb - fb) * (fcb - fb);
                if (distsq < min_distsq) {
                    min_distsq = distsq;
                    nearest_palette = i;
                }
            }
            return nearest_palette;
        }

    private:
        uint8_t palette_data_[vga::kNumColors * 3] = {0};
        size_t num_colors_;
    };
}


#endif
