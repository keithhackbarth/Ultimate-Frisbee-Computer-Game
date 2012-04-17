#include "LeakWatcher.h"

#ifdef _DEBUG
void* operator new(size_t nSize, const char * lpszFileName, int nLine)
{
    return ::operator new(nSize, 1, lpszFileName, nLine);
}

void __cdecl operator delete(void * _P, const char * lpszFileName, int nLine)
{
::operator delete(_P, lpszFileName, nLine);
}

#endif // _DEBUG

