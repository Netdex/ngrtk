#include "io/vga_dos.h"

#include <memory>
#include <cstdio>
#include <cstring>

#include <pc.h>
#include <dos.h>
#include <sys/nearptr.h>
#include <gfx/surface.h>
#include <cassert>

#include "util/logging.h"

namespace vga {
    namespace {
        // BIOS video interrupt.
        constexpr int kVideoInterrupt = 0x10;
        // BIOS func for plotting a pixel.
        constexpr int kWriteDot = 0x0C;
        // BIOS func for setting the video mode.
        constexpr int kSetMode = 0x00;

        constexpr int kInputStatus1 = 0x03da;
        constexpr int kVretrace = 0x08;

        // Pointer to video memory.
        uint8_t *kVgaMemory = reinterpret_cast<uint8_t *>(0xA0000);
        std::unique_ptr<gfx::vga_surface> kVgaSurface;

        // Number of palette colors which can't be touched by SetPalette.
        constexpr size_t kReservedPalette = 16;
        uint8_t kVgaDefaultPalette[] = {
                /*  0: BLACK  */ 0, 0, 0,
                /*  0: BLACK  */ 64, 64, 64,
                /*  0: BLACK  */ 128, 128, 128,
                /*  0: BLACK  */ 192, 192, 192,
                /* 15: WHITE  */ 255, 255, 255,
                /*  1: BLUE   */ 0, 0, 255,
                /*  2: CYAN   */ 0, 255, 255,
                /*  3: RED    */ 255, 0, 0,
                /*  4: PURPLE */ 255, 0, 255,
                /*  5: YELLOW */ 255, 255, 0,
                                 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0, 0,
        };
    }

    void init() {
        CHECK(__djgpp_nearptr_enable() != 0);
        kVgaMemory += __djgpp_conventional_base;
        kVgaSurface = std::make_unique<gfx::surface<kVgaWidth, kVgaHeight>>();
        // __djgpp_nearptr_disable();
    }

    void set_mode(Mode mode) {
        union REGS regs{};
        regs.h.ah = kSetMode;
        regs.h.al = static_cast<unsigned char>(mode);
        int86(kVideoInterrupt, &regs, &regs);
    }

    void plot(int x, int y, uint8_t color) {
        assert(x >= 0 && y >= 0 && x < kVgaWidth && y < kVgaHeight);
        kVgaSurface->plot(x, y, color);
    }

    void clear(uint8_t color) {
        kVgaSurface->set(color);
    }

    uint8_t *get_buffer() {
        return kVgaSurface->data();
    }

    uint8_t *buffer() {
        return kVgaSurface->data();
    }

    void swap(int flags) {
        if (flags & kVgaWaitRetrace) wait_retrace();
        kVgaSurface->copy_to(kVgaMemory);
        if (flags & kVgaClear) {
            clear(0);
        }
    }

    void wait_retrace() {
        while ((inp(kInputStatus1) & kVretrace));
        while (!(inp(kInputStatus1) & kVretrace));
    }

    void set_palette_index(int index, uint8_t r, uint8_t g, uint8_t b) {
        outp(0x03c8, static_cast<uint8_t>(index));
        outp(0x03c9, r >> 2);
        outp(0x03c9, g >> 2);
        outp(0x03c9, b >> 2);
    }

    void set_palette(uint8_t *palette, size_t palette_size) {
        outp(0x03c8, 0);
        CHECK(palette_size <= kNumColors - kReservedPalette);

        for (size_t i = 0; i < kReservedPalette * 3; i++) {
            outp(0x03c9, kVgaDefaultPalette[i] >> 2);
        }
        for (size_t i = 0; i < palette_size * 3; i++) {
            outp(0x03c9, palette[i] >> 2);
        }
    }

    void set_default_palette() {
        set_palette(nullptr, 0);
    }

    void reset() {
        set_mode(Mode::kTextMode);
    }

}
