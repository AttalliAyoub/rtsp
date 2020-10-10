#include "video_reader.hpp"

bool video_reader_open(VideoReaderState* state, const char* filePath) {

    auto& width = state->width;
    auto& height = state->height;
    auto& av_format_ctx = state->av_format_ctx;
    auto& av_codec_ctx = state->av_codec_ctx;
    auto& av_frame = state->av_frame;
    auto& av_packet = state->av_packet;
    auto& sws_scaler_ctx = state->sws_scaler_ctx;
    auto& video_stream_index = state->video_stream_index;

    av_format_ctx = avformat_alloc_context();
	if (!av_format_ctx) {
		cout << "couldn't allocat avformat\n";
		return false;
	}
	if (avformat_open_input(&av_format_ctx, filePath, NULL, NULL) != 0) {
		cout << "couldn't open file\n";
		return false;
	}

	video_stream_index = -1;

	AVCodecParameters* codec_parms;
	AVCodec* av_codec;
	for (int i = 0; i < av_format_ctx->nb_streams; ++i) {
		codec_parms = av_format_ctx->streams[i]->codecpar;
		av_codec = avcodec_find_decoder(codec_parms->codec_id);
		if (!av_codec) {
			continue;
		}
		if (av_codec->type == AVMEDIA_TYPE_VIDEO) {
			video_stream_index = i;
            // width = stream->
            width = codec_parms->width;
            height = codec_parms->height;
			break;
		}
	}

	if (video_stream_index == -1) {
		cout << "couldn't find video \n";
		return false;
	}

	av_codec_ctx = avcodec_alloc_context3(av_codec);
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

    av_frame = av_frame_alloc();
    if (!av_frame) {
        cout << "couldn't allocate frame\n";
        return false;
    }
    av_packet = av_packet_alloc();
    if (!av_packet) {
        cout << "couldn't allocate packet\n";
        return false;
    }

    state->sws_scaler_ctx_state = false;

    return true;
}

bool video_reader_read_frame(VideoReaderState* state, uint8_t* frame_buffer) {

    auto& width = state->width;
    auto& height = state->height;
    auto& av_format_ctx = state->av_format_ctx;
    auto& av_codec_ctx = state->av_codec_ctx;
    auto& av_frame = state->av_frame;
    auto& av_packet = state->av_packet;
    auto& sws_scaler_ctx = state->sws_scaler_ctx;
    auto& video_stream_index = state->video_stream_index;

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
    if (!state->sws_scaler_ctx_state) {
        sws_scaler_ctx = sws_getContext(width, height, av_codec_ctx->pix_fmt,
                                        width, height, AV_PIX_FMT_RGB0,
                                        SWS_BILINEAR,
                                        NULL, NULL, NULL);
        if (!sws_scaler_ctx) {
            cout << "couldn't init swsscaler \n";
            return false;
        }
        state->sws_scaler_ctx_state = true;
    }

    uint8_t* dest[4] = { frame_buffer, NULL, NULL, NULL };
    int dest_linesize[4] = { width * 4, 0, 0, 0 };
    sws_scale(sws_scaler_ctx, av_frame->data, av_frame->linesize, 0, av_frame->height, dest, dest_linesize);

	return true;
}

void video_reader_close(VideoReaderState* state) {
    sws_freeContext(state->sws_scaler_ctx);
	avformat_close_input(&state->av_format_ctx);
	avformat_free_context(state->av_format_ctx);
    av_frame_free(&state->av_frame);
    av_packet_free(&state->av_packet);
	avcodec_free_context(&state->av_codec_ctx);
}