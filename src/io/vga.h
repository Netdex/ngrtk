#ifndef VGA_VGA_H_
#define VGA_VGA_H_

#include <cstdint>
#include <cstdlib>


namespace vga {
    // Width in pixels of mode 13h.
    constexpr size_t kVgaWidth = 320;
    // Height in pixels of mode 13h.
    constexpr size_t kVgaHeight = 200;
    // Number of colors in mode 13h.
    constexpr size_t kNumColors = 256;

    enum class Mode : unsigned char {
        kVga256ColorMode = 0x13,
        kTextMode = 0x03
    };

    enum SwapFlags : int {
        kNone = 0,
        kVgaClear = 0x1,
        kVgaWaitRetrace = 0x2
    };

    void Init();

    void SetMode(Mode mode);

    void Plot(size_t x, size_t y, uint8_t color);

    void Clear(uint8_t color);

    void Swap(int flags = SwapFlags::kNone);

    void WaitRetrace();

    void SetPaletteIndex(size_t index, uint8_t r, uint8_t g, uint8_t b);

    void SetPalette(uint8_t *palette, size_t palette_size);

    void SetDefaultPalette();

    void Reset();
}

#endif
