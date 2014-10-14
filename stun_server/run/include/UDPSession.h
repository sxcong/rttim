// RTPSession.h: interface for the RTPSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RTPSESSION_H__E0363A1C_703C_456A_9E3A_5FED274429BF__INCLUDED_)
#define AFX_RTPSESSION_H__E0363A1C_703C_456A_9E3A_5FED274429BF__INCLUDED_



#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <map>
#include <vector>
using namespace std;

#include "UDPSock.h"
#include "global.h"

#include "evbuffer.h"

class CUDPSession  
{
public:	
	CUDPSession();
	virtual ~CUDPSession();
	int Open(uint16_t usLocalPort, udp_data_cb cb, void* param);
	int Close();

	int SendRaw(char* pBuff, uint32_t nLen, uint32_t ip, uint16_t port);
	int Send(char* pBuff,uint32_t nLen,uint32_t ip,uint16_t port);

	uint32_t m_localUid;
	uint32_t m_destUid;
	uint32_t m_destIP;
	uint32_t m_destPort;
	uint32_t m_timestamp;
	
	udp_data_cb m_pCB;
	void* m_pParam;

	timeout_cb m_pTimeOutCB;
	
	struct evbuffer *m_buffer;;
	

private:
	static void*  EventLoop(void* lpParameter );
	void ProcessEvent();

	int Reassemble(CPackIn& pack, uint32_t ip, uint32_t port);
	int OnRecv(char* pBuff,uint32_t nLen, uint32_t ip,uint16_t port);
	int SendPacket(CPackOut* pPack, uint32_t ip, uint32_t port);
	void OnTimer();


	bool         m_bEventLoopStarted;
	CUDPSock     m_udpIO;
	pthread_t    m_tid;

	uint32_t m_nFrameIndex;
	uint32_t m_nRecvFrameIndex;


	int SplitData(char* pBuff, uint32_t nLen);
};

#endif // !defined(AFX_RTPSESSION_H__E0363A1C_703C_456A_9E3A_5FED274429BF__INCLUDED_)
