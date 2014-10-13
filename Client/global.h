#pragma once


#define UDP_BLOCK_SIZE 1200

enum
{
	PACK_TYPE_REQ = 0,
	PACK_TYPE_DATA = 1,
	PACK_TYPE_REQACK = 2,
	PACK_TYPE_DATAACK = 3,
	PACK_TYPE_FINISH = 4,
	PACK_TYPE_ACK_DATAACK = 5,
};

#define SEND_WINDOIW_SIZE 128
#define ONCE_MAX_ACK_NUM 8
#define MAX_NO_ACK_TIMEOUT 2000
#define MAX_NO_DATA_TIMEOUT 1000
#define MAX_PACK_SIZE 1024

#define MAX_VIDEOFRAME_SIZE 1024000


typedef void (*udp_data_cb)(int sockid, char *data, int len, int ip, int port, void* param);

class CPackIn
{
public:
	CPackIn(void);
	~CPackIn(void);
	const CPackIn& operator>>(int& nData);
	const CPackIn& GetBuffer(char*& pBuffer, int& nSizeOut);
	void SetContent(char* pBuffer, int nLen);

private:
	char* m_pBuf;
	int m_nReadPos;
};

class CPackOut
{
public:
	CPackOut(void);
	~CPackOut(void);
	const CPackOut& operator<<(int nData);
	const CPackOut& SetBuffer(char* pBuffer, int nSize);
	void GetContent(char*& pBuffer, int& nSizeOut);

private:
	int m_nWritePos;
	char m_szBuffer[1500];
};

#define SAFE_RETURN(x)	{HRESULT hr = (x); if(hr != S_OK) return hr;}
#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }
//#define SAFE_DELETE(x) { if (x) delete x; x = NULL; }


#if defined(__WIN32__) || defined(_WIN32)
// For Windoze, we need to implement our own gettimeofday()
extern int gettimeofday(struct timeval*, int*);
#endif