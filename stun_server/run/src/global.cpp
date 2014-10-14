
#include <stdio.h>
#include <string.h>
#include "global.h"

CPackIn::CPackIn(void)
{
	m_nReadPos = 0;
	m_pBuf = NULL;
}

CPackIn::~CPackIn(void)
{
}

const CPackIn& CPackIn::operator>>(int& nData)
{
	nData = *(int*)(m_pBuf + m_nReadPos);
	m_nReadPos = m_nReadPos + sizeof(int);

	return *this;
}

const CPackIn& CPackIn::GetBuffer(char*& pBuffer, int& nSizeOut)
{
	nSizeOut = *(int*)(m_pBuf + m_nReadPos);
	m_nReadPos = m_nReadPos + sizeof(int);
	pBuffer = m_pBuf + m_nReadPos;

	return *this;
}

void CPackIn::SetContent(char* pBuffer, int nLen)
{
	m_pBuf = pBuffer;
}

CPackOut::CPackOut(void)
{
	m_nWritePos = 0;
	memset(m_szBuffer, 0, sizeof(m_szBuffer));
}

CPackOut::~CPackOut(void)
{
}

const CPackOut& CPackOut::operator<<(int nData)
{
	*(int*)(m_szBuffer + m_nWritePos) = nData;
	m_nWritePos = m_nWritePos + sizeof(int);

	return *this;
}


const CPackOut& CPackOut::SetBuffer(char* pBuffer, int nSize)
{
	*(int*)(m_szBuffer + m_nWritePos) = nSize;
	m_nWritePos = m_nWritePos + sizeof(int);
	memcpy(m_szBuffer + m_nWritePos, pBuffer, nSize);
	m_nWritePos = m_nWritePos + nSize;

	return *this;
}

void CPackOut::GetContent(char*& pBuffer, int& nSizeOut)
{
	pBuffer = m_szBuffer;
	nSizeOut = m_nWritePos;
}




#if defined(__WIN32__) || defined(_WIN32)
// For Windoze, we need to implement our own gettimeofday()

// used to make sure that static variables in gettimeofday() aren't initialized simultaneously by multiple threads
static LONG initializeLock_gettimeofday = 0;  

#if !defined(_WIN32_WCE)
#include <sys/timeb.h>
#endif

int gettimeofday(struct timeval* tp, int* /*tz*/) {
  static LARGE_INTEGER tickFrequency, epochOffset;

  static BOOL isInitialized = FALSE;

  LARGE_INTEGER tickNow;

#if !defined(_WIN32_WCE)
  QueryPerformanceCounter(&tickNow);
#else
  tickNow.QuadPart = GetTickCount();
#endif
 
  if (!isInitialized) {
    if(1 == InterlockedIncrement(&initializeLock_gettimeofday)) {
#if !defined(_WIN32_WCE)
      // For our first call, use "ftime()", so that we get a time with a proper epoch.
      // For subsequent calls, use "QueryPerformanceCount()", because it's more fine-grain.
      struct timeb tb;
      ftime(&tb);
      tp->tv_sec = tb.time;
      tp->tv_usec = 1000*tb.millitm;

      // Also get our counter frequency:
      QueryPerformanceFrequency(&tickFrequency);
#else
      /* FILETIME of Jan 1 1970 00:00:00. */
      const LONGLONG epoch = 116444736000000000LL;
      FILETIME fileTime;
      LARGE_INTEGER time;
      GetSystemTimeAsFileTime(&fileTime);

      time.HighPart = fileTime.dwHighDateTime;
      time.LowPart = fileTime.dwLowDateTime;

      // convert to from 100ns time to unix timestamp in seconds, 1000*1000*10
      tp->tv_sec = (long)((time.QuadPart - epoch) / 10000000L);

      /*
        GetSystemTimeAsFileTime has just a seconds resolution,
        thats why wince-version of gettimeofday is not 100% accurate, usec accuracy would be calculated like this:
        // convert 100 nanoseconds to usec
        tp->tv_usec= (long)((time.QuadPart - epoch)%10000000L) / 10L;
      */
      tp->tv_usec = 0;

      // resolution of GetTickCounter() is always milliseconds
      tickFrequency.QuadPart = 1000;
#endif     
      // compute an offset to add to subsequent counter times, so we get a proper epoch:
      epochOffset.QuadPart
          = tp->tv_sec * tickFrequency.QuadPart + (tp->tv_usec * tickFrequency.QuadPart) / 1000000L - tickNow.QuadPart;
      
      // next caller can use ticks for time calculation
      isInitialized = TRUE; 
      return 0;
    } else {
        InterlockedDecrement(&initializeLock_gettimeofday);
        // wait until first caller has initialized static values
        while(!isInitialized){
          Sleep(1);
        }
    }
  }

  // adjust our tick count so that we get a proper epoch:
  tickNow.QuadPart += epochOffset.QuadPart;

  tp->tv_sec =  (long)(tickNow.QuadPart / tickFrequency.QuadPart);
  tp->tv_usec = (long)(((tickNow.QuadPart % tickFrequency.QuadPart) * 1000000L) / tickFrequency.QuadPart);

  return 0;
}
#endif

