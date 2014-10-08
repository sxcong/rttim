// VideoDec.cpp: implementation of the CVideoDec class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VideoDec.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static int sws_flags = SWS_BICUBIC;
CVideoDec::CVideoDec()
{
	video_width = 320;
	video_height = 240;
	frame_rate = 20;
	bit_rate = 400000;//400K
	m_fVDecodeCB = NULL;

	//²Î¿¼:ffmpeg.c main()
	avcodec_register_all();
    av_register_all();
	m_bInit = false;
	img_convert_ctx = NULL;
	m_pParam = NULL;

//	avcodec_init();
}

CVideoDec::~CVideoDec()
{

}
int CVideoDec::Dec_Init(UINT nWidth,UINT nHeight,UINT nFrame,UINT nBitrate,VDecodeCB CB, void* param)
{
	m_fVDecodeCB = CB;
	m_pParam = param;

	video_width = nWidth;
	video_height = nHeight;
	frame_rate = nFrame;
	bit_rate = nBitrate;

	decode_codec = avcodec_find_decoder(CODEC_ID_H264);
    if (!decode_codec) 
	{
        fprintf(stderr, "codec not found\n");
        return -1;
    }

	decode_picture= av_frame_alloc();

	decode_c= NULL;
	decode_c= avcodec_alloc_context3(decode_codec);
	decode_c->codec_id= CODEC_ID_H264;
    decode_c->codec_type = AVMEDIA_TYPE_VIDEO;

    decode_c->pix_fmt = PIX_FMT_YUV420P;

    

	 if (avcodec_open2(decode_c, decode_codec, NULL) < 0) 
	 {
        fprintf(stderr, "could not open codec\n");
       return -1;
    }

	 m_bInit = true;
	 return 0;
}
void CVideoDec::Dec_UnInit()
{
	if(!m_bInit)
		return;

	m_bInit = false;
	if(decode_c)
	{
		avcodec_close(decode_c);
		av_free(decode_c);
	}
	if(decode_picture)
		av_free(decode_picture);
}
int CVideoDec::decode_frame(BYTE* input,int nLen)
{
	  if(!m_bInit)
        return -1;

    if(input == NULL || nLen <= 0)
        return -2;

	int got_picture;
	int size = nLen;


	AVPacket avpkt;
	av_init_packet(&avpkt);
	avpkt.size = size;
	avpkt.data = input;

	//while (avpkt.size > 0)
	{

		int len = avcodec_decode_video2(decode_c, decode_picture, &got_picture, &avpkt);

		if(len == -1)
		{
			return -3;
		}

		if (got_picture)
		{
			int w = decode_c->width;
			int h = decode_c->height;
			int numBytes=avpicture_get_size(PIX_FMT_RGB24, w,h);
			uint8_t * buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

			AVFrame *pFrameRGB = av_frame_alloc();
			avpicture_fill((AVPicture *)pFrameRGB, buffer,PIX_FMT_RGB24,  w, h);

			img_convert_ctx = sws_getCachedContext(img_convert_ctx,
				w, h, (PixelFormat)(decode_picture->format), w, h,PIX_FMT_RGB24, sws_flags, NULL, NULL, NULL);
			if (img_convert_ctx == NULL)
			{
				fprintf(stderr, "Cannot initialize the conversion context\n");
				//exit(1);
				av_free(buffer);
				av_free(pFrameRGB);
				return -4;
			}
			sws_scale(img_convert_ctx, decode_picture->data, decode_picture->linesize,
				0, h, pFrameRGB->data, pFrameRGB->linesize);

			 if (m_fVDecodeCB)
			 {
			    m_fVDecodeCB(pFrameRGB->data[0], numBytes*sizeof(uint8_t), m_pParam);
			 }

			av_free(buffer);
			av_free(pFrameRGB);
			return 0;
		}
		else
		{
			return -5;
		}
		//return 0;
	}

	//return 0;
    return -6;
}