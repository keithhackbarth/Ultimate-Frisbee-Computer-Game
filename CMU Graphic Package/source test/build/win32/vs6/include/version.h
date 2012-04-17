/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: version.h,v 1.3 2002/10/23 18:50:25 jsaks Exp $
*/

#ifndef __VERSION_H__
#define __VERSION_H__

#include <iostream>

using namespace std;

// Current Renderer is written using GLUT
// Perhaps change to something less hackish
#define RENDERER GLUTImp

// You will need to uncomment these if your build system doesn't 
// automagically define these and you are using them
#define HAVE_LIBJPEG
#define HAVE_LIBPNG

////// Specify the type of threading used //////

//#define COOP_THREADS  // Cooperative threading using GNU Portable Threads 
#define POSIX_THREADS   // POSIX compliant threading
//#define BEOS_THREADS  // BEOS threads 
//#define WIN32_THREADS // Win32 threading
//#define MACOS_THREADS // MacOS threading

////// Include the correct headers for threading type ////// 
#ifdef COOP_THREADS
  #include <pth.h>
#endif //COOP_THREADS

#ifdef POSIX_THREADS
  #include <pthread.h>
  #include <sched.h>  
#endif //POSIX_THREADS

#ifdef BEOS_THREADS
  #include <be/kernel/OS.h>
#endif //BEOS_THREADS    

#ifdef MACOS_THREADS
  namespace macspace {
  #include <threads.h>
  }
#endif //MACOS_THREADS

////// Define the threading specific yield function //////

inline void yield() {

// FIX -- add some error checking

  #ifdef COOP_THREADS
    pth_yield(0);
  #endif //COOP_THREADS

  #ifdef POSIX_THREADS
    sched_yield();
  #endif //POSIX_THREADS
  
  #ifdef MACOS_THREADS
    macspace::YieldToAnyThread();
  #endif //MACOS_THREADS
}

////// Define the return type and default parameter for a spawned thread //////

#ifdef COOP_THREADS
#define THREAD_RETURN_TYPE void *
#define THREAD_RETURN_VAL NULL
#endif //COOP_THREADS

#ifdef POSIX_THREADS
#define THREAD_RETURN_TYPE void *
#define THREAD_RETURN_VAL NULL
#endif //POSIX_THREADS

#ifdef BEOS_THREADS
#define THREAD_RETURN_TYPE int32
// FIX?
#define THREAD_RETURN_VAL 0
#endif //BEOS_THREADS

#ifdef MACOS_THREADS
#define THREAD_RETURN_TYPE pascal void *
#define THREAD_RETURN_VAL NULL
#endif //MACOS_THREADS

////// Define the threadng specific spawning function //////

inline void spawn_thread(THREAD_RETURN_TYPE(*function)(void *)) {

// FIX -- add some error checking

#ifdef COOP_THREADS
  pth_init();
  pth_attr_t attr = pth_attr_new();
  pth_spawn(attr, function, NULL);
  pth_yield(NULL);
#endif //COOP_THREADS

#ifdef POSIX_THREADS
  pthread_t pthread_id;  
  pthread_create(&pthread_id, NULL, function, NULL);
#endif //POSIX_THREADS

#ifdef BEOS_THREADS
  thread_id daemon_thread;
  daemon_thread = spawn_thread(function, "Daemon", B_DISPLAY_PRIORITY, NULL);
  resume_thread(daemon_thread);
#endif //BEOS_THREADS

#ifdef MACOS_THREADS
  macspace::ThreadID temp;
  if(macspace::NewThread(macspace::kCooperativeThread, function, NULL, 0, macspace::kCreateIfNeeded, 0, &temp) 
     != macspace::noErr) 
     cout << "Error creating thread!" << endl;
#endif //MACOS_THREADS

}

////// Define a few constants //////

const double ARC_PRECISION = 1024.0;  // Number of subdivision of a degree
                                      // kept when drawing arcs, wedges, etc.

const unsigned int THROTTLE_SIZE = 1; // The maximum number of drawing commands
                                      // that may be placed in the queue at 
                                      // one time.  Used to prevent too many
                                      // commands from being sent and swamping
                                      // the renderer.   Can be used to tweak
                                      // performance.  For example, if your 
                                      // programs act on mouse or keyboard 
                                      // input infrequently, larger values will
                                      // improve drawing performance.

#endif //__VERSION_H__
