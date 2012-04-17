/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: DaemonImp.h,v 1.3 2003/01/07 20:24:02 jsaks Exp $
*/

#ifndef __DAEMONIMP_H__
#define __DAEMONIMP_H__

#include <DaemonCommand.h>
#include <PortMutex.h>

class DaemonImp {

 public:
  
  static void createWindow(Window *window, int xpos, int ypos, const int width, const int height, const string& title, bool fullscreen, int bitdepth, int refreshrate);
  static void destroyWindow(Window *window);

  static void hideWindow(Window *window);
  static void showWindow(Window *window);
  static void resize(Window *window, int w, int h);
  static void setPosition(Window *window, int x, int y);
  static void getPosition(Window *window, int &x, int &y);
  static void setTitle(Window *window, const std::string &title);

  static void enableAntialiasing(Window *window);
  static void disableAntialiasing(Window *window);

  static void enableFullScreen(Window *window,int bitdepth,int refreshrate);
  static void disableFullScreen(Window *window);

  static void enableAutoPageFlip(Window *window);
  static void disableAutoPageFlip(Window *window);

  static void drawPixel(Window* window, const Style& s, const int x, const int y);
  static void drawLine(Window* window, const Style& s, const int x1, const int y1, const int x2, const int y2);
  static void drawArc(Window* window, const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end);
  static void drawRectangleFilled(Window* window, const Style& s, const int x1, const int y1, const int x2, const int y2);
  static void drawRectangleOutline(Window* window, const Style& s, const int x1, const int y1, const int x2, const int y2);
  static void drawTriangleFilled(Window* window, const Style& s, const int x1, const int y1, const int x2, const int y2, const int x3, const int y3);
  static void drawTriangleOutline(Window* window, const Style& s, const int x1, const int y1, const int x2, const int y2, const int x3, const int y3);
  static void drawCircleFilled(Window* window, const Style& s, const int x, const int y, const int radius);
  static void drawCircleOutline(Window* window, const Style& s, const int x, const int y, const int radius);
  static void drawEllipseFilled(Window* window, const Style& s, const int x1, const int y1, const int x2, const int y2);
  static void drawEllipseOutline(Window* window, const Style& s, const int x1, const int y1, const int x2, const int y2);
  static void drawWedgeFilled(Window* window, const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end);
  static void drawWedgeOutline(Window* window, const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end);
  static void drawChordFilled(Window* window, const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end);
  static void drawChordOutline(Window* window, const Style& s, const int x1, const int y1, const int x2, const int y2, const int start, const int end);
  static void drawText(Window *window, const Style& s, const Font& f, const int x, const int y, const string& text); 
  static void drawText(Window *window, const Style& s, const Font& f, const int x, const int y, const string& text, const Transform &transform); 
  static void drawPolygonFilled(Window *window, const Style &s, const vector<int>& xcoords, const vector<int>& ycoords);
  static void drawPolygonOutline(Window *window, const Style &s, const vector<int>& xcoords, const vector<int>& ycoords);
  static void drawPolyLine(Window *window, const Style &s, const vector<int>& xcoords, const vector<int>& ycoords);
  static void drawBezierCurve(Window *window, const Style &s, const vector<int>& xcoords, const vector<int>& ycoords);
  static void drawImage(Window *window, const Image &image, const int x, const int y);
  static void drawImage(Window *window, const Image &image, const int x, const int y, const Transform& transform);
  static Image createImage(Window *window, const int x, const int y, const int width, const int height);
  static void copyRegion(Window* window, const int x1, const int y1, const int x2, const int y2, const int width, const int height);
  static Color readPixel(Window *window, const int x, const int y);
  static void flipPage(Window *window);
  static void ignoreKeyRepeat(Window *window, bool ignore);
  
  // These functions *must* be implemented to provide a working implementation class

  DaemonImp(void);  // Create a new DaemonImp Object
  ~DaemonImp();     // Cleanup 

  static int getFontHandle(const string face, const int pointsize);
  static int getFontHandle(int fontID, const int pointsize);
  static void releaseFontHandle(const int fonthandle); 
  static void bumpFontRefCount(const int fonthandle);
  static bool checkFont(const string face, const int pointsize);
 
  static int getImageHandle(const string file, const Image::Type type, const int *imagewidth, const int *imageheight);
  static void releaseImageHandle(const int imagehandle); 
  static void bumpImageRefCount(const int imagehandle);
  static void saveImage(const int imagehandle, const string file, const Image::Type type);
  static bool checkImage(const string file, const Image::Type type);
  static int subImage(const int imagehandle,int x,int y,int width,int height);
  static int createEditableImage(int imagehandle);
  static int createEditableImage(int width, int height);
  static unsigned char *getImageData(int imagehandle);

  static int registerNewTimerEvent(Window *window, const int milliseconds);
  static int startTimer(Window *window, int milliseconds);
  static void stopTimer(Window *window, int timerid);

 protected:
  static deque<DaemonCommand*> comqueue;
  static PortMutex commutex;
  static void throttle();
  
};

#endif //__DAEMONIMP_H__
