// VideoEnc.cpp: implementation of the CVideoEnc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VideoEnc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static int sws_flags = SWS_BICUBIC;
CVideoEnc::CVideoEnc()
{
	video_width = 320;
	video_height = 240;
	frame_rate = 20;
	bit_rate = 400000;//400K

	video_outbuf = NULL;
	encode_picture = NULL;
	tmp_picture = NULL;

	img_convert_ctx = NULL;

	avcodec_register_all();
    av_register_all();
	m_pParam = NULL;
	c = NULL;
}

CVideoEnc::~CVideoEnc()
{

}



int CVideoEnc::Enc_Init(UINT nWidth,UINT nHeight,UINT nFrame,UINT nBitrate,VEncodeCB fCB, void* param)
{
	m_fVEncodeCB = fCB;
	m_pParam = param;
	video_width = nWidth;
	video_height = nHeight;
	frame_rate = nFrame;
	bit_rate = nBitrate;

	
	video_outbuf_size = nWidth * nHeight * 3;


    encode_picture= av_frame_alloc();

    video_outbuf = (unsigned char *)av_malloc(video_outbuf_size);
	
    avpicture_fill((AVPicture *)encode_picture, video_outbuf,
		PIX_FMT_YUV420P, video_width, video_height);


    if (!encode_picture) 
	{
//        OutputDebugString( "8 Could not allocate picture\n");
        return -1;
    }

	tmp_picture = av_frame_alloc();


	codec = avcodec_find_encoder(CODEC_ID_H264);
    if (!codec)
	{
        fprintf(stderr, "codec not found\n");
        exit(1);
    }

    c= avcodec_alloc_context3(codec);

    c->bit_rate = bit_rate;
    c->width = video_width;
    c->height = video_height;
	c->time_base.den =25;
	c->time_base.num =1;
    //c->time_base= (AVRational){1,25};
    c->gop_size = 10; // emit one intra frame every ten frames 
    c->max_b_frames=1;
    c->pix_fmt = PIX_FMT_YUV420P;


    av_opt_set(c->priv_data, "preset", "slow", 0);
	av_opt_set(c->priv_data, "tune", "zerolatency", 0);//≤ª”√ª·ÕœŒ≤

     if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }


	m_bInit = true;
	
	 return 0;
}
void CVideoEnc::Enc_UnInit()
{
	m_bInit = false;

	if(encode_picture)
		av_free(encode_picture);
	
	if(tmp_picture)
		av_free(tmp_picture);
	
	if(video_outbuf)
		av_free(video_outbuf);
	
	if(c)
	{
		avcodec_close(c);
		av_free(c);
	}

}
int CVideoEnc::encode_frame(BYTE* input, int nLen)
{
	if(!m_bInit)
		return -1;

	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = NULL;    // packet data will be allocated by the encoder
	pkt.size = 0;

	int w = c->width;
	int h = c->height;

	AVFrame *pFrameRGB = av_frame_alloc();
	avpicture_fill((AVPicture *)pFrameRGB, input, PIX_FMT_RGB24,  w, h);

	img_convert_ctx = sws_getCachedContext(img_convert_ctx,
		w, h, PIX_FMT_RGB24 , w, h, PIX_FMT_YUV420P, sws_flags, NULL, NULL, NULL);
	if (img_convert_ctx == NULL)
	{
		fprintf(stderr, "Cannot initialize the conversion context\n");
		//exit(1);
		//av_free(buffer);
		av_free(pFrameRGB);
		return -4;
	}
	sws_scale(img_convert_ctx,  pFrameRGB->data, pFrameRGB->linesize,
		0, h, encode_picture->data, encode_picture->linesize);


	int got_output = 0;
	int   ret = avcodec_encode_video2(c, &pkt, encode_picture, &got_output);
	if (ret < 0) 
	{
		fprintf(stderr, "Error encoding frame\n");
		exit(1);
	}

	if (got_output)
	{
		if (m_fVEncodeCB)
		{
			//MP4ENC_Metadata mete;
			//PraseMetadata(pkt.data, pkt.size, mete);

			m_fVEncodeCB(pkt.data, pkt.size, m_pParam);
		}
		av_free_packet(&pkt);
	}
	av_free(pFrameRGB);

	return 0;
}



int PraseMetadata(const unsigned char* pData, int size, MP4ENC_Metadata &metadata)
{
    if(pData == NULL || size<4)
    {
        return -1;
    }
    MP4ENC_NaluUnit nalu;
    int pos = 0;
    bool bRet1 = false,bRet2 = false;
    while (int len = ReadOneNaluFromBuf(pData,size,pos,nalu))
    {
        if(nalu.type == 0x07)
        {
            memcpy(metadata.Sps,nalu.data,nalu.size);
            metadata.nSpsLen = nalu.size;
            bRet1 = true;

			OutputDebugString("sps \n");

            //h264_decode_sps(pData, size, m_nWidth, m_nHeight);
        }
        else if((nalu.type == 0x08))
        {
            memcpy(metadata.Pps,nalu.data,nalu.size);
            metadata.nPpsLen = nalu.size;
            bRet2 = true;
			OutputDebugString("pps \n");
        }
		 else if((nalu.type == 0x05))
		 {
			 OutputDebugString("key \n");
		}
        pos += len;
    }
    if (!bRet1)
        return -2;
    if (!bRet2)
        return -3;


    return 0;
}


int ReadOneNaluFromBuf(const unsigned char *buffer,unsigned int nBufferSize,unsigned int offSet,MP4ENC_NaluUnit &nalu)
{
    int i = offSet;
    while(i<nBufferSize)
    {
        if(buffer[i++] == 0x00 &&
            buffer[i++] == 0x00 &&
            buffer[i++] == 0x00 &&
            buffer[i++] == 0x01
            )
        {
            int pos = i;
            while (pos<nBufferSize)
            {
                if(buffer[pos++] == 0x00 &&
                    buffer[pos++] == 0x00 &&
                    buffer[pos++] == 0x00 &&
                    buffer[pos++] == 0x01
                    )
                {
                    break;
                }
            }
            if(pos == nBufferSize)
            {
                nalu.size = pos-i;
            }
            else
            {
                nalu.size = (pos-4)-i;
            }

            nalu.type = buffer[i]&0x1f;
            nalu.data =(unsigned char*)&buffer[i];
            return (nalu.size+i-offSet);
        }
    }
    return 0;
}