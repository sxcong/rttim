#include <stdio.h>
#include <iostream>
#include <string>
#include "XAgent.h"


int main(int argc, char** argv)
{
	CXAgent::Instance().Run(argv[0]);
	return 0;	
}
