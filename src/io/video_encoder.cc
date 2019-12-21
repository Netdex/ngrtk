#include "io/video_coder.h"

#include "third_party/lz4/lz4hc.h"

namespace io {
    video_encoder::video_encoder(std::ostream &stream) : stream_(stream) {
        CHECK(stream_.good());
    }

    void video_encoder::encode_header(video_file_hdr &hdr) {
        CHECK(stream_.good());
        stream_.write(reinterpret_cast<const char *>(&hdr.frame_count), sizeof(hdr.frame_count));
        stream_.write(reinterpret_cast<const char *>(&hdr.framerate), sizeof(hdr.framerate));
        stream_.write(reinterpret_cast<const char *>(&hdr.palette_size), sizeof(hdr.palette_size));
        stream_.write(reinterpret_cast<const char *>(hdr.palette_data), hdr.palette_size);
    }

    void video_encoder::encode_frame(video_frame_hdr &hdr) {
        CHECK(stream_.good());
        char cmp_buf[LZ4_COMPRESSBOUND(kBufferSize)];
        int cmp_bytes = LZ4_compress_HC_continue(lz4_stream_, reinterpret_cast<char *>(get_surface().data()),
                                                 cmp_buf,
                                                 kBufferSize, sizeof(cmp_buf));
        CHECK(cmp_bytes > 0);
        hdr.compress_size = static_cast<uint32_t>(cmp_bytes);
        stream_.write(reinterpret_cast<const char *>(&hdr.compress_size), sizeof(hdr.compress_size));
        stream_.write(cmp_buf, hdr.compress_size);
        swap();
    }

}
