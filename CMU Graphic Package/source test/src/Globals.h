/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: Globals.h,v 1.2 2003/01/07 20:24:08 jsaks Exp $
*/

#ifndef __GLOBALS_H_
#define __GLOBALS_H_

#include <new>

/* This style of doing global static variables is adapted from 
 * James Kanze's post: http://cpptips.hyperformix.com/cpptips/glob_init2 */
template<class T>
class Globals {
public:
  Globals() {
    if (impl == NULL) {
      init();
    }
  }
  T* operator->() const {
    return impl;
  }
private:
  static T*   impl;
  static void init() {
    //cout << "init was called for a struct of size " << sizeof(T) << endl;
    static union
	  {
	    char   buf[ sizeof( T ) ] ;
	    double dummyForAlignment ;
	  }        data ;
    impl = new (data.buf) T;
  }
};

template<class T> T* Globals<T>::impl = NULL;

#endif
