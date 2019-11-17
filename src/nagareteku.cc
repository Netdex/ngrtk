#include <iostream>
#include <cmath>
#include <pc.h>

#include "util/logging.h"
#include "io/vga.h"

int main(int argc, const char *argv[]) {
    vga::Init();
    vga::SetMode(vga::Mode::kVga256ColorMode);
    vga::SetDefaultPalette();

    size_t time = 0;
    while (true) {
        sound(time * 10);
        for (size_t y = 0; y < vga::kVgaHeight; ++y) {
            for (size_t x = 0; x < vga::kVgaWidth; ++x) {
                vga::Plot(x, y,
                          static_cast<uint8_t>(sin(x * y + time * 0.1f) * 6));
            }
        }
        ++time;
        vga::Swap(vga::SwapFlags::kVgaWaitRetrace | vga::SwapFlags::kVgaClear);
    }
    std::cin.ignore();
    vga::Reset();
    return 0;
}