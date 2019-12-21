
#include <iostream>
#include <filesystem>
#include <vector>
#include "util/logging.h"
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
    std::cerr << "Usage: " << argv[0]
              << "-s <start> -n <count> -p palette -o filename [-b <brightness>] [-c <contrast>] [-x] <prefix>"
              << std::endl;
}

int main(int argc, char *argv[]) {
    long start = 0;
    long count = 0;
    float contrast = 1.0f;
    float brightness = 0.0f;
    bool save = false;
    size_t palette_size = 0;
    uint8_t palette_data[vga::kNumColors * 3];
    float framerate = 0.0f;
    const char ext[] = "png";

    char filename[256] = {0};

    int opt;
    while ((opt = getopt(argc, argv, "xo:n:p:f:s::b::c::")) != -1) {
        switch (opt) {
            case 's': {
                char *endptr;
                CHECK(optarg != nullptr);
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
                CHECK(optarg != nullptr);
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
                CHECK(optarg != nullptr);
                brightness = std::strtof(optarg, &endptr);
                CHECK(brightness >= -1.0f && brightness <= 1.0f);
                if (endptr == optarg || *endptr != '\0') {
                    std::cerr << argv[0] << ": \"" << optarg << "\" is invalid format for -b, expected scalar float"
                              << std::endl;
                    return EXIT_FAILURE;
                }
                break;
            }
            case 'c': {
                char *endptr;
                CHECK(optarg != nullptr);
                contrast = std::strtof(optarg, &endptr);
                if (endptr == optarg || *endptr != '\0') {
                    std::cerr << argv[0] << ": \"" << optarg << "\" is invalid format for -c, expected scalar float"
                              << std::endl;
                    return EXIT_FAILURE;
                }
                break;
            }
            case 'f': {
                char *endptr;
                CHECK(optarg != nullptr);
                framerate = std::strtof(optarg, &endptr);
                if (endptr == optarg || *endptr != '\0') {
                    std::cerr << argv[0] << ": \"" << optarg << "\" is invalid format for -f, expected scalar float"
                              << std::endl;
                    return EXIT_FAILURE;
                }
                break;
            }
            case 'p': {
                palette_size = 0;
                char *endptr = optarg;
                CHECK(optarg != nullptr);
                while (*endptr != '\0') {
                    long val = std::strtol(endptr, &endptr, 0);
                    if (optarg == endptr) {
                        std::cerr << argv[0]
                                  << ": \"" << optarg << "\" is invalid format for -p, expected vector integral"
                                  << std::endl;
                        return EXIT_FAILURE;
                    }
                    CHECK(val >= 0 && val < 256);
                    if (palette_size > vga::kNumColors * 3) {
                        std::cerr << argv[0] << ": palette data is too large, limit to 256 colors" << std::endl;
                        return EXIT_FAILURE;
                    }
                    palette_data[palette_size] = static_cast<uint8_t>(val);
                    ++palette_size;
                }
                CHECK(palette_size >= 6 && palette_size % 3 == 0);
                break;
            }
            case 'x': {
                save = true;
                break;
            }
            case 'o': {
                CHECK(optarg != nullptr);
                int retval = snprintf(filename, sizeof(filename), "%s", optarg);
                CHECK(retval > 0);
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

    CHECK(count > 0);
    CHECK(framerate > 0.0f);

    std::cout << "framepack: " << count << " frames @ " << framerate << " fps, prefix " << prefix << ", start "
              << start << ", output " << filename << std::endl;

    const gfx::vga_palette my_palette(palette_data, palette_size);

    std::ofstream outf(filename, std::ios::binary);
    CHECK(outf.good());

    io::video_encoder encoder(outf);

    io::video_file_hdr file_hdr{};
    file_hdr.frame_count = static_cast<uint32_t>(count);
    file_hdr.framerate = framerate;
    file_hdr.palette_size = static_cast<uint16_t>(palette_size);
    std::copy(palette_data, palette_data + palette_size, file_hdr.palette_data);
    encoder.encode_header(file_hdr);

    for (long i = start; i < start + count; ++i) {
        int retval = snprintf(filename, 256, "%s%06ld.%s", prefix, i, ext);
        CHECK(retval > 0);
        std::cout << "[" << (i - start) << " / " << count << "]" << " " << filename << "\r";

        int x, y, n;
        unsigned char *data = stbi_load(filename, &x, &y, &n, 3);
        CHECK(data != nullptr);
        CHECK(x == vga::kVgaWidth && y == vga::kVgaHeight);

        gfx::vga_dither(data, encoder.get_surface(), my_palette, contrast, brightness);
        stbi_image_free(data);

        if (save) {
            char png_filename[256];
            retval = snprintf(png_filename, sizeof(png_filename), "%s%06ld.quant.%s", prefix, i, ext);
            CHECK(retval > 0);
            encoder.get_surface().save(png_filename, my_palette);
        }

        io::video_frame_hdr frame_hdr{};
        encoder.encode_frame(frame_hdr);

//        gfx::vga_dither(data, frame, my_palette, 1.38f, 0.03f);
    }
    std::cout << std::endl;
    outf.close();
    return 0;
}