/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: Window.cpp,v 1.9 2003/01/07 20:24:09 jsaks Exp $
*/

#include <iostream>
#include <CarnegieMellonGraphics.h>
#include <Daemon.h>
#include <PortMutex.h>
#include <Debugging.h>
#include <stdlib.h>
#include <stdio.h>
#include <Globals.h>

using namespace std;

Window::Window(int width, int height, const string &title,bool fullscreen,int bitdepth,int refreshrate)
{
  /* INT_MAX signifies don't change the default position */
  _init(INT_MAX, INT_MAX, width, height, title, fullscreen, bitdepth, refreshrate);
}

Window::Window(int xpos, int ypos, int width, int height, const string &title,bool fullscreen,int bitdepth,int refreshrate) 
{
  _init(xpos, ypos, width, height, title, fullscreen, bitdepth, refreshrate);
}

void Window::_init(int xpos, int ypos, int width, int height, const std::string &title, bool fullscreen,int bitdepth,int refreshrate)
{
  // moving this from the constructor line to here to be more readable:
  this->width             = width;
  this->height            = height;
  this->mousex            = -1;
  this->mousey            = -1;
  this->mousefilter       = MouseEvent(MouseEvent::MOUSE_MOVE_EVENT, MouseEvent::NO_BUTTON, -1, -1, KeyModifiers::NO_MODIFIERS);
  this->keyfilter         = KeyboardEvent(0,KeyModifiers::NO_MODIFIERS);
  this->timerfilter       = TimerEvent(-1);
  this->mousemutex        = new PortMutex;
  this->keymutex          = new PortMutex;
  this->sizemutex         = new PortMutex;
  this->timermutex        = new PortMutex;
  this->colorsignal       = new SemaphoreT<Color>;
  this->imagesignal       = new SemaphoreT<Image>;
  this->positionsignal    = new SemaphoreT<std::pair<int, int> >;
  this->hidden            = false;
  this->autoflushkeyboard = false;
  this->autoflushmouse    = false;
  this->autoflushtimer    = false;
  this->mousesem          = new Semaphore;
  this->keyboardsem       = new Semaphore;
  this->timersem          = new Semaphore;
  this->currentmodifiers  = KeyModifiers::NO_MODIFIERS;
  this->daemon            = NULL;

  if(mousemutex == NULL)
    fatal_exit("Could not allocate memory for mousemutex.  Free up some memory and try again", __FILE__, __LINE__);

  if(keymutex == NULL)
    fatal_exit("Could not allocate memory for keymutex.  Free up some memory and try again", __FILE__, __LINE__);

  if(timermutex == NULL)
    fatal_exit("Could not allocate memory for timermutex.  Free up some memory and try again", __FILE__, __LINE__);

  if(sizemutex == NULL)
    fatal_exit("Could not allocate memory for sizemutex.  Free up some memory and try again", __FILE__, __LINE__);

  if(imagesignal == NULL)
    fatal_exit("Could not allocate memory for imagesignal.  Free up some memory and try again", __FILE__, __LINE__);

  if(colorsignal == NULL)
    fatal_exit("Could not allocate memory for colorsignal.  Free up some memory and try again", __FILE__, __LINE__);

  if(width < 1)
    fatal_exit("Window width is not a positive integer", __FILE__, __LINE__);

  if(height < 1)
    fatal_exit("Window height is a not a positive integer", __FILE__, __LINE__);

  daemon = new Daemon(this);
    if(daemon == NULL)
    fatal_exit("Could not allocate memory for a daemon.  Free up some memory and try again", __FILE__, __LINE__);

  daemon->createWindow(xpos, ypos, width, height, title, fullscreen, bitdepth, refreshrate);
  WindowManager::registerWindow(this);
  m_autoPageFlipStatus = true; // glutimp enables this by default

  // Necessary for doing cooperative threading
  yield();
}

