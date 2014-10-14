
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <arpa/inet.h>
#include <netdb.h>


#include "XAgent.h"
#include "global.h"
#include "UserMgr.h"
#include "UDPSession.h"

enum
{
	CMD_GET_SELF_PUBLIC_IP_PORT = 1001,
	CMD_RE_GET_SELF_PUBLIC_IP_PORT = 1002,
	CMD_GET_DEST_PUBLIC_IP_PORT = 1003,
	CMD_RE_GET_DEST_PUBLIC_IP_PORT = 1004
};

CUDPSession g_Sess;
bool g_bQuit = false;

static void sigterm_handler(int sig)
{
    g_bQuit = true;
}

static void OnRecvFrom(int sockid, char *data, int len, int ip, int port, void* param)
{
	CPackIn pack;
	pack.SetContent(data, len);
	int nCmd;
	pack >> nCmd;
	
	switch(nCmd)
	{
	case CMD_GET_SELF_PUBLIC_IP_PORT:
		{
			if (len < sizeof(int)*2)
				return;
			int nID = 0;
			pack >> nID;
			if (nID < 1)
				return;

			CUserMgr::Instance().AddUser(nID, ip, port);

			CPackOut* pack = new CPackOut;
			(*pack) << CMD_RE_GET_SELF_PUBLIC_IP_PORT;
			(*pack) << nID;
			(*pack) << 0;  //0 is success
			(*pack) << ip;
			(*pack) << port;
		

			char* pBuf = NULL;
			int nSize;
			pack->GetContent(pBuf, nSize);
			g_Sess.Send(pBuf, nSize, ip, port);

			delete pack;
			pack = NULL;
		}
		break;

	case CMD_GET_DEST_PUBLIC_IP_PORT:
		{
			if (len < sizeof(int)*2)
				return;
			int nDestID = 0;
			pack >> nDestID;
			if (nDestID < 1)
				return;

			CPackOut* pack = new CPackOut;
			(*pack) << CMD_RE_GET_DEST_PUBLIC_IP_PORT;
			(*pack) << nDestID;
		

			CUser* pUser = CUserMgr::Instance().FindUser(nDestID);
			if (pUser)
			{
				(*pack) << 0;  //0 is success
				(*pack) << ip;
				(*pack) << port;
			}
			else
			{
				(*pack) << 404;//err 
			}

			char* pBuf = NULL;
			int nSize;
			pack->GetContent(pBuf, nSize);
			g_Sess.Send(pBuf, nSize, ip, port);

			delete pack;
			pack = NULL;
		}
		break;

	}	
}

static void OnTimeOut(int sockid)
{
}



CXAgent::CXAgent(void)
{
}

CXAgent::~CXAgent(void)
{
}

CXAgent& CXAgent::Instance()
{
	static CXAgent _agent;
	return _agent;
}

int CXAgent::Run(char *name)
{	 
    signal(SIGKILL,  sigterm_handler); 
    signal(SIGHUP,  sigterm_handler); 
    signal(SIGINT , sigterm_handler); 
    signal(SIGTERM, sigterm_handler); 

	if (g_Sess.Open(8800, OnRecvFrom, NULL) < 0)
	{
		printf("open error\n");
		return -1;
	}
	g_Sess.m_pTimeOutCB = OnTimeOut;

	while (!g_bQuit)
	{
		sleep(1);
	}
	g_Sess.Close();
	return 0;
}

