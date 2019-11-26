#include "io/video_coder.h"

#include "third_party/lz4/lz4hc.h"

namespace io {
    video_encoder::video_encoder(std::ostream &stream) : stream(stream) {}

    void video_encoder::encode_header(video_file_hdr &hdr) {
        stream.write(reinterpret_cast<const char *>(&hdr.frame_count), sizeof(hdr.frame_count));
    }

    void video_encoder::encode_frame(video_frame_hdr &hdr) {
        char cmp_buf[LZ4_COMPRESSBOUND(kBufferSize)];
        int cmp_bytes = LZ4_compress_HC_continue(lz4_stream, reinterpret_cast<char *>(get_surface()->data()),
                                                 cmp_buf,
                                                 kBufferSize, sizeof(cmp_buf));
        assert(cmp_bytes > 0);
        hdr.compress_size = static_cast<uint32_t>(cmp_bytes);
        stream.write(reinterpret_cast<const char *>(&hdr.compress_size), sizeof(hdr.compress_size));
        stream.write(cmp_buf, cmp_bytes);
        swap();
    }

}