Window::~Window() {
  WindowManager::removeWindow(this);
  daemon->destroyWindow();

  nonfatal_assert((mousemutex == NULL), "mousemutex has somehow become NULL",
                  __FILE__, __LINE__);

  nonfatal_assert((keymutex == NULL), "keymutex has somehow become NULL",
                  __FILE__, __LINE__);

  nonfatal_assert((timermutex == NULL), "timermutex has somehow become NULL",
                  __FILE__, __LINE__);

  nonfatal_assert((sizemutex == NULL), "sizemutex has somehow become NULL",
                  __FILE__, __LINE__);

  nonfatal_assert((imagesignal == NULL), "imagesignal has somehow become NULL",
                  __FILE__, __LINE__);

  nonfatal_assert((colorsignal  == NULL), "colorsignal has somehow become NULL",
                  __FILE__, __LINE__);

  nonfatal_assert((daemon == NULL), "daemon has somehow become NULL",
                  __FILE__, __LINE__);

  SafeDelete (mousemutex);
  SafeDelete (keymutex);
  SafeDelete (timermutex);
  SafeDelete (sizemutex);
  SafeDelete (imagesignal);
  SafeDelete (colorsignal);
  SafeDelete (positionsignal);
  SafeDelete (daemon);
  SafeDelete (mousesem);
  SafeDelete (keyboardsem);
  SafeDelete (timersem);

  // Necessary for doing cooperative threading
  yield();
}

void Window::hide(void) {
  // Necessary for doing cooperative threading
  yield();

  if(hidden == false) {
    daemon->hideWindow();
    hidden = true;
  } else {
    /* this isn't really a problem, is it? */
//    warning("Tried to hide an already hidden window", __FILE__, __LINE__);
  }
}

void Window::show(void) {
  // Necessary for doing cooperative threading
  yield();

  if(hidden == true) {
    daemon->showWindow();
    hidden = false;
  } else {
    /* this isn't really a problem, is it? */
//    warning("Tried to show an already visable window", __FILE__, __LINE__);
  }
}

bool Window::isHidden(void) { return hidden; }

bool Window::isKeyboardQueueEmpty(void) {
  // Necessary for doing cooperative threading
  yield();

  keymutex->lock();

  bool temp = keyqueue.empty();

  keymutex->unlock();

  return temp;
}

bool Window::isMouseQueueEmpty(void) {
  // Necessary for doing cooperative threading
  yield();

  mousemutex->lock();

  bool temp = mousequeue.empty();

  mousemutex->unlock();

  return temp;
}

bool Window::isTimerQueueEmpty(void) {
  // Necessary for doing cooperative threading
  yield();

  timermutex->lock();

  bool temp = timerqueue.empty();

  timermutex->unlock();

  return temp;
}

KeyboardEvent Window::getKeyboardEvent(void) {

  // Necessary for doing cooperative threading
  yield();

  if(isKeyboardQueueEmpty()) {
    fatal_exit("Tried to remove a KeyboardEvent from an empty queue!  Call isKeyboardQueueEmpty() first to ensure that this does not occur",
               __FILE__, __LINE__);
  }

  keymutex->lock();
  KeyboardEvent *ke = keyqueue.front();
  keyqueue.pop_front();
  keymutex->unlock();

  KeyboardEvent temp(*ke);
  delete ke;

  return temp;
}

MouseEvent Window::getMouseEvent(void) {

  // Necessary for doing cooperative threading
  yield();

  if(isMouseQueueEmpty()) {
    fatal_exit("Tried to remove a MouseEvent from an empty queue!  Call isMouseQueueEmpty() first to ensure that this does not occur",
               __FILE__, __LINE__);
  }

  mousemutex->lock();
  MouseEvent *me = mousequeue.front();
  mousequeue.pop_front();
  mousemutex->unlock();

  MouseEvent temp(*me);
  delete me;

  return temp;
}

TimerEvent Window::getTimerEvent(void) {

  // Necessary for doing cooperative threading
  yield();

  if(isTimerQueueEmpty()) {
    fatal_exit("Tried to remove a TimerEvent from an empty queue!  Call isTimerQueueEmpty() first to ensure that this does not occur",
               __FILE__, __LINE__);
  }

  timermutex->lock();
  TimerEvent *te = timerqueue.front();
  timerqueue.pop_front();
  timermutex->unlock();

  TimerEvent temp(*te);
  delete te;

  return temp;
}

