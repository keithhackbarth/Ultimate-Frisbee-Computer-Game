/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: PortMutex.cpp,v 1.4 2003/01/07 20:24:09 jsaks Exp $
*/

#include <PortMutex.h>

PortMutex::PortMutex() {
  #ifdef POSIX_THREADS
    pthread_mutex_init(&posix_mutex, NULL);
  #endif //POSIX_THREADS
  #ifdef BEOS_THREADS
    beos_mutex = create_sem(0, "BeOS Semephore");
  #endif //BEOS_THREADS
}

PortMutex::~PortMutex() {
  #ifdef POSIX_THREADS
    pthread_mutex_destroy(&posix_mutex);
  #endif //POSIX_THREADS
  #ifdef BEOS_THREADS
    delete_sem(beos_mutex);
  #endif //BEOS_THREADS
}

void PortMutex::lock() {
  #ifdef POSIX_THREADS
    pthread_mutex_lock(&posix_mutex);
  #endif //POSIX_THREADS
  #ifdef BEOS_THREADS
    //acquire_sem(beos_mutex);
  #endif //BEOS_THREADS
}

void PortMutex::unlock() {
  #ifdef POSIX_THREADS
    pthread_mutex_unlock(&posix_mutex);
  #endif //POSIX_THREADS
  #ifdef BEOS_THREADS
    //release_sem(beos_mutex);
  #endif //BEOS_THREADS
  }


Semaphore::Semaphore() {
  #ifdef POSIX_THREADS
    pthread_mutex_init(&posix_mutex, NULL);
    pthread_cond_init(&posix_cond, NULL);
  #endif //POSIX_THREADS
  flag = 0;
}

Semaphore::~Semaphore() {
  #ifdef POSIX_THREADS
    pthread_mutex_destroy(&posix_mutex);
    pthread_cond_destroy(&posix_cond);
  #endif //POSIX_THREADS
}

void Semaphore::wait() {
  #ifdef POSIX_THREADS
  pthread_mutex_lock(&posix_mutex);

  while(flag==0) pthread_cond_wait(&posix_cond, &posix_mutex);
  flag = 0;
  pthread_mutex_unlock(&posix_mutex);
  #endif //POSIX_THREADS
}

void Semaphore::signal() {
  #ifdef POSIX_THREADS
  pthread_mutex_lock(&posix_mutex);
  flag = 1;
  pthread_mutex_unlock(&posix_mutex);
  pthread_cond_signal(&posix_cond);
  #endif //POSIX_THREADS
}
