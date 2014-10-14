
#include "global.h"
#include "UserMgr.h"
#include <string>
using namespace std;

#include <algorithm>
class CFODeleteMap
{
public:
    template<class T>
    void operator() ( T& item )
    {
    delete item.second;
    item.second = NULL;
    }
};

class CFODelete
{
public:
    template<class T>
    void operator() ( T* pItem )
    {
    delete pItem;
    pItem = NULL;
    }
};

string ip_int2char(uint32_t ip)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_addr.s_addr = ip;
	addr.sin_family = AF_INET;
	string temp = inet_ntoa(addr.sin_addr);
	return temp;
}

CUser::CUser(uint32_t id, uint32_t ip, uint32_t port)
{
	m_uid = id;
	m_ip = ip;
	m_port = port;

	string ip1 = ip_int2char(ip);

	printf("ip:port = %s:%d \n", ip1.c_str(), port);
	
	//time(&m_lastTime);
}

CUser::~CUser()
{
}

///////////
//user manager
CUserMgr::CUserMgr(void)
{
	
}

CUserMgr::~CUserMgr(void)
{
	for_each(m_UserMap.begin(), m_UserMap.end(), CFODeleteMap());
}

CUserMgr& CUserMgr::Instance()
{
	static CUserMgr agent;
	return agent;
}

CUser* CUserMgr::AddUser(uint32_t id, uint32_t ip, uint32_t port)
{
	CUser* pUser = NULL;
	map<uint32_t, CUser*>::iterator it = m_UserMap.find(id);
	if (it != m_UserMap.end())
	{
		pUser = it->second;
		pUser->m_ip = ip;
		pUser->m_port = port;
	}
	else
	{
		 pUser = new CUser(id, ip, port);
		 m_UserMap[id] = pUser;
	}
	return pUser;
}

CUser* CUserMgr::FindUser(uint32_t uid)
{
	map<uint32_t, CUser*>::iterator it = m_UserMap.find(uid);
	if (it != m_UserMap.end())
		return it->second;
	return NULL;
}

void CUserMgr::RemoveUser(uint32_t uid)
{
	map<uint32_t, CUser*>::iterator it = m_UserMap.find(uid);
	if (it != m_UserMap.end())
	{
		delete it->second;
		it->second = NULL;
		m_UserMap.erase(it);
	}
}