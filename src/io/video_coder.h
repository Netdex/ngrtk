#ifndef IO_VIDEO_CODER_H
#define IO_VIDEO_CODER_H

#include <iostream>
#include <cstdint>

#include "gfx/surface.h"
#include "third_party/lz4/lz4.h"
#include "third_party/lz4/lz4hc.h"

namespace io {
    struct video_file_hdr {
        uint32_t frame_count;
    };

    struct video_frame_hdr {
        uint32_t compress_size;
    };

    class video_coder {
    public:
        video_coder() = default;

        video_coder(const video_coder &o) = delete;

        video_coder &operator=(const video_coder &o) = delete;

        gfx::vga_surface *get_surface() {
            return active_surface;
        }

    protected:
        void swap() {
            active_surface = (active_surface == &front_surface) ? &back_surface : &front_surface;
        }

        gfx::vga_surface front_surface;
        gfx::vga_surface back_surface;
        gfx::vga_surface *active_surface = &front_surface;

        static const size_t kBufferSize = vga::kVgaWidth * vga::kVgaHeight;
    };

    class video_encoder : public video_coder {
    public:
        video_encoder(std::ostream &stream);

        void encode_header(video_file_hdr &hdr);

        void encode_frame(video_frame_hdr &hdr);

    private:
        std::ostream &stream;

        LZ4_streamHC_t lz4_stream_body;
        LZ4_streamHC_t *lz4_stream = &lz4_stream_body;
    };

    class video_decoder : public video_coder {
    public:
        video_decoder(std::istream &stream);

        void decode_header(video_file_hdr &hdr);

        void decode_frame(video_frame_hdr &hdr);

    private:
        std::istream &stream;

        LZ4_streamDecode_t lz4_stream_body;
        LZ4_streamDecode_t *lz4_stream = &lz4_stream_body;
    };
}


#endif
