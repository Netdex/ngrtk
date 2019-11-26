
#include <iostream>
#include <filesystem>
#include <vector>
#include <cassert>
#include <fstream>

#include <getopt.h>

#include "gfx/surface.h"
#include "gfx/dither.h"
#include "gfx/vga_palette.h"
#include "io/video_coder.h"
#include "io/vga_dos.h"

#include "third_party/stb/stb_image.h"
#include "third_party/lz4/lz4.h"
#include "third_party/lz4/lz4hc.h"

void print_usage(int argc, char *argv[]) {
    std::cerr << "Usage: " << argv[0] << "-s <start> -n <count> -p palette [-b <brightness>] [-c <contrast>] <prefix>"
              << std::endl;
}

int main(int argc, char *argv[]) {
    long start = 0;
    long count = 0;
    float contrast = 1.0f;
    float brightness = 0.0f;
    size_t palette_size = 0;
    uint8_t palette_data[vga::kNumColors * 3];
    const char ext[] = "png";

    int opt;
    while ((opt = getopt(argc, argv, "s::n:p:b::c::")) != -1) {
        switch (opt) {
            case 's': {
                char *endptr;
                assert(optarg != nullptr);
                start = std::strtol(optarg, &endptr, 0);
                if (endptr == optarg || *endptr != '\0') {
                    std::cerr << argv[0] << ": \"" << optarg << "\" is invalid format for -s, expected scalar integral"
                              << std::endl;
                    return EXIT_FAILURE;
                }
                break;
            }
            case 'n': {
                char *endptr;
                assert(optarg != nullptr);
                count = std::strtol(optarg, &endptr, 0);
                if (endptr == optarg || *endptr != '\0') {
                    std::cerr << argv[0] << ": \"" << optarg << "\" is invalid format for -n, expected scalar integral"
                              << std::endl;
                    return EXIT_FAILURE;
                }
                break;
            }
            case 'b': {
                char *endptr;
                assert(optarg != nullptr);
                brightness = std::strtof(optarg, &endptr);
                assert(brightness >= -1.0f && brightness <= 1.0f);
                if (endptr == optarg || *endptr != '\0') {
                    std::cerr << argv[0] << ": \"" << optarg << "\" is invalid format for -b, expected scalar float"
                              << std::endl;
                    return EXIT_FAILURE;
                }
                break;
            }
            case 'c': {
                char *endptr;
                assert(optarg != nullptr);
                contrast = std::strtof(optarg, &endptr);
                if (endptr == optarg || *endptr != '\0') {
                    std::cerr << argv[0] << ": \"" << optarg << "\" is invalid format for -c, expected scalar float"
                              << std::endl;
                    return EXIT_FAILURE;
                }
                break;
            }
            case 'p': {
                palette_size = 0;
                char *endptr = optarg;
                assert(optarg != nullptr);
                while (*endptr != '\0') {
                    long val = std::strtol(endptr, &endptr, 0);
                    if (optarg == endptr) {
                        std::cerr << argv[0]
                                  << ": \"" << optarg << "\" is invalid format for -p, expected vector integral"
                                  << std::endl;
                        return EXIT_FAILURE;
                    }
                    assert(val >= 0 && val < 256);
                    if (palette_size >= vga::kNumColors * 3) {
                        std::cerr << argv[0] << ": palette data is too large, limit to 256 colors" << std::endl;
                        return EXIT_FAILURE;
                    }
                    palette_data[palette_size] = static_cast<uint8_t>(val);
                    ++palette_size;
                }
                assert(palette_size >= 6 && palette_size % 3 == 0);
                break;
            }
            default:
                print_usage(argc, argv);
                return EXIT_FAILURE;
        }
    }
    if (optind >= argc) {
        print_usage(argc, argv);
        return EXIT_FAILURE;
    }
    char *prefix = argv[optind];
    assert(count > 0);

    std::cout << "framepack: processing " << count << " images, with prefix " << prefix << ", starting with index "
              << start << std::endl;

    const gfx::vga_palette my_palette(palette_data, static_cast<uint8_t>(palette_size));

    char filename[256];
    snprintf(filename, 256, "%s.lz4", prefix);
    std::ofstream outf(filename);
    assert(outf.good());

    io::video_encoder encoder(outf);
    io::video_file_hdr file_hdr{};
    file_hdr.frame_count = static_cast<uint32_t>(count);
    encoder.encode_header(file_hdr);

    for (long i = start; i < start + count; ++i) {
        snprintf(filename, 256, "%s%06ld.%s", prefix, i, ext);
        std::cout << filename << std::endl;

        int x, y, n;
        unsigned char *data = stbi_load(filename, &x, &y, &n, 3);
        assert(data != nullptr);
        assert(x == vga::kVgaWidth && y == vga::kVgaHeight);

        gfx::vga_dither(data, *encoder.get_surface(), my_palette, contrast, brightness);
        stbi_image_free(data);

        io::video_frame_hdr frame_hdr{};
        encoder.encode_frame(frame_hdr);

//        gfx::vga_dither(data, frame, my_palette, 1.38f, 0.03f);
//        snprintf(filename, 256, "%s%06ld.quant.%s", prefix, i, ext);
//        frame.save(filename, my_palette);
    }
    outf.close();
    return 0;
}