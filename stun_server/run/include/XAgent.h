#pragma once

#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <arpa/inet.h>
#include <netdb.h>


#include <vector>
#include <string>
using namespace std;


int get_key_value(char* input, char* key, char* output);

char* get_item_value_from_file(char* file, char* key, char* value);
class CXAgent
{
public:
	CXAgent(void);
	~CXAgent(void);
	int m_nPort;

	int Run(char *name);

private:
	pthread_t tid;
	

private:
	//bool m_bLive;
	
public:
	string m_progName;

public:
	static CXAgent& Instance();
	void init();
	//void uninit();	
};
