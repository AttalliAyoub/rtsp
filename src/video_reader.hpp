#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <inttypes.h>
}

using namespace std;

struct VideoReaderState {
    //public
    int width, height;
    uint8_t* frame_buffer;
    // private
    int video_stream_index;
    AVFormatContext* av_format_ctx;
    AVCodecContext* av_codec_ctx;
    AVFrame* av_frame;
	AVPacket* av_packet;
    SwsContext* sws_scaler_ctx;
    bool sws_scaler_ctx_state;
};

bool video_reader_open(VideoReaderState* state, const char* filePath);
bool video_reader_read_frame(VideoReaderState* state, uint8_t* frame_buffer);
void video_reader_close(VideoReaderState* state);

bool load_frame(const char* filePath, int* width_out, int* height_out, uint8_t** data_out);