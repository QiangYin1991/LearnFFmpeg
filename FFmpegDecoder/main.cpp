#include <iostream>

extern "C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}

int main()
{
	std::string path = "Titanic.ts";

	// init
	av_register_all();
	avformat_network_init();
	// AVFormatContext
	AVFormatContext *pFormatCtx = avformat_alloc_context();
	if (avformat_open_input(&pFormatCtx, path.c_str(), NULL, NULL) != 0)
	{
		std::cout << "open input stream failed." << std::endl;
		return -1;
	}
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		std::cout << "find stream information failed." << std::endl;
		return -1;
	}
	int videoIdx = -1;
	for (unsigned int i = 0; i < pFormatCtx->nb_streams; ++i)
	{
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoIdx = i;
			break;
		}
	}
	if (videoIdx == -1)
	{
		std::cout << "Could not find video stream." << std::endl;
		return -1;
	}
	AVCodecContext *pCodecCtx = pFormatCtx->streams[videoIdx]->codec;
	AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL)
	{
		std::cout << "Could not find a codec." << std::endl;
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		std::cout << "open codec failed." << std::endl;
		return -1;
	}

	//
	AVFrame *pFrame = av_frame_alloc();
	AVFrame *pFrameYUV = av_frame_alloc();
	uint8_t *out_buffer = (uint8_t*)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	avpicture_fill((AVPicture*)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
	AVPacket *pPacket = (AVPacket*)av_malloc(sizeof(AVPacket));
	std::cout << "------------------File Information------------------------" << std::endl;
	av_dump_format(pFormatCtx, 0, path.c_str(), 0);
	std::cout << "----------------------------------------------------------" << std::endl;

	return 0;
}