
#ifndef GFX_SURFACE_H
#define GFX_SURFACE_H

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "io/vga_dos.h"
#include "gfx/vga_palette.h"

#include "third_party/stb/stb_image_write.h"

namespace gfx {
    template<size_t W, size_t H>
    class surface {
    public:
        surface() : buffer_(new uint8_t[size_]), is_buffer_owner(true) {}

        surface(uint8_t *buffer) : buffer_(buffer), is_buffer_owner(false) {}

        ~surface() {
            if (is_buffer_owner) {
                delete[] buffer_;
            }
        }

        surface(surface &&o) = delete;

        surface(const surface &o) = delete;

        surface &operator=(const surface &o) = delete;

        friend void swap(surface &a, surface &b) {
            std::swap(a.buffer_, b.buffer_);
            std::swap(a.is_buffer_owner, b.is_buffer_owner);
        }

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

        void save(char *filename, const vga_palette &palette) {
            constexpr int comp = 3;
            uint8_t image[size_ * comp];
            for (size_t y = 0; y < H; ++y) {
                for (size_t x = 0; x < W; ++x) {
                    uint8_t r, g, b;
                    palette.get(buffer_[y * W + x], r, g, b);
                    image[y * (comp * W) + comp * x] = r;
                    image[y * (comp * W) + comp * x + 1] = g;
                    image[y * (comp * W) + comp * x + 2] = b;
                }
            }
            stbi_write_png(filename, static_cast<int>(W), static_cast<int>(H), comp, image, static_cast<int>(W) * comp);
        }

        uint8_t *data() {
            return buffer_;
        }


    private:
        static constexpr size_t size_ = W * H;
        uint8_t *buffer_;
        bool is_buffer_owner;
    };

    using vga_surface = gfx::surface<vga::kVgaWidth, vga::kVgaHeight>;
}

#endif
