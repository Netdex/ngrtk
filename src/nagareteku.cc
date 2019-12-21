#include <iostream>
#include <cmath>
#include <fstream>

#include <pc.h>
#include <dos.h>

#include "util/logging.h"
#include "io/vga_dos.h"
#include "io/video_coder.h"

int main(int argc, const char *argv[]) {
    vga::init();
    vga::set_mode(vga::Mode::kVga256ColorMode);
//    vga::set_default_palette();

    std::ifstream video_file("video.lz4", std::ios::binary);
    io::video_decoder decoder(video_file);
    io::video_file_hdr video_hdr{};
    decoder.decode_header(video_hdr);
    vga::set_palette(video_hdr.palette_data, video_hdr.palette_size);

    for (size_t i = 0; i < video_hdr.frame_count; ++i) {
        io::video_frame_hdr frame_hdr{};
        decoder.decode_frame(frame_hdr).copy_to(vga::get_buffer());
        vga::swap(vga::SwapFlags::kVgaWaitRetrace);
    }
    video_file.close();
    std::cin.ignore();

    vga::reset();
    return 0;
}