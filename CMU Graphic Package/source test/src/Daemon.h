/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: Daemon.h,v 1.4 2003/01/07 20:24:02 jsaks Exp $
*/

#ifndef __DAEMON_H__
#define __DAEMON_H__

#include <CarnegieMellonGraphics.h>
#include <version.h>

class Daemon {

 public:
  Daemon(Window* window);
  ~Daemon();
   
  void createWindow(int xpos, int ypos, const int width, const int height, const string& title, bool fullscreen, int bitdepth, int refreshrate);
  void destroyWindow(void);

  void hideWindow(void);
  void showWindow(void);
  void resize(int w, int h);
  void setPosition(int x, int y);
  void getPosition(int &x, int &y);
  void setTitle(const std::string &title);

  void drawPixel(const Style& s, const int x, const int y);
  void drawLine(const Style& s, const int x1, const int y1, const int x2, const int y2);
  void drawArc(const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end);
  void drawRectangleFilled(const Style& s, const int x1, const int y1, const int x2, const int y2);
  void drawRectangleOutline(const Style& s, const int x1, const int y1, const int x2, const int y2);
  void drawTriangleFilled(const Style& s, const int x1, const int y1, const int x2, const int y2, const int x3, const int y3);
  void drawTriangleOutline(const Style& s, const int x1, const int y1, const int x2, const int y2, const int x3, const int y3);
  void drawCircleFilled(const Style& s, const int x, const int y, const int radius);
  void drawCircleOutline(const Style& s, const int x, const int y, const int radius);
  void drawEllipseFilled(const Style& s, const int x1, const int y1, const int x2, const int y2);
  void drawEllipseOutline(const Style& s, const int x1, const int y1, const int x2, const int y2);
  void drawWedgeFilled(const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end);
  void drawWedgeOutline(const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end);
  void drawChordFilled(const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end);
  void drawChordOutline(const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end);
  void drawText(const Style& s, const Font& f, const int x, const int y, const string& text); 
  void drawText(const Style& s, const Font& f, const int x, const int y, const string& text, const Transform &transform); 
  void drawPolygonFilled(const Style &s, const vector<int>& xcoords, const vector<int>& ycoords);
  void drawPolygonOutline(const Style &s, const vector<int>& xcoords, const vector<int>& ycoords);
  void drawPolyLine(const Style &s, const vector<int>& xcoords, const vector<int>& ycoords);
  void drawBezierCurve(const Style &s, const vector<int>& xcoords, const vector<int>& ycoords);
  void drawImage(const Image &image, const int x, const int y);
  void drawImage(const Image &image, const int x, const int y, const Transform &transform);
  Image createImage(const int x, const int y, const int width, const int height);
  void copyRegion(const int x1, const int y1, const int x2, const int y2, const int width, const int height);
  Color readPixel(const int x, const int y);
  void flipPage(void);

  void enableAntialiasing(void);
  void disableAntialiasing(void);

  void enableFullScreen(int bitdepth,int refreshrate);
  void disableFullScreen(void);

  void enableAutoPageFlip(void);
  void disableAutoPageFlip(void);

  void ignoreKeyRepeat(bool ignore);

  static int getFontHandle(const string face, const int pointsize);
  static int getFontHandle(const int fontID, const int pointsize);
  static void releaseFontHandle(const int fonthandle);
  static void bumpFontRefCount(const int fonthandle);
  static bool checkFont(const string face, const int pointsize);
  static void getStringSize(int fonthandle, const string &text, int &width, int &height);

  static int getImageHandle(const string file, const Image::Type type, int *imagewidth, int *imageheight);
  static void releaseImageHandle(const int imagehandle);
  static void bumpImageRefCount(const int imagehandle);
  static void saveImage(const int imagehandle, const string file, const Image::Type type);
  static bool checkImage(const string file, const Image::Type type);
  static int subImage(int imagehandle,int x,int y,int width,int height);
  static int createEditableImage(int imagehandle);
  static int createEditableImage(int width, int height);
  static unsigned char *getImageData(int imagehandle);
  // HACK! But we're already locked into this implementation, so it doesn't even matter.
  static void *getImageInstance(int imagehandle);

  int registerNewTimerEvent(const int milliseconds);
  int startTimer(int milliseconds);
  void stopTimer(int timerid);

 
 private:
  Window *window;
  static class RENDERER *renderer;
  static bool renderer_active;

};

#endif //__DAEMON_H__
