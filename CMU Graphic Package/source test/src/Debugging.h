/*
See "CMUgraphics.h" for library version info and copyright information
$Id: Debugging.h,v 1.4 2003/01/06 03:53:08 jsaks Exp $
*/

#ifndef __DEBUGGING_H__
#define __DEBUGGING_H__

#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include "version.h"

using namespace std;
/*
#ifdef _MSC_VER

#include "LeakWatcher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#endif
*/
#define DEBUGGING 


////// Fatal assertion function //////

#ifdef DEBUGGING
inline void fatal_assert(bool result, const char *message, const char* file, long line) {
  if(result) {
    cerr << "FATAL ASSERTION in " << file << " @ line " << line << " : " 
         << message << ".  Program execution will now halt." << endl;
    abort();
  }
}
#else
#define fatal_assert(w, x, y, z) // (NULL)
#endif //DEBUGGING

////// Non-Fatal assertion function //////

#ifdef DEBUGGING
inline void nonfatal_assert(bool result, const char *message, const char *file, long line) {
  if(result) {
  cerr << "NON-FATAL ASSERTION in " << file << " @ line " << line << " : " 
       << message << ".  Program execution will continue, but may " 
       << "not behave as anticipated." << endl;
  }
}
#else
#define nonfatal_assert(w,x,y,z) // (NULL)
#endif

////// Fatal exit function //////

inline void fatal_exit(const char *message, const char *file, long line) {
  cerr << "FATAL ERROR in " << file << " @ line " << line << " : " 
       <<  message << ".  Program excecution will now halt." << endl;
  abort();
}

////// warning function //////

inline void warning(const char *message, const char *file, long line) {
  cerr << "WARNING in " << file << " @ line " << line << " : " 
       << message << ".  Program execution will continue, but may " 
       << "not behave as anticipated." << endl;
}


/*
#ifdef DEBUGGING

#define CMUGFX_LOGFILE "cmugfxlog.txt"

void log_message(const char *message, const char *file="unknown", long line=0); 

#else
#define log_message(m,f,l) //NULL
#define log_message(m) //NULL
#endif //DEBUGGING
*/
#ifdef DEBUGGING
//#pragma warning "Debugging is ON"
#else 
//#pragma warning "Debugging is OFF"
#endif

#if defined(_DEBUG) || defined(DEBUGGING)
//#define SafeDelete(x) { fatal_assert(x == NULL, "excepted " #x " to be null", __FILE__, __LINE__); delete x; x = NULL; }
#define SafeDelete(x) { delete x; x = NULL; }
#else 
#define SafeDelete(x) delete x
#endif

#endif //__DEBUGGING_H__

