/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: PortMutex.h,v 1.4 2003/01/07 20:24:09 jsaks Exp $
*/

#ifndef __PORTMUTEX_H__
#define __PORTMUTEX_H__

#include <version.h>

// "Portable" Mutual Exclusion (Mutex) Class

class PortMutex {
 private:
 
  #ifdef POSIX_THREADS
    pthread_mutex_t posix_mutex;
  #endif //POSIX_THREADS
  #ifdef BEOS_THREADS
    sem_id beos_mutex;
  #endif //BEOS_THREADS

 public:
  PortMutex(); 
  ~PortMutex();
  void lock();
  void unlock();
};

class Semaphore {
private:
  #ifdef POSIX_THREADS
    pthread_mutex_t posix_mutex;
    pthread_cond_t  posix_cond;
  #endif //POSIX_THREADS
    /* TODO: how to do it in BeOS? probably doesn't really matter anymore... */

  // need to lock the checks for the flag
  PortMutex p;
  volatile int flag;
public:
	Semaphore();
  ~Semaphore();
	void wait(); 
	void signal();
};

template <class T>
class SemaphoreT : public Semaphore {
public: 
  T message;
};

#endif //__PORTMUTEX_H__
