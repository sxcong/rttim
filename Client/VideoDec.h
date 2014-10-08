// VideoDec.h: interface for the CVideoDec class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEODEC_H__886325CC_EA9A_40EC_A0FE_6C68048052C1__INCLUDED_)
#define AFX_VIDEODEC_H__886325CC_EA9A_40EC_A0FE_6C68048052C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef void (*VDecodeCB)(BYTE* pData, UINT nLen, void* lpParam);

class CVideoDec  
{
public:
	CVideoDec();
	virtual ~CVideoDec();
	int  Dec_Init(UINT nWidth,UINT nHeight,UINT nFrame,UINT nBitrate,VDecodeCB CB, void* param);
	void Dec_UnInit();

	int decode_frame(BYTE* input,int nLen);

private:
	AVCodec *decode_codec;
	AVCodecContext *decode_c;
	AVFrame *decode_picture;

	struct SwsContext *img_convert_ctx;

	UINT video_width;
	UINT video_height;
	UINT frame_rate;
	UINT bit_rate;

	bool m_bInit;

	VDecodeCB m_fVDecodeCB;
	void* m_pParam;
};

#endif // !defined(AFX_VIDEODEC_H__886325CC_EA9A_40EC_A0FE_6C68048052C1__INCLUDED_)