KeyboardEvent Window::waitForKeyboardEvent(void) {

 // Necessary for doing cooperative threading
  yield();

  while(true) {
    if(!isKeyboardQueueEmpty()) {
      return getKeyboardEvent();
    }
  }
}

MouseEvent Window::waitForMouseEvent(void) {

  // Necessary for doing cooperative threading
  yield();

  while(true) {
    if(!isMouseQueueEmpty()) {
      return getMouseEvent();
    }
  }
}

TimerEvent Window::waitForTimerEvent(void) {

  // Necessary for doing cooperative threading
  yield();

  while(true) {
    if(!isTimerQueueEmpty()) {
      return getTimerEvent();
    }
  }
}

void Window::flushKeyboardQueue(void) {
  // Necessary for doing cooperative threading
  yield();

  keymutex->lock();

  keyqueue.clear();

  keymutex->unlock();
}

void Window::flushMouseQueue(void) {
  // Necessary for doing cooperative threading
  yield();

  mousemutex->lock();

  mousequeue.clear();

  mousemutex->unlock();
}

void Window::flushTimerQueue(void) {
  // Necessary for doing cooperative threading
  yield();

  timermutex->lock();

  timerqueue.clear();

  timermutex->unlock();
}

void Window::addKeyboardEvent(const KeyboardEvent& keyboard) {

  if (!autoflushkeyboard) {
    keymutex->lock();
    keyqueue.push_back(new KeyboardEvent(keyboard));
    keymutex->unlock();
  }
}

void Window::addMouseEvent(const MouseEvent& mouse) {

  mousemutex->lock();

  if (!autoflushmouse) {
    mousequeue.push_back(new MouseEvent(mouse));
  }

  if((mouse.getX() != -1) && (mouse.getY() != -1)) {
    mousex = mouse.getX();
    mousey = mouse.getY();
  }

  mousemutex->unlock();
}

void Window::addTimerEvent(const TimerEvent& timer) {

  timermutex->lock();
  if (!autoflushtimer) {
    timerqueue.push_back(new TimerEvent(timer));
  }
  timer_count[timer.getTimerID()]++;
  timermutex->unlock();
}

MouseEvent Window::getMouseFilter(void) {
  return mousefilter;
}

KeyboardEvent Window::getKeyboardFilter(void) {
  return keyfilter;
}

TimerEvent Window::getTimerFilter(void) {
  return timerfilter;
}

void Window::setMouseFilter(const MouseEvent &filter) {
  mousefilter = filter;
}

void Window::setKeyboardFilter(const KeyboardEvent &filter) {
  keyfilter = filter;
}

void Window::setTimerFilter(const TimerEvent &filter) {
  timerfilter = filter;
}

void Window::drawPixel(const Style& s, const int x, const int y) {
  // Necessary for doing cooperative threading
  yield();

  daemon->drawPixel(s, x, y);
}

void Window::drawLine(const Style& s, const int x1, const int y1, const int x2, const int y2) {
  // Necessary for doing cooperative threading
  yield();

  daemon->drawLine(s, x1, y1, x2, y2);
}

void Window::drawArc(const Style& s, const int x1, const int y1, const int x2, const int y2, const double start, const double end) {
  // Necessary for doing cooperative threading
  yield();

  // FIX move to a constant somewhere....
  daemon->drawArc(s, x1, y1, x2, y2,
    static_cast<int>(start * ARC_PRECISION) , static_cast<int>(end * ARC_PRECISION));
}

void Window::drawRectangleFilled(const Style& s, const int x1, const int y1, const int x2, const int y2) {
  // Necessary for doing cooperative threading
  yield();

  daemon->drawRectangleFilled(s, x1, y1, x2, y2);
}

void Window::drawRectangleOutline(const Style& s, const int x1, const int y1, const int x2, const int y2) {
  // Necessary for doing cooperative threading
  yield();

  daemon->drawRectangleOutline(s, x1, y1, x2, y2);
}

void Window::drawTriangleFilled(const Style& s, const int x1, const int y1, const int x2, const int y2, const int x3, const int y3) {
  // Necessary for doing cooperative threading
  yield();

  daemon->drawTriangleFilled(s, x1, y1, x2, y2, x3, y3);
}

