/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: WindowManager.cpp,v 1.3 2003/01/06 03:53:10 jsaks Exp $
*/

#include <ctime>
#include <CarnegieMellonGraphics.h>
#include <version.h>
#include <Debugging.h>
#include <GLUTImp.h>
#include <Globals.h>

struct WindowManagerStatics {
  std::list<Window *> registeredwindows; 
  std::map<Window *, MouseEvent> mousefilters;
  std::map<Window *, KeyboardEvent> keyfilters;
  std::map<Window *, TimerEvent> timerfilters;
 
  bool handleractive;

  WindowManagerStatics() {
    handleractive = false;
  }
};

typedef Globals<WindowManagerStatics> WMGlobals;

/************************************************************************************/

// WindowManager stuff

void WindowManager::registerWindow(Window *window) {
  static WMGlobals globals;

  fatal_assert((window == NULL), "How did you do that?  registerWindow has been passed a NULL pointer", __FILE__, __LINE__);

  if(globals->handleractive == true) {
    globals->mousefilters[window] = window->getMouseFilter();
//    window->setMouseFilter(NULL);
    globals->keyfilters[window] = window->getKeyboardFilter();   
//    window->setKeyboardFilter(NULL);
    globals->timerfilters[window] = window->getTimerFilter();   
//    window->setTimerFilter(NULL);
  }

  globals->registeredwindows.push_front(window);
}

void WindowManager::removeWindow(Window *window) {
  static WMGlobals globals;

  fatal_assert((window == NULL), "How did you do that?  removeWindow has been passed a NULL pointer", __FILE__, __LINE__);

  globals->registeredwindows.remove(window);

  if(globals->handleractive == true) {

    fatal_assert((globals->mousefilters.count(window) != 1), 
      "Tried to remove a window that is not in the mousefilter map", __FILE__, __LINE__);

    window->setMouseFilter(globals->mousefilters[window]);
    globals->mousefilters.erase(window);

    fatal_assert((globals->keyfilters.count(window) != 1), 
      "Tried to remove a window that is not in the keyfilter map", __FILE__, __LINE__);

    window->setKeyboardFilter(globals->keyfilters[window]);
    globals->keyfilters.erase(window);

    fatal_assert((globals->timerfilters.count(window) != 1), 
      "Tried to remove a window that is not in the timerfilter map", __FILE__, __LINE__);

    window->setTimerFilter(globals->timerfilters[window]);
    globals->timerfilters.erase(window);
  }
}

void WindowManager::handleEvents() {
  static WMGlobals globals;


  list<Window *>::iterator iterate;

  iterate = globals->registeredwindows.begin();
  while(iterate != globals->registeredwindows.end()) {

    fatal_assert((*iterate == NULL), "There seems to be a NULL pointer in the window list", __FILE__, __LINE__);

    globals->mousefilters[*iterate] = (*iterate)->getMouseFilter();
//    (*iterate)->setMouseFilter(NULL);
    globals->keyfilters[*iterate] = (*iterate)->getKeyboardFilter();
//    (*iterate)->setKeyboardFilter(NULL);
    globals->timerfilters[*iterate] = (*iterate)->getTimerFilter();
//    (*iterate)->setTimerFilter(NULL);
    iterate++;
  }
   
  globals->handleractive = true;
  while(globals->handleractive) {
    // Necessary for doing cooperative threading
    yield();

    iterate = globals->registeredwindows.begin();
    while(iterate != globals->registeredwindows.end()) {
      fatal_assert((*iterate == NULL), "There seems to be a a NULL pointer in the window list", __FILE__, __LINE__);

      while(!(*iterate)->isTimerQueueEmpty())
        (*iterate)->handleTimerEvent((*iterate)->getTimerEvent());

      while(!(*iterate)->isKeyboardQueueEmpty())
        (*iterate)->handleKeyboardEvent((*iterate)->getKeyboardEvent());

      while(!(*iterate)->isMouseQueueEmpty())
        (*iterate)->handleMouseEvent((*iterate)->getMouseEvent());      
 
      (*iterate)->handleIdleEvent();
      
      // Necessary for doing cooperative threading
      yield();
      iterate++;
    }
 
  }

  iterate = globals->registeredwindows.begin();
  while(iterate != globals->registeredwindows.end()) {

    fatal_assert((*iterate == NULL), "There seems to be a NULL pointer in the window list", __FILE__, __LINE__);

    fatal_assert((globals->mousefilters.count(*iterate) != 1), 
      "There is a window in the window list that is not in the mousefilter map", __FILE__, __LINE__);

    (*iterate)->setMouseFilter(globals->mousefilters[*iterate]);
///    mousefilters[*iterate] = NULL;
    globals->mousefilters.erase(*iterate);

    fatal_assert((globals->keyfilters.count(*iterate) != 1), 
      "There is a window in the window list that is not in the keyfilter map.", __FILE__, __LINE__);

    (*iterate)->setKeyboardFilter(globals->keyfilters[*iterate]);
//    keyfilters[*iterate] = NULL;
    globals->keyfilters.erase(*iterate);

    fatal_assert((globals->timerfilters.count(*iterate) != 1), 
      "There is a window in the window list that is not in the timerfilter map.", __FILE__, __LINE__);

    (*iterate)->setTimerFilter(globals->timerfilters[*iterate]);
//    timerfilters[*iterate] = NULL;
    globals->timerfilters.erase(*iterate);
    iterate++;
  }
}

void WindowManager::exitHandler() {
  static WMGlobals globals;
  globals->handleractive = false;
}

unsigned long WindowManager::getTime() {
  return static_cast<unsigned long>(clock());
}
