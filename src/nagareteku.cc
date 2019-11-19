#include <iostream>
#include <cmath>
#include <pc.h>

#include "util/logging_dos.h"
#include "io/vga_dos.h"

int main(int argc, const char *argv[]) {
    vga::init();
    vga::set_mode(vga::Mode::kVga256ColorMode);
    vga::set_default_palette();

    size_t time = 0;
    while (true) {
        sound((time * 10) % 100);
        for (size_t y = 0; y < vga::kVgaHeight; ++y) {
            for (size_t x = 0; x < vga::kVgaWidth; ++x) {
//                vga::Plot(x, y,
//                          static_cast<uint8_t>(sin(x * y + time * 0.1f) * 6));
                vga::plot(x, y, (x + time) % 6);
            }
        }
        ++time;
        vga::swap(vga::SwapFlags::kVgaWaitRetrace | vga::SwapFlags::kVgaClear);
    }
    std::cin.ignore();
    vga::reset();
    return 0;
}