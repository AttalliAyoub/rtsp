#include "load_frame.hpp"

bool load_frame(const char* filePath, int* width_out, int* height_out, uint8_t** data_out) {
	AVFormatContext* av_format_ctx = avformat_alloc_context();
	if (!av_format_ctx) {
		cout << "couldn't allocat avformat\n";
		return false;
	}
	if (avformat_open_input(&av_format_ctx, filePath, NULL, NULL) != 0) {
		cout << "couldn't open file\n";
		return false;
	}

	int video_stream_index = -1;

	AVCodecParameters* codec_parms;
	AVCodec* av_codec;
	for (int i = 0; i < av_format_ctx->nb_streams; ++i) {
		auto stream = av_format_ctx->streams[i];
		codec_parms = stream->codecpar;
		av_codec = avcodec_find_decoder(codec_parms->codec_id);
		if (!av_codec) {
			continue;
		}
		if (av_codec->type == AVMEDIA_TYPE_VIDEO) {
			video_stream_index = i;
			break;
		}
	}

	if (video_stream_index == -1) {
		cout << "couldn't find video \n";
		return false;
	}

	AVCodecContext* av_codec_ctx = avcodec_alloc_context3(av_codec);
	if (!av_codec_ctx) {
		cout << "couldn't allocat av_codec_ctx \n";
		return false;
	}

	if (avcodec_parameters_to_context(av_codec_ctx, codec_parms) < 0) {
		cout << "could't init context\n";
		return false;
	}

	if (avcodec_open2(av_codec_ctx, av_codec, NULL) < 0) {
		cout << "couldn't open codec\n";
		return false;
	}

    AVFrame* av_frame = av_frame_alloc();
    if (!av_frame) {
        cout << "couldn't allocate frame\n";
        return false;
    }
    AVPacket* av_packet = av_packet_alloc();
    if (!av_packet) {
        cout << "couldn't allocate packet\n";
        return false;
    }

    int resp;
    while (av_read_frame(av_format_ctx, av_packet) >= 0) {
        if (av_packet->stream_index != video_stream_index) continue;
        resp = avcodec_send_packet(av_codec_ctx, av_packet);
        if (resp < 0) {
            // printf("couldn't decode packet: %s\n", av_err2str(resp));
            // cout << "couldn't decode packet " << av_err2str(resp) << endl;
            cout << "couldn't decode packet " << resp << endl;
            return false;
        }
        resp = avcodec_receive_frame(av_codec_ctx, av_frame);
        if (resp == AVERROR(EAGAIN) || resp == AVERROR_EOF) {
            continue;
        } else if (resp < 0) {
            cout << "couldn't decode packet " << resp << endl;
            return false;
        }
        av_packet_unref(av_packet);
        break;
    }
    auto size = av_frame->width * av_frame->height * 4;
    uint8_t* data = new uint8_t[size];

    SwsContext* sws_scaler_ctx = sws_getContext(av_frame->width, av_frame->height, av_codec_ctx->pix_fmt,
                                                av_frame->width, av_frame->height, AV_PIX_FMT_RGB0,
                                                SWS_BILINEAR,
                                                NULL, NULL, NULL);

    if (!sws_scaler_ctx) {
        cout << "couldn't init swsscaler \n";
        return false;
    }

    // uint8_t* dest[4] = { data, NULL, NULL, NULL };
    // int dest_linesize[4] = { av_frame->width * 4, 0, 0, 0 };
    uint8_t* dest[4] = { data, NULL, NULL, NULL };
    int dest_linesize[4] = { av_frame->width * 4, 0, 0, 0 };
    sws_scale(sws_scaler_ctx, av_frame->data, av_frame->linesize, 0, av_frame->height, dest, dest_linesize);
    sws_freeContext(sws_scaler_ctx);

    *width_out = av_frame->width;
    *height_out = av_frame->height;
    *data_out = data;

	avformat_close_input(&av_format_ctx);
	avformat_free_context(av_format_ctx);
    av_frame_free(&av_frame);
    av_packet_free(&av_packet);
	avcodec_free_context(&av_codec_ctx);

	return true;
}