void Window::drawTriangleOutline(const Style& s, const int x1, const int y1, const int x2, const int y2, const int x3, const int y3) {
  // Necessary for doing cooperative threading
  yield();

  daemon->drawTriangleOutline(s, x1, y1, x2, y2, x3, y3);
}

void Window::drawCircleFilled(const Style& s, const int x, const int y, const int radius) {
  // Necessary for doing cooperative threading
  yield();

  daemon->drawCircleFilled(s, x, y, radius);
}

void Window::drawCircleOutline(const Style& s, const int x, const int y, const int radius) {
  // Necessary for doing cooperative threading
  yield();

  daemon->drawCircleOutline(s, x, y, radius);
}

void Window::drawEllipseFilled(const Style& s, const int x1, const int y1, const int x2, const int y2) {
  // Necessary for doing cooperative threading
  yield();

  daemon->drawEllipseFilled(s, x1, y1, x2, y2);
}

void Window::drawEllipseOutline(const Style& s, const int x1, const int y1, const int x2, const int y2) {
  // Necessary for doing cooperative threading
  yield();

  daemon->drawEllipseOutline(s, x1, y1, x2, y2);
}

void Window::drawWedgeFilled(const Style& s, const int x1, const int y1, const int x2, const int y2, const double start, const double end) {
  // Necessary for doing cooperative threading
  yield();

  daemon->drawWedgeFilled(s, x1, y1, x2, y2,
    static_cast<int>(start * ARC_PRECISION), static_cast<int>(end * ARC_PRECISION));
}

void Window::drawWedgeOutline(const Style& s, const int x1, const int y1, const int x2, const int y2, const double start, const double end) {
  // Necessary for doing cooperative threading
  yield();

  daemon->drawWedgeOutline(s, x1, y1, x2, y2,
    static_cast<int>(start * ARC_PRECISION), static_cast<int>(end * ARC_PRECISION));
}

void Window::drawChordFilled(const Style& s, const int x1, const int y1, const int x2, const int y2, const double start, const double end) {
  // Necessary for doing cooperative threading
  yield();

  daemon->drawChordFilled(s, x1, y1, x2, y2,
    static_cast<int>(start * ARC_PRECISION), static_cast<int>(end * ARC_PRECISION));
}

void Window::drawChordOutline(const Style& s, const int x1, const int y1, const int x2, const int y2, const double start, const double end) {
  // Necessary for doing cooperative threading
  yield();

  daemon->drawChordOutline(s, x1, y1, x2, y2,
    static_cast<int>(start * ARC_PRECISION), static_cast<int>(end * ARC_PRECISION));
}

void Window::drawText(const Style& s, const Font& f, const int x, const int y, const string& text) {
 // Necessary for doing cooperative threading
  yield();

  daemon->drawText(s, f, x, y, text);
}

void Window::drawText(const Style& s, const Font& f, const int x, const int y, const string& text,const Transform &transform) {
 // Necessary for doing cooperative threading
  yield();

  daemon->drawText(s, f, x, y, text, transform);
}

void Window::drawPolygonFilled(const Style &s, const vector<int>& xcoords, const vector<int>& ycoords) {
 // Necessary for doing cooperative threading
  yield();

  if(xcoords.size() != ycoords.size()) {
    fatal_exit("The xcoords and ycoords vectors passed to drawPolygonFilled are different sizes",  __FILE__, __LINE__);
  }

  daemon->drawPolygonFilled(s, xcoords, ycoords);
}

void Window::drawPolygonOutline(const Style &s, const vector<int>& xcoords, const vector<int>& ycoords) {
 // Necessary for doing cooperative threading
  yield();

  if(xcoords.size() != ycoords.size()) {
    fatal_exit("The xcoords and ycoords vectors passed to drawPolygonOutline are different sizes",  __FILE__, __LINE__);
  }

  daemon->drawPolygonOutline(s, xcoords, ycoords);
}

void Window::drawPolyLine(const Style &s, const vector<int>& xcoords, const vector<int>& ycoords) {
 // Necessary for doing cooperative threading
  yield();

  if(xcoords.size() != ycoords.size()) {
    fatal_exit("The xcoords and ycoords vectors passed to drawPolyLine are different sizes",  __FILE__, __LINE__);
  }

  daemon->drawPolyLine(s, xcoords, ycoords);
}

