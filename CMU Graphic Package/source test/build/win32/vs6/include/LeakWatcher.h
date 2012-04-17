#ifndef IMWATCHINGYOULEAK
#define IMWATCHINGYOULEAK

#include <crtdbg.h>

#ifdef _DEBUG
void* operator new(size_t nSize, const char * lpszFileName, int nLine);
#define DEBUG_NEW new(THIS_FILE, __LINE__)

void __cdecl operator delete(void * _P, const char * lpszFileName, int nLine);

#define MALLOC_DBG(x) _malloc_dbg(x, 1, THIS_FILE, __LINE__);
#define malloc(x) MALLOC_DBG(x)

#endif // _DEBUG

#endif // #include guard
