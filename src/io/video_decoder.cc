#include "io/video_coder.h"

namespace io {
    video_decoder::video_decoder(std::istream &stream) : stream_(stream) {
        CHECK(stream_.good());
        LZ4_setStreamDecode(lz4_stream_, nullptr, 0);
    }

    void video_decoder::decode_header(video_file_hdr &hdr) {
        CHECK(stream_.good());
        stream_.read(reinterpret_cast<char *>(&hdr.frame_count), sizeof(hdr.frame_count));
    }

    gfx::vga_surface &video_decoder::decode_frame(video_frame_hdr &hdr) {
        CHECK(stream_.good());
        char cmp_buf[LZ4_COMPRESSBOUND(kBufferSize)];
        stream_.read(reinterpret_cast<char *>(&hdr.compress_size), sizeof(hdr.compress_size));
        CHECK(hdr.compress_size < sizeof(cmp_buf));
        stream_.read(cmp_buf, static_cast<std::streamsize>(hdr.compress_size));
        int dec_bytes = LZ4_decompress_safe_continue(lz4_stream_, cmp_buf,
                                                     reinterpret_cast<char *>(get_surface().data()),
                                                     static_cast<int>(hdr.compress_size), kBufferSize);
        CHECK(dec_bytes > 0);
        gfx::vga_surface &frame = get_surface();
        swap();
        return frame;
    }

}