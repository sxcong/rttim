#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <map>
using namespace std;


class CUser
{
public:
	CUser(uint32_t id, uint32_t ip, uint32_t port);
	~CUser();

	uint32_t m_uid;
	uint32_t m_ip;
	uint32_t m_port;
};


class CUserMgr
{
public:
	CUserMgr(void);
	~CUserMgr(void);

	static CUserMgr& Instance();

	CUser* AddUser(uint32_t id, uint32_t ip, uint32_t port);
	CUser* FindUser(uint32_t uid);
	void RemoveUser(uint32_t uid);

private:
	map<uint32_t, CUser*> m_UserMap;
};
