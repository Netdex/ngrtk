
#ifndef GFX_SURFACE_H
#define GFX_SURFACE_H

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "io/vga_dos.h"

#include "third_party/stb/stb_image_write.h"

namespace gfx {
    template<size_t W, size_t H>
    class surface {
    public:
        surface() : buffer_{0} {}

        void copy_to(surface<W, H> &other) const {
            copy_to(other.buffer_);
        }

        void copy_to(uint8_t *dest) const {
            memcpy(dest, buffer_, size_);
        }

        void set(uint8_t value) {
            memset(buffer_, value, size_);
        }

        void plot(int x, int y, uint8_t color) {
            buffer_[(y << 8) + (y << 6) + x] = color;
        }

        uint8_t get(int x, int y) {
            return buffer_[(y << 8) + (y << 6) + x];
        }

        void save(char *filename) {
            // TODO use palette data?
            uint8_t image[size_];
            for (size_t y = 0; y < H; ++y) {
                for (size_t x = 0; x < W; ++x) {
                    image[y * W + x] = buffer_[y * W + x] == 1 ? 255 : 0;
                }
            }
            stbi_write_png(filename, static_cast<int>(W), static_cast<int>(H), 1, image, static_cast<int>(W));
        }

        uint8_t *data() {
            return buffer_;
        }

    private:
        static constexpr size_t size_ = W * H;
        uint8_t buffer_[size_];
    };

    using vga_surface = gfx::surface<vga::kVgaWidth, vga::kVgaHeight>;
}

#endif
