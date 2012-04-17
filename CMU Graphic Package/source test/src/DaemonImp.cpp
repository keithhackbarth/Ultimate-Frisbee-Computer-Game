/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: DaemonImp.cpp,v 1.5 2003/01/07 20:24:02 jsaks Exp $
*/

#include <Daemon.h>
#include <DaemonImp.h>
#include <Debugging.h>

deque<DaemonCommand*> DaemonImp::comqueue;
PortMutex DaemonImp::commutex;

///////////////////////////////////////////////////////////////////////////////

DaemonImp::DaemonImp(void) {


}

DaemonImp::~DaemonImp() {


}

///////////////////////////////////////////////////////////////////////////////

int DaemonImp::getFontHandle(const string, const int) {
  // Dummy return value, should never be called anyway
  return 0;
}

int DaemonImp::getFontHandle(int, int) {

  // Dummy return value, should never be called anyway
  return 0;
}

void DaemonImp::releaseFontHandle(const int) {
}

void DaemonImp::bumpFontRefCount(const int) {
}

bool DaemonImp::checkFont(const string, const int) {
  // Dummy return value, should never be called anyway
  return false;
}

int DaemonImp::getImageHandle(const string, const Image::Type, const int *, const int *) {
  // Dummy return value, should never be called anyway
  return 0;
}

void DaemonImp::releaseImageHandle(const int) {
}

void DaemonImp::bumpImageRefCount(const int) {
}

void DaemonImp::saveImage(const int, const string, const Image::Type) {
}

bool DaemonImp::checkImage(const string, const Image::Type) {
  // Dummy return value, should never be called anyway
  return false;
}

int DaemonImp::subImage(const int ,int ,int ,int ,int ) {
  return 0;
}

int DaemonImp::createEditableImage(int) {
  return 0;
}

int DaemonImp::createEditableImage(int, int) {
  return 0;
}

unsigned char *DaemonImp::getImageData(int) {
  return NULL;
}


int DaemonImp::registerNewTimerEvent(Window *, const int) {
  // Dummy return value, should never be called anyway
  return 0;
}

int DaemonImp::startTimer(Window *, int) {
  return 0;
}

void DaemonImp::stopTimer(Window *, int) {

}

///////////////////////////////////////////////////////////////////////////////


void DaemonImp::throttle() {

   while(true) {
     commutex.lock();
     if(comqueue.size() < THROTTLE_SIZE) {
       commutex.unlock();
       break;
     }
     commutex.unlock();
     yield();
   }
}

void DaemonImp::createWindow(Window *window, int xpos, int ypos, const int width, const int height, const string& title, bool fullscreen, int bitdepth, int refreshrate) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::CREATE_WINDOW, window);

  temp->addArgument(xpos);
  temp->addArgument(ypos);
  temp->addArgument(width);
  temp->addArgument(height);
  temp->addArgument(fullscreen?1:0);
  temp->addArgument(bitdepth);
  temp->addArgument(refreshrate);

  temp->setString(title);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::destroyWindow(Window *window) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DESTROY_WINDOW, window);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::hideWindow(Window *window) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::OBSCURE_WINDOW, window);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();

}

void DaemonImp::showWindow(Window *window) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::SHOW_WINDOW, window);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();

}

void DaemonImp::resize(Window *window, int w, int h) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::RESIZE_WINDOW, window);

  temp->addArgument(w);
  temp->addArgument(h);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();

}

void DaemonImp::setPosition(Window *window, int x, int y) {
  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::SET_WINDOW_POSITION, window);

  temp->addArgument(x);
  temp->addArgument(y);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();

}

void DaemonImp::getPosition(Window *window, int &x, int &y) {
  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::GET_WINDOW_POSITION, window);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();

  window->positionsignal->wait();
  x = window->positionsignal->message.first;
  y = window->positionsignal->message.second;

}


void DaemonImp::setTitle(Window *window, const std::string &title) {
  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::SET_WINDOW_TITLE, window);

  temp->setString(title);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();

}

void DaemonImp::enableAntialiasing(Window *window) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::ENABLE_ANTIALIASING, window);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::disableAntialiasing(Window *window) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DISABLE_ANTIALIASING, window);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::enableFullScreen(Window *window,int bitdepth,int refreshrate) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::ENABLE_FULLSCREEN, window);

  temp->addArgument(bitdepth);
  temp->addArgument(refreshrate);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::disableFullScreen(Window *window) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DISABLE_FULLSCREEN, window);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}