void Window::drawBezierCurve(const Style &s, const vector<int>& xcoords, const vector<int>& ycoords) {
 // Necessary for doing cooperative threading
  yield();

  if(xcoords.size() != ycoords.size()) {
    fatal_exit("The xcoords and ycoords vectors passed to drawPolyLine are different sizes",  __FILE__, __LINE__);
  }

  daemon->drawBezierCurve(s, xcoords, ycoords);
}

void Window::drawImage(const Image &image, const int x, const int y) {
  // Necessary for doing cooperative threading
  yield();

  daemon->drawImage(image, x, y);
}

void Window::drawImage(const Image &image, const int x, const int y,const Transform &transform) {
  // Necessary for doing cooperative threading
  yield();

  daemon->drawImage(image, x, y,transform);
}


Image Window::createImage(const int x, const int y, const int width, const int height) {
  // Necessary for doing cooperative threading
  yield();

  if(width < 1)
    fatal_exit("The rectangle chosen to create an image from is less than one pixel wide", __FILE__, __LINE__);

  if(height < 1)
    fatal_exit("The rectangle chosen to create an image from is less than one pixel high", __FILE__, __LINE__);

  if((x < 0) || (y < 0))
    warning("The rectangle chosen to create an image from is outside the bounds of the window", __FILE__, __LINE__);
  if((x > this->width) || (y > this->height))
    warning("The rectangle chosen to create an image from is outside the bounds of the window", __FILE__, __LINE__);
  if((x + width > this->width) || (y + height > this->height))
    warning("The rectangle chosen to create an image from is outside the bounds of the window", __FILE__, __LINE__);

  return daemon->createImage(x, y, width, height);
}

void Window::copyRegion(const int x1, const int y1, const int x2, const int y2, const int width, const int height) {
  if(width < 1)
    fatal_exit("The rectangle chosen for copying a region from is less than one pixel wide", __FILE__, __LINE__);

  if(height < 1)
    fatal_exit("The rectangle chosen for copying a region from is less than one pixel high", __FILE__, __LINE__);

  if((x1 < 0) || (y1 < 0))
    warning("The rectangle chosen for copying a region is outside the bounds of the window", __FILE__, __LINE__);
  if((x1 > this->width) || (y1 > this->height))
    warning("The rectangle chosen for copying a region is outside the bounds of the window", __FILE__, __LINE__);
  if((x1 + width > this->width) || (y1 + height > this->height))
    warning("The rectangle chosen for copying a region is outside the bounds of the window", __FILE__, __LINE__);

  daemon->copyRegion(x1, y1, x2, y2, width, height);
}

Color Window::readPixel(const int x, const int y) {
  // Necessary for doing cooperative threading
  yield();

  if((x < 0) || (y < 0))
    warning("The pixel being read is outside the bounds of the window", __FILE__, __LINE__);
  if((x > this->width) || (y > this->height))
    warning("The pixel being read is outside the bounds of the window", __FILE__, __LINE__);

  return daemon->readPixel(x, y);
}

void Window::flipPage(void) {
  // Necessary for doing cooperative threading
  yield();

  daemon->flipPage();
}

void Window::enableAntialiasing(void) {
  // Necessary for doing cooperative threading
  yield();

  daemon->enableAntialiasing();
}

void Window::disableAntialiasing(void) {
  // Necessary for doing cooperative threading
  yield();

  daemon->disableAntialiasing();
}

void Window::enableFullScreen(int bitdepth,int refreshrate) {
  yield();
  daemon->enableFullScreen(bitdepth,refreshrate);
}

void Window::disableFullScreen() {
  yield();
  daemon->disableFullScreen();
}


void Window::enableAutoPageFlip(void) {
  // Necessary for doing cooperative threading
  yield();

  m_autoPageFlipStatus = true;
  daemon->enableAutoPageFlip();
}


void Window::disableAutoPageFlip(void) {
  // Necessary for doing cooperative threading
  yield();

  m_autoPageFlipStatus = false;
  daemon->disableAutoPageFlip();
}

bool Window::isAutoPageFlipEnabled() {
  return m_autoPageFlipStatus;
}

