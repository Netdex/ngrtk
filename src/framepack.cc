
#include <iostream>
#include <filesystem>
#include <vector>
#include <cassert>
#include <fstream>

#include <getopt.h>

#include "gfx/surface.h"
#include "gfx/dither.h"
#include "io/vga_dos.h"

#include "third_party/stb/stb_image.h"

void print_usage(int argc, char *argv[]) {
    std::cerr << "Usage: " << argv[0] << " -n <count>" << std::endl;
}

int main(int argc, char *argv[]) {
    long count = 0;

    int opt;
    while ((opt = getopt(argc, argv, "n:")) != -1) {
        switch (opt) {
            case 'n':
                count = std::strtol(optarg, nullptr, 0);
                break;
            default:
                print_usage(argc, argv);
                exit(EXIT_FAILURE);
        }
    }
    if (optind >= argc) {
        print_usage(argc, argv);
        exit(EXIT_FAILURE);
    }
    char *prefix = argv[optind];
    assert(count > 0);

    std::cout << "framepack: processing " << count << " images with prefix " << prefix << std::endl;

    std::vector<gfx::vga_surface> frames;
    for (long i = 0; i < count; ++i) {
        char filename[256];
        snprintf(filename, 256, "%s%06ld.png", prefix, i);
        std::cout << filename << std::endl;

        int x, y, n;
        unsigned char *data = stbi_load(filename, &x, &y, &n, 3);
        assert(data != nullptr);
        assert(x == vga::kVgaWidth && y == vga::kVgaHeight);

        gfx::vga_surface frame;
        gfx::vga_dither(data, frame, 1.5f, 0);
        snprintf(filename, 256, "%s%06ld.quant.png", prefix, i);
        frame.save(filename);

        stbi_image_free(data);
    }


    return 0;
}