/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: Daemon.cpp,v 1.5 2003/01/07 20:24:02 jsaks Exp $
*/

#include <Daemon.h>
#include <GLUTImp.h>
#include <version.h>
#include <Debugging.h>

bool Daemon::renderer_active = false;
RENDERER *Daemon::renderer = NULL;

Daemon::Daemon(Window *window) : window(window) {
  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);
  
  if(!renderer_active) {
    renderer = new RENDERER;
    renderer_active = true;
  }
}

Daemon::~Daemon() {
//  delete renderer;
}

void Daemon::createWindow(int xpos, int ypos, const int width, const int height, const string& title, bool fullscreen, int bitdepth, int refreshrate) {
  RENDERER::createWindow(window, xpos, ypos, width, height, title, fullscreen, bitdepth, refreshrate);
}

void Daemon::destroyWindow(void) {
  RENDERER::destroyWindow(window);
}

void Daemon::hideWindow(void) {
  RENDERER::hideWindow(window);
}

void Daemon::showWindow(void) {
  RENDERER::showWindow(window);
}

void Daemon::enableAntialiasing(void) {
  RENDERER::enableAntialiasing(window);
}

void Daemon::disableAntialiasing(void) {
  RENDERER::disableAntialiasing(window);
}

void Daemon::enableFullScreen(int bitdepth,int refreshrate) {
  RENDERER::enableFullScreen(window,bitdepth,refreshrate);
}

void Daemon::disableFullScreen(void) {
  RENDERER::disableFullScreen(window);
}

void Daemon::enableAutoPageFlip(void) {
  RENDERER::enableAutoPageFlip(window);
}

void Daemon::disableAutoPageFlip(void) {
  RENDERER::disableAutoPageFlip(window);
}

void Daemon::drawPixel(const Style& s, const int x, const int y) {
  RENDERER::drawPixel(window, s, x, y);
}

void Daemon::drawLine(const Style& s, const int x1, const int y1, const int x2, const int y2) {
  RENDERER::drawLine(window, s, x1, y1, x2, y2);
}

void Daemon::drawArc(const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end) {
  RENDERER::drawArc(window, s, x1, y1, x2, y2, start, end);
}

void Daemon::drawRectangleFilled(const Style& s, const int x1, const int y1, const int x2, const int y2) {
  RENDERER::drawRectangleFilled(window, s, x1, y1, x2, y2);
}

void Daemon::drawRectangleOutline(const Style& s, const int x1, const int y1, const int x2, const int y2) {
  RENDERER::drawRectangleOutline(window, s, x1, y1, x2, y2);
}

void Daemon::drawTriangleFilled(const Style& s, const int x1, const int y1, const int x2, const int y2, const int x3, const int y3) {
  RENDERER::drawTriangleFilled(window, s, x1, y1, x2, y2, x3, y3);
}

void Daemon::drawTriangleOutline(const Style& s, const int x1, const int y1, const int x2, const int y2, const int x3, const int y3) {
  RENDERER::drawTriangleOutline(window, s, x1, y1, x2, y2, x3, y3);
}

void Daemon::drawCircleFilled(const Style& s, const int x, const int y, const int radius) {
  RENDERER::drawCircleFilled(window, s, x, y, radius);
}

void Daemon::drawCircleOutline(const Style& s, const int x, const int y, const int radius) {
  RENDERER::drawCircleOutline(window, s, x, y, radius);
}

void Daemon::drawEllipseFilled(const Style& s, const int x1, const int y1, const int x2, const int y2) {
  RENDERER::drawEllipseFilled(window, s, x1, y1, x2, y2);
}

void Daemon::drawEllipseOutline(const Style& s, const int x1, const int y1, const int x2, const int y2) {
  RENDERER::drawEllipseOutline(window, s, x1, y1, x2, y2);
}

void Daemon::drawWedgeFilled(const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end) {
  RENDERER::drawWedgeFilled(window, s, x1, y1, x2, y2, start, end);
}

void Daemon::drawWedgeOutline(const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end) {
  RENDERER::drawWedgeOutline(window, s, x1, y1, x2, y2, start, end);
}

void Daemon::drawChordFilled(const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end) {
  RENDERER::drawChordFilled(window, s, x1, y1, x2, y2, start, end);
}

void Daemon::drawChordOutline(const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end) {
  RENDERER::drawChordOutline(window, s, x1, y1, x2, y2, start, end);
}

void Daemon::drawText(const Style& s, const Font& f, const int x, const int y, const string& text) {
  RENDERER::drawText(window, s, f, x, y, text);
}

void Daemon::drawText(const Style& s, const Font& f, const int x, const int y, const string& text, const Transform &transform) {
  RENDERER::drawText(window, s, f, x, y, text, transform);
}

