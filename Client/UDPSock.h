// RTPSock.h: interface for the CRTPSock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RTPSOCK_H__1190002F_8701_406A_8BEF_FD4053D18C20__INCLUDED_)
#define AFX_RTPSOCK_H__1190002F_8701_406A_8BEF_FD4053D18C20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




#define RUDP_RECV_BUFFER_SIZE  1500

class CUDPSock  
{
public:
	CUDPSock();
	virtual ~CUDPSock();

	int Open(int nPort);

	int Close();

	int Send( const char* pBuf, size_t nLen, uint32_t ip, uint16_t port ) const;
	
	int Receive(char* pBuf, size_t nLen, uint32_t& ip, uint16_t& port) const;

	
	int m_handle;

};

#endif // !defined(AFX_RTPSOCK_H__1190002F_8701_406A_8BEF_FD4053D18C20__INCLUDED_)
