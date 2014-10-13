
#include "stdafx.h"
#include "UDPSession.h"
#include <vector>
using namespace std;


CUDPSession::CUDPSession()
{
	m_bEventLoopStarted	= false;
	m_pCB = NULL;
	m_localUid = 0;
	m_destUid = 0;	
	m_buffer = NULL;

	m_nFrameIndex = 0;
	m_nRecvFrameIndex = 0;
}

CUDPSession::~CUDPSession()
{
	if (m_bEventLoopStarted)
		Close();

	if (m_buffer)
	{
		evbuffer_free(m_buffer);
	}
}

int CUDPSession::Open(uint16_t usLocalPort, udp_data_cb cb, void* param)
{
	m_pCB = cb;
	m_pParam = param;

	if (m_bEventLoopStarted)
        return 0;

	int iResult = m_udpIO.Open(usLocalPort);
	if (iResult == -1)
		return -2;


	//m_nFrameIndex = 0;
	//m_pReassemble = new CUdpReassemble();
	//m_pReassemble->SetCallBack(cb, param);

	int r = pthread_create (&m_tid, NULL, EventLoop, this);
	if (r)
	{
		//perror ("pthread_create()");
		return -1;
	}
	return iResult;	
}

int CUDPSession::Close()
{
	if (m_bEventLoopStarted)
	{
		m_bEventLoopStarted = false;
		pthread_join(m_tid, NULL);
		m_udpIO.Close();
	}
	return 0;
}


void* CUDPSession::EventLoop(void* lpParameter )
{
	CUDPSession* pThis = (CUDPSession*)lpParameter;
	pThis->ProcessEvent();
	return 0;
}

void CUDPSession::ProcessEvent()
{
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000; //500ms

	fd_set fsRead;
	char szBuffer[RUDP_RECV_BUFFER_SIZE];
	int iReceived = 0;
	int iErrorCode = 0;


	m_bEventLoopStarted	= true;
	bool bLastSend = false;
	while (m_bEventLoopStarted)
	{
		FD_ZERO( &fsRead );
		FD_SET(m_udpIO.m_handle, &fsRead);
		int ret = select (1024, &fsRead, NULL, NULL, &timeout);
		if (ret < 0)
		{
		}
		else if (ret == 0)
		{
			//OnTimer();
		}
		else 
		{
			if(FD_ISSET( m_udpIO.m_handle, &fsRead))
			{
				
				uint32_t ip;
				uint16_t nPort;
				iReceived = m_udpIO.Receive(szBuffer, sizeof ( szBuffer ), ip,nPort);
				if (iReceived > 0)
				{
					OnRecv(szBuffer, iReceived, ip, nPort);
				}
				else
				{
				
				}
			}
		}
		timeout.tv_sec = 0;
		timeout.tv_usec = 500000;
		timeout.tv_usec = 100000;		
	}
}

int CUDPSession::OnRecv(char* pBuff, uint32_t nLen, uint32_t ip,uint16_t port)
{
	CPackIn pack;
	pack.SetContent(pBuff, nLen);
	int nType;
	pack >> nType;
	
	switch(nType)
	{
	case PACK_TYPE_DATA:
		{
			Reassemble(pack, ip, port);
		}
		break;

	}	
	return 0;
}

int CUDPSession::SendPacket(CPackOut* pPack, uint32_t ip, uint32_t port)
{
	if (pPack)
	{
		char* pBuf = NULL;
		int nSize;
		pPack->GetContent(pBuf, nSize);
		return m_udpIO.Send(pBuf, nSize, ip, port);
	}
	return -1;
}

int CUDPSession::SendRaw(char* pBuff, uint32_t nLen, uint32_t ip, uint16_t port)
{
	  return m_udpIO.Send(pBuff, nLen, ip, port);
}

int CUDPSession::SplitData(char* pBuff, uint32_t nLen)
{
	int nBlockNum = nLen / UDP_BLOCK_SIZE;
	if (nLen % UDP_BLOCK_SIZE != 0)
	{
		nBlockNum++;
	}

	int sendlen = 0;
	for (int i = 0; i < nBlockNum; i++)
	{
		int poayload_size = UDP_BLOCK_SIZE;
		char* payload = pBuff + UDP_BLOCK_SIZE * (i);

		if (nLen - UDP_BLOCK_SIZE * (i) < UDP_BLOCK_SIZE)
			poayload_size = nLen - UDP_BLOCK_SIZE * (i);

		CPackOut* pack = new CPackOut;
		(*pack) << PACK_TYPE_DATA;
		(*pack) << m_nFrameIndex;
		if (i == nBlockNum - 1)
		{
			(*pack) << 1;
		}
		else
		{
			(*pack) << 0;
		}
		(*pack) << poayload_size;
		(*pack).SetBuffer(payload, poayload_size);


		int ret = SendPacket(pack, m_destIP, m_destPort);
		printf("SendPacket ret = %d\n", ret);
		sendlen += ret;
		delete pack;
		pack = NULL;
	}
	
	return sendlen;
}


int CUDPSession::Reassemble(CPackIn& pack, uint32_t ip, uint32_t port)
{
	int nSeq = 0;
	int nMark = 0;
	int nLen = 0;

	pack >> nSeq;
	pack >> nMark;
	pack >> nLen;

	if (m_nRecvFrameIndex != nSeq)
	{
		if (m_buffer)
		{
			evbuffer_free(m_buffer);
			m_buffer = NULL;
		}
		m_nRecvFrameIndex = nSeq;
	}

	if (m_buffer == NULL)
	{
		m_buffer = evbuffer_new();
	}
	

	char* pBuf = 0;
	int   nSize;
	pack.GetBuffer(pBuf, nSize);

	evbuffer_add(m_buffer, pBuf, nSize);

	if (nMark == 1)
	{
		//回调
		if (m_pCB)
		{
			m_pCB(m_udpIO.m_handle, (char*)(m_buffer->buffer), m_buffer->off, ip, port, m_pParam);
		}
		evbuffer_free(m_buffer);
		m_buffer = NULL;
	}

	return 0;
}


int CUDPSession::Send(char* pBuff, uint32_t nLen, uint32_t ip,uint16_t port)
{
	m_destIP = ip;
	m_destPort = port;
	m_nFrameIndex++;
	return SplitData(pBuff, nLen);
}

void CUDPSession::OnTimer()
{

}

