#ifndef IO_VIDEO_CODER_H
#define IO_VIDEO_CODER_H

#include <iostream>
#include <cstdint>

#include "io/vga_dos.h"
#include "gfx/surface.h"
#include "third_party/lz4/lz4.h"
#include "third_party/lz4/lz4hc.h"

namespace io {
    struct video_file_hdr {
        uint32_t frame_count;
        float framerate;
        uint16_t palette_size;
        uint8_t palette_data[3 * vga::kNumColors];
    };

    struct video_frame_hdr {
        uint32_t compress_size;
    };

    class video_coder {
    public:
        video_coder() : front_surface(double_buffer[0]), back_surface(double_buffer[1]) {}

        video_coder(const video_coder &o) = delete;

        video_coder &operator=(const video_coder &o) = delete;

        gfx::vga_surface& get_surface() {
            return *active_surface;
        }

    protected:
        static const size_t kBufferSize = vga::kVgaWidth * vga::kVgaHeight;

        void swap() {
            active_surface = (active_surface == &front_surface) ? &back_surface : &front_surface;
        }

        gfx::vga_surface front_surface;
        gfx::vga_surface back_surface;
        gfx::vga_surface *active_surface = &front_surface;

        uint8_t double_buffer[2][kBufferSize] = {0};
    };

    class video_encoder : public video_coder {
    public:
        explicit video_encoder(std::ostream &stream);

        void encode_header(video_file_hdr &hdr);

        void encode_frame(video_frame_hdr &hdr);

    private:
        std::ostream &stream_;

        LZ4_streamHC_t lz4_stream_body_;
        LZ4_streamHC_t *lz4_stream_ = &lz4_stream_body_;
    };

    class video_decoder : public video_coder {
    public:
        explicit video_decoder(std::istream &stream);

        void decode_header(video_file_hdr &hdr);

        gfx::vga_surface& decode_frame(video_frame_hdr &hdr);

    private:
        std::istream &stream_;

        LZ4_streamDecode_t lz4_stream_body_;
        LZ4_streamDecode_t *lz4_stream_ = &lz4_stream_body_;
    };
}


#endif
