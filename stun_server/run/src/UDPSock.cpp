// RTPSock.cpp: implementation of the CRTPSock class.
//
//////////////////////////////////////////////////////////////////////

#include "UDPSock.h"
#include <string.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUDPSock::CUDPSock()
{
	m_handle = -1;
}

CUDPSock::~CUDPSock()
{

}

int CUDPSock::Open(int nPort)
{
	if(m_handle != -1)
	{
		Close();
	}

	m_handle =  socket(AF_INET, SOCK_DGRAM, 0) ;

	if (m_handle == -1)
	{
		return -1;
	}

	int one = 1;
	int nSet = setsockopt(m_handle, SOL_SOCKET, SO_REUSEADDR, (const char *)&one, sizeof(int));
	sockaddr_in	addr;

	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_family = AF_INET;					
	addr.sin_port = htons (nPort);	
	addr.sin_addr.s_addr = htonl( INADDR_ANY );
	int iResult = bind(m_handle, (struct sockaddr*)&addr, sizeof(addr));

	if( iResult == -1)
	{
		//int err=GetLastError();
		Close();
		return -1;
	}
	return m_handle;
}

int CUDPSock::Close()
{
	int iResult = 0;
	if ( m_handle != -1)
	{
#ifndef WIN32
	close(m_handle);
#else
	 closesocket(m_handle);
#endif
		
	m_handle = -1;
	}
	return 0;
}

int CUDPSock::Send( const char* pBuf, size_t nLen, uint32_t ip, uint16_t port ) const
{
	if (m_handle == -1)
		return 0;

	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));

	addr.sin_port = htons((u_short)port);
	addr.sin_addr.s_addr = ip;
	addr.sin_family = AF_INET;

	int nSendLen = sendto(m_handle, pBuf, nLen, 0, (sockaddr*)&addr, sizeof(sockaddr_in));

	if (nSendLen < 0)
	{
	//	int nErr = GetLastError();
	//	char log[1024] = {'\0'};
        //sprintf(log, "send error = %d \n", nErr);
        //OutputDebugString(log);
	}
	
	return nSendLen;
}


int CUDPSock::Receive(char* pBuf, size_t nLen, uint32_t& ip, uint16_t& port) const
{
	if (m_handle == -1)
		return 0;

	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	int iReceived = recvfrom(m_handle, pBuf, nLen, 0, (sockaddr*)&sockAddr, (socklen_t*)&nSockAddrLen);
	port= ntohs(sockAddr.sin_port);
	ip = sockAddr.sin_addr.s_addr;

	return iReceived;
}