void DaemonImp::enableAutoPageFlip(Window *window) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::ENABLE_AUTO_PAGE_FLIP, window);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::disableAutoPageFlip(Window *window) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DISABLE_AUTO_PAGE_FLIP, window);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawPixel(Window *window, const Style& s, const int x, const int y) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_PIXEL, window);

  temp->addArgument(x);
  temp->addArgument(y);

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawLine(Window *window, const Style& s, const int x1, const int y1, const int x2, const int y2) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_LINE, window);

  temp->addArgument(x1);
  temp->addArgument(y1);
  temp->addArgument(x2);
  temp->addArgument(y2);

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawArc(Window *window, const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_ARC, window);

  temp->addArgument(x1);
  temp->addArgument(y1);
  temp->addArgument(x2);
  temp->addArgument(y2);
  temp->addArgument(start);
  temp->addArgument(end);

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}


void DaemonImp::drawRectangleFilled(Window *window, const Style& s, const int x1, const int y1, const int x2, const int y2) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_RECTANGLE_FILLED, window);

  temp->addArgument(x1);
  temp->addArgument(y1);
  temp->addArgument(x2);
  temp->addArgument(y2);

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawRectangleOutline(Window *window, const Style& s, const int x1, const int y1, const int x2, const int y2) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_RECTANGLE_OUTLINE, window);

  temp->addArgument(x1);
  temp->addArgument(y1);
  temp->addArgument(x2);
  temp->addArgument(y2);

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawTriangleFilled(Window *window, const Style& s, const int x1, const int y1, const int x2, const int y2, const int x3, const int y3) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_TRIANGLE_FILLED, window);

  temp->addArgument(x1);
  temp->addArgument(y1);
  temp->addArgument(x2);
  temp->addArgument(y2);
  temp->addArgument(x3);
  temp->addArgument(y3);

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawTriangleOutline(Window *window, const Style& s, const int x1, const int y1, const int x2, const int y2, const int x3, const int y3) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_TRIANGLE_OUTLINE, window);

  temp->addArgument(x1);
  temp->addArgument(y1);
  temp->addArgument(x2);
  temp->addArgument(y2);
  temp->addArgument(x3);
  temp->addArgument(y3);

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawCircleFilled(Window *window, const Style& s, const int x, const int y, const int radius) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_CIRCLE_FILLED, window);

  temp->addArgument(x);
  temp->addArgument(y);
  temp->addArgument(radius);

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawCircleOutline(Window *window, const Style& s, const int x, const int y, const int radius) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_CIRCLE_OUTLINE, window);

  temp->addArgument(x);
  temp->addArgument(y);
  temp->addArgument(radius);

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}


void DaemonImp::drawEllipseFilled(Window *window, const Style& s, const int x1, const int y1, const int x2, const int y2) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_ELLIPSE_FILLED, window);

  temp->addArgument(x1);
  temp->addArgument(y1);
  temp->addArgument(x2);
  temp->addArgument(y2);

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawEllipseOutline(Window *window, const Style& s, const int x1, const int y1, const int x2, const int y2) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_ELLIPSE_OUTLINE, window);

  temp->addArgument(x1);
  temp->addArgument(y1);
  temp->addArgument(x2);
  temp->addArgument(y2);

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawWedgeFilled(Window *window, const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_WEDGE_FILLED, window);

  temp->addArgument(x1);
  temp->addArgument(y1);
  temp->addArgument(x2);
  temp->addArgument(y2);
  temp->addArgument(start);
  temp->addArgument(end);

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawWedgeOutline(Window *window, const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_WEDGE_OUTLINE, window);

  temp->addArgument(x1);
  temp->addArgument(y1);
  temp->addArgument(x2);
  temp->addArgument(y2);
  temp->addArgument(start);
  temp->addArgument(end);

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawChordFilled(Window *window, const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_CHORD_FILLED, window);

  temp->addArgument(x1);
  temp->addArgument(y1);
  temp->addArgument(x2);
  temp->addArgument(y2);
  temp->addArgument(start);
  temp->addArgument(end);

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawChordOutline(Window *window, const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_CHORD_OUTLINE, window);

  temp->addArgument(x1);
  temp->addArgument(y1);
  temp->addArgument(x2);
  temp->addArgument(y2);
  temp->addArgument(start);
  temp->addArgument(end);

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawText(Window *window, const Style& s, const Font& f, const int x, const int y, const string& text) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_TEXT, window);

  temp->addArgument(x);
  temp->addArgument(y);

  temp->addArgument(f.fonthandle);

  temp->setStyle(s);

  temp->setString(text);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawText(Window *window, const Style& s, const Font& f, const int x, const int y, const string& text, const Transform &transform) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_TEXT, window);

  temp->addArgument(x);
  temp->addArgument(y);

  temp->addArgument(f.fonthandle);

  temp->addArgument((int)(transform.getRotation()*Transform::SCALE));
  temp->addArgument((int)(transform.getScaleX()*Transform::SCALE));
  temp->addArgument((int)(transform.getScaleY()*Transform::SCALE));

  temp->setStyle(s);

  temp->setString(text);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawPolygonFilled(Window *window, const Style &s, const vector<int>& xcoords, const vector<int>& ycoords) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_POLYGON_FILLED, window);

  temp->addArgument(static_cast<int>(xcoords.size()));

  unsigned int index;
  for(index = 0; index < xcoords.size(); index++) {
    temp->addArgument(xcoords[index]);
    temp->addArgument(ycoords[index]);
  }

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}