void Daemon::drawPolygonFilled(const Style &s, const vector<int>& xcoords, const vector<int>& ycoords) {
  RENDERER::drawPolygonFilled(window, s, xcoords, ycoords);
}

void Daemon::drawPolygonOutline(const Style &s, const vector<int>& xcoords, const vector<int>& ycoords) {
  RENDERER::drawPolygonOutline(window, s, xcoords, ycoords);
}

void Daemon::drawPolyLine(const Style &s, const vector<int>& xcoords, const vector<int>& ycoords) {
  RENDERER::drawPolyLine(window, s, xcoords, ycoords);
}

void Daemon::drawBezierCurve(const Style &s, const vector<int>& xcoords, const vector<int>& ycoords) {
  RENDERER::drawBezierCurve(window, s, xcoords, ycoords);
}

void Daemon::drawImage(const Image &image, const int x, const int y) {
  RENDERER::drawImage(window, image, x, y);
}

void Daemon::drawImage(const Image &image, const int x, const int y,const Transform& transform) {
  RENDERER::drawImage(window, image, x, y, transform);
}

Image Daemon::createImage(const int x, const int y, const int width, const int height) {
  return RENDERER::createImage(window, x, y, width, height);
}

void Daemon::flipPage(void) {
  RENDERER::flipPage(window);
}

void Daemon::copyRegion(const int x1, const int y1, const int x2, const int y2, const int width, const int height) {
  RENDERER::copyRegion(window, x1, y1, x2, y2, width, height);
}

Color Daemon::readPixel(const int x, const int y) {
  return RENDERER::readPixel(window, x, y);
}

int Daemon::getFontHandle(const string face, const int pointsize) {
  return RENDERER::getFontHandle(face, pointsize);
}

int Daemon::getFontHandle(const int fontID, const int pointsize) {
  return RENDERER::getFontHandle(fontID, pointsize);
}

void Daemon::releaseFontHandle(const int fonthandle) {
  RENDERER::releaseFontHandle(fonthandle);
}

bool Daemon::checkFont(const string face, const int pointsize) {
  return RENDERER::checkFont(face, pointsize);
}

void Daemon::getStringSize(int fonthandle, const string &text, int &width, int &height) {
  RENDERER::getStringSize(fonthandle, text, width, height);
}

int Daemon::getImageHandle(const string file, const Image::Type type, int *imagewidth, int *imageheight) {
  return RENDERER::getImageHandle(file, type, imagewidth, imageheight);
}

void *Daemon::getImageInstance(int imagehandle) {
  return RENDERER::getImageInstance(imagehandle);
}

void Daemon::releaseImageHandle(const int imagehandle) {
  RENDERER::releaseImageHandle(imagehandle);
}

void Daemon::bumpImageRefCount(const int imagehandle) {
  RENDERER::bumpImageRefCount(imagehandle);
}

void Daemon::saveImage(const int imagehandle, const string file, const Image::Type type) {
  RENDERER::saveImage(imagehandle, file, type);
}

bool Daemon::checkImage(const string file, const Image::Type type) {
  return RENDERER::checkImage(file, type);
}

void Daemon::bumpFontRefCount(const int fonthandle) {
  RENDERER::bumpFontRefCount(fonthandle);
}

int Daemon::registerNewTimerEvent(const int milliseconds) {
  return RENDERER::registerNewTimerEvent(window, milliseconds);
}

int Daemon::startTimer(int milliseconds) {
  return RENDERER::startTimer(window, milliseconds);
}

void Daemon::stopTimer(int timerid) {
  RENDERER::stopTimer(window, timerid);
}

int Daemon::subImage(int imagehandle,int x,int y,int width,int height) {
  return RENDERER::subImage(imagehandle,x,y,width,height);
}

void Daemon::ignoreKeyRepeat(bool ignore) {
  RENDERER::ignoreKeyRepeat(window, ignore);
}

int Daemon::createEditableImage(int imagehandle) {
  return RENDERER::createEditableImage(imagehandle);
}

int Daemon::createEditableImage(int width, int height) {
  return RENDERER::createEditableImage(width, height);
}

unsigned char *Daemon::getImageData(int imagehandle) {
  return RENDERER::getImageData(imagehandle);
}

void Daemon::resize(int w, int h) {
  RENDERER::resize(window, w, h);
}

void Daemon::setPosition(int x, int y) {
  RENDERER::setPosition(window, x, y);
}

void Daemon::getPosition(int &x, int &y) {
  RENDERER::getPosition(window, x, y);
}

void Daemon::setTitle(const std::string &title) {
  RENDERER::setTitle(window, title);
}