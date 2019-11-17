#include "io/vga.h"

#include <cstdio>
#include <cstring>

#include <pc.h>
#include <dos.h>
#include <sys/nearptr.h>

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
        // Pointer to double buffer.
        uint8_t *kVgaBuffer = nullptr;

        // Number of palette colors which can't be touched by SetPalette.
        constexpr size_t kReservedPalette = 16;
        uint8_t kVgaDefaultPalette[] = {
                /*  0: BLACK  */ 0, 0, 0,
                /*  1: BLUE   */ 0, 0, 255,
                /*  2: CYAN   */ 0, 255, 255,
                /*  3: RED    */ 255, 0, 0,
                /*  4: PURPLE */ 255, 0, 255,
                /*  5: YELLOW */ 255, 255, 0,
                0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0,
                /* 15: WHITE  */ 255, 255, 255,
        };
    }

    void Init() {
        ASSERT(__djgpp_nearptr_enable() != 0);
        ASSERT(kVgaBuffer == nullptr);
        kVgaBuffer = new uint8_t[kVgaWidth * kVgaHeight];
        kVgaMemory += __djgpp_conventional_base;
        // __djgpp_nearptr_disable();
    }

    void SetMode(Mode mode) {
        union REGS regs{};
        regs.h.ah = kSetMode;
        regs.h.al = static_cast<unsigned char>(mode);
        int86(kVideoInterrupt, &regs, &regs);
    }

    void Plot(size_t x, size_t y, uint8_t color) {
        kVgaBuffer[(y << 8) + (y << 6) + x] = color;
    }

    void Clear(uint8_t color) {
        memset(kVgaBuffer, color, kVgaWidth * kVgaHeight);
    }

    void Swap(int flags) {
        if (flags & kVgaWaitRetrace) WaitRetrace();
        memcpy(kVgaMemory, kVgaBuffer, kVgaWidth * kVgaHeight);
        if (flags & kVgaClear) {
            Clear(0);
        }
    }

    void WaitRetrace() {
        while ((inp(kInputStatus1) & kVretrace));
        while (!(inp(kInputStatus1) & kVretrace));
    }

    void SetPaletteIndex(size_t index, uint8_t r, uint8_t g, uint8_t b) {
        outp(0x03c8, static_cast<uint8_t>(index));
        outp(0x03c9, r >> 2);
        outp(0x03c9, g >> 2);
        outp(0x03c9, b >> 2);
    }

    void SetPalette(uint8_t *palette, size_t palette_size) {
        outp(0x03c8, 0);
        ASSERT(palette_size <= kNumColors - kReservedPalette);

        for (size_t i = 0; i < kReservedPalette * 3; i++) {
            outp(0x03c9, kVgaDefaultPalette[i] >> 2);
        }
        for (size_t i = 0; i < palette_size * 3; i++) {
            outp(0x03c9, palette[i] >> 2);
        }
    }

    void SetDefaultPalette(){
        SetPalette(nullptr, 0);
    }

    void Reset() {
        SetMode(Mode::kTextMode);
        delete kVgaBuffer;
    }

}