void DaemonImp::drawPolygonOutline(Window *window, const Style &s, const vector<int>& xcoords, const vector<int>& ycoords) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_POLYGON_OUTLINE, window);

  temp->addArgument(static_cast<int>(xcoords.size()));

  unsigned int index;
  for(index = 0; index < xcoords.size(); index++) {
    temp->addArgument(xcoords[index]);
    temp->addArgument(ycoords[index]);
  }

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawPolyLine(Window *window, const Style &s, const vector<int>& xcoords, const vector<int>& ycoords) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_POLYLINE, window);

  temp->addArgument(static_cast<int>(xcoords.size()));

  unsigned int index;
  for(index = 0; index < xcoords.size(); index++) {
    temp->addArgument(xcoords[index]);
    temp->addArgument(ycoords[index]);
  }

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawBezierCurve(Window *window, const Style &s, const vector<int>& xcoords, const vector<int>& ycoords) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_BEZIER_CURVE, window);

  temp->addArgument(static_cast<int>(xcoords.size()));

  unsigned int index;
  for(index = 0; index < xcoords.size(); index++) {
    temp->addArgument(xcoords[index]);
    temp->addArgument(ycoords[index]);
  }

  temp->setStyle(s);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawImage(Window *window, const Image &image, const int x, const int y) {
  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_IMAGE, window);

  /* we have to bump the ref count so that if the image object that was passed to us happens
   * to be destroyed before we draw it, it will still exist -- we will release our handle
   * when we're done. Our implementation must remember to release the handle. */
  Daemon::bumpImageRefCount(image.imagehandle);

  temp->addArgument(image.imagehandle);
  temp->addArgument(x);
  temp->addArgument(y);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::drawImage(Window *window, const Image &image, const int x, const int y, const Transform& transform) {
  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::DRAW_IMAGE_TRANSFORMED, window);

  /* see comment in drawImage above for why we do this */
  Daemon::bumpImageRefCount(image.imagehandle);

  temp->addArgument(image.imagehandle);
  temp->addArgument(x);
  temp->addArgument(y);
  // the stupid queue only accepts integers as arguments to a command. must lose precision, but it's okay.
  temp->addArgument((int)(transform.getRotation()*Transform::SCALE));
  temp->addArgument((int)(transform.getScaleX()*Transform::SCALE));
  temp->addArgument((int)(transform.getScaleY()*Transform::SCALE));

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

Image DaemonImp::createImage(Window *window, const int x, const int y, const int width, const int height) {
  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::CREATE_IMAGE, window);


  temp->addArgument(x);
  temp->addArgument(y);
  temp->addArgument(width);
  temp->addArgument(height);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();

  window->imagesignal->wait();
  return window->imagesignal->message;
}

void DaemonImp::copyRegion(Window* window, const int x1, const int y1, const int x2, const int y2, const int width, const int height) {
  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::COPY_REGION, window);

  temp->addArgument(x1);
  temp->addArgument(y1);
  temp->addArgument(x2);
  temp->addArgument(y2);
  temp->addArgument(width);
  temp->addArgument(height);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

Color DaemonImp::readPixel(Window *window, const int x, const int y) {
  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::READ_PIXEL, window);

  temp->addArgument(x);
  temp->addArgument(y);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();

  window->colorsignal->wait();
  // once we've gotten the go-ahead, return the message
  return window->colorsignal->message;
}

void DaemonImp::flipPage(Window *window) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::FLIP_PAGE, window);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

void DaemonImp::ignoreKeyRepeat(Window *window, bool ignore) {
  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::IGNORE_KEY_REPEAT, window);

  temp->addArgument(ignore);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}
