#ifndef VGA_VGA_H_
#define VGA_VGA_H_

#include <cstdint>
#include <cstdlib>


namespace vga {
    // Width in pixels of mode 13h.
    constexpr int kVgaWidth = 320;
    // Height in pixels of mode 13h.
    constexpr int kVgaHeight = 200;
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

    void init();

    void set_mode(Mode mode);

    void plot(int x, int y, uint8_t color);

    void clear(uint8_t color);

    uint8_t *get_buffer();

    void swap(int flags = SwapFlags::kNone);

    void wait_retrace();

    void set_palette_index(size_t index, uint8_t r, uint8_t g, uint8_t b);

    void set_palette(uint8_t *palette, size_t palette_size);

    void set_default_palette();

    void reset();
}

#endif
