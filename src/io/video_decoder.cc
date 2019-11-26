#include "io/video_coder.h"

namespace io {
    video_decoder::video_decoder(std::istream &stream) : stream(stream) {}

    void video_decoder::decode_header(video_file_hdr &hdr) {
        stream.read(reinterpret_cast<char *>(&hdr.frame_count), sizeof(hdr.frame_count));
    }

    void video_decoder::decode_frame(video_frame_hdr &hdr) {
        stream.read(reinterpret_cast<char *>(&hdr.compress_size), sizeof(hdr.compress_size));
        char cmp_buf[LZ4_COMPRESSBOUND(kBufferSize)];
        stream.read(cmp_buf, hdr.compress_size);
        int dec_bytes = LZ4_decompress_safe_continue(lz4_stream, cmp_buf,
                                                     reinterpret_cast<char *>(get_surface()->data()),
                                                     hdr.compress_size, kBufferSize);
        assert(dec_bytes > 0);
        swap();
    }

}