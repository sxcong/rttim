// VideoEnc.h: interface for the CVideoEnc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOENC_H__2A48208D_E3DF_4D13_80A2_BB771FBC6EC8__INCLUDED_)
#define AFX_VIDEOENC_H__2A48208D_E3DF_4D13_80A2_BB771FBC6EC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


typedef struct _MP4ENC_NaluUnit
{
    int type;
    int size;
    unsigned char *data;
}MP4ENC_NaluUnit;

typedef struct _MP4ENC_Metadata
{
    // video, must be h264 type
    unsigned int	nSpsLen;
    unsigned char	Sps[1024];
    unsigned int	nPpsLen;
    unsigned char	Pps[1024];

} MP4ENC_Metadata,*LPMP4ENC_Metadata;

int PraseMetadata(const unsigned char* pData, int size,MP4ENC_Metadata &metadata);
int ReadOneNaluFromBuf(const unsigned char *buffer,unsigned int nBufferSize,unsigned int offSet,MP4ENC_NaluUnit &nalu);


typedef void (*VEncodeCB)(BYTE* pData, UINT nLen, void* lpParam);
class CVideoEnc  
{
public:
	CVideoEnc();
	virtual ~CVideoEnc();

	int Enc_Init(UINT nWidth,UINT nHeight,UINT nFrame,UINT nBitrate,VEncodeCB fCB, void* param);
	void Enc_UnInit();
	int encode_frame(BYTE* input,int nLen);


private:
	bool m_bInit;


	AVFrame *encode_picture;
	AVFrame *tmp_picture;

	UINT video_width;
	UINT video_height;
	UINT frame_rate;
	UINT bit_rate;

	UINT video_outbuf_size ;
	uint8_t *video_outbuf;

	VEncodeCB m_fVEncodeCB;

	AVCodec *codec;
    AVCodecContext *c;
	struct SwsContext *img_convert_ctx;
	void* m_pParam;
};

#endif // !defined(AFX_VIDEOENC_H__2A48208D_E3DF_4D13_80A2_BB771FBC6EC8__INCLUDED_)