void Window::handleKeyboardEvent(const KeyboardEvent&) {
  // Necessary for doing cooperative threading
  yield();
}

void Window::handleMouseEvent(const MouseEvent&) {
  // Necessary for doing cooperative threading
  yield();
}

void Window::handleIdleEvent(void) {
  // Necessary for doing cooperative threading
  yield();
}

TimerEvent Window::registerNewTimerEvent(const int milliseconds) {
  // Necessary for doing cooperative threading
  yield();

  TimerEvent t = TimerEvent(daemon->registerNewTimerEvent(milliseconds));

  return t;
}

TimerEvent Window::startTimer(int milliseconds) {
  yield();

  TimerEvent t = TimerEvent(daemon->startTimer(milliseconds));

  return t;
}

void Window::stopTimer(const TimerEvent &timer) {
  yield();

  daemon->stopTimer(timer.getTimerID());
}

void Window::handleTimerEvent(const TimerEvent&) {
  // Necessary for doing cooperative threading
  yield();
}

void Window::_setWindowSize(int newwidth, int newheight) {
  sizemutex->lock();
  width = newwidth;
  height = newheight;
  sizemutex->unlock();
}

/* fix: thanks to elatimer.  Why this ever worked, I will never know. */

/*
int Window::getWidth(void) const {
  sizemutex->lock();
  return width;
  sizemutex->unlock();
}
*/

int Window::getWidth(void) const {
  int w;
  sizemutex->lock();
  w = width;
  sizemutex->unlock();
  return w;
}

int Window::getHeight(void) const {
  int h;
  sizemutex->lock();
  h = height;
  sizemutex->unlock();
  return h;
}

int Window::getMouseX(void) const {
  return mousex;
}

int Window::getMouseY(void) const {
  return mousey;
}

void Window::autoFlushKeyboardQueue(bool enable) {
  flushKeyboardQueue();
  autoflushkeyboard = enable;
}

void Window::autoFlushMouseQueue(bool enable) {
  flushMouseQueue();
  autoflushmouse = enable;
}

void Window::autoFlushTimerQueue(bool enable) {
  flushTimerQueue();
  autoflushtimer = enable;
}

bool Window::isKeyDown(int key) {
  return key_state[key];
}

bool Window::isButtonDown(MouseEvent::Button b) {
  return button_state[b];
}

bool Window::isModifierDown(KeyModifiers km) {
  // do a little bit manipulation magic to see if all the bits in km are in the current modifiers.
  return ((km & currentmodifiers) == km);
}

int Window::getTimerCount(const TimerEvent &te) {
  timermutex->lock();
  int c = timer_count[te.getTimerID()];
  timermutex->unlock();
  return c;
}

void Window::setTimerCount(const TimerEvent &te, int count) {
  timermutex->lock();
  timer_count[te.getTimerID()] = count;
  timermutex->unlock();
}

void Window::ignoreKeyRepeat(bool ignore) {
  yield();

  daemon->ignoreKeyRepeat(ignore);
}

std::string Window::numberToString(int n) {
  char temp[100];
  sprintf(temp, "%d", n);
  return string(temp);
}

std::string Window::numberToString(double d, int precision) {
  char tempformat[100],temp[1000];
  if (precision < 0) strcpy(tempformat, "%f");
  else {
    if (precision > 100) precision = 100;
    sprintf(tempformat, "%%.%d%%f", precision);
  }
  sprintf(temp, tempformat, d);
  return string(temp);
}

/////

void Window::resize(int newwidth, int newheight) {
  yield();

  daemon->resize(newheight, newwidth);

  _setWindowSize(newheight, newwidth);
}

void Window::setPosition(int xpos, int ypos) {
  yield();

  daemon->setPosition(xpos, ypos);
}

void Window::getPosition(int &x, int &y) {
  yield();

  daemon->getPosition(x, y);
}

void Window::setTitle(const std::string &title) {
  yield();

  daemon->setTitle(title);
}

//////

// since there's only a constructor, this is an appropriate place for now
FullScreenWindow::FullScreenWindow(int width, int height, int bitdepth, int refreshrate) :
 Window(width, height, "CarnegieMellonGraphics", true, bitdepth, refreshrate) {
 }

