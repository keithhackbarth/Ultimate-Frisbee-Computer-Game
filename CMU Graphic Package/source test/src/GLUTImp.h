/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: GLUTImp.h,v 1.6 2003/01/06 19:36:00 jsaks Exp $
*/

#ifndef __GLUTIMP_H__
#define __GLUTIMP_H__

#include <glut.h>
#include <map>
#include <DaemonImp.h>
#include <gltt/FTFace.h>
#include <gltt/GLTTFont.h>

class GLUTImp : public DaemonImp {

 public:

  // Public interfaces needed to having a working Daemon Implementation 
  GLUTImp(void);
  ~GLUTImp();

  static int getFontHandle(const string face, const int pointsize);
  static int getFontHandle(int fontID, int pointsize);
  static void releaseFontHandle(const int fonthandle); 
  static void bumpFontRefCount(const int fonthandle);
  static bool checkFont(const string face, const int pointsize);
  static void getStringSize(int fonthandle, const string &text, int &width, int &height);

  static int getImageHandle(const string file, const Image::Type type, int *imagewidth, int *imageheight);
  static void releaseImageHandle(const int imagehandle); 
  static void bumpImageRefCount(const int imagehandle);
  static void saveImage(const int imagehandle, const string file, const Image::Type type);
  static bool checkImage(const string file, const Image::Type type);
  static int subImage(const int imagehandle, int x, int y, int width, int height);
  static int createEditableImage(int imagehandle);
  static int createEditableImage(int width, int height);
  static unsigned char *getImageData(int imagehandle);
  static void *getImageInstance(int imagehandle);

  static int registerNewTimerEvent(Window *window, const int milliseconds);
  static int startTimer(Window *window, int milliseconds);
  static void stopTimer(Window *window, int timerid);

 protected: 

  // Start GLUT running
  static THREAD_RETURN_TYPE start_glut(void *argument); 
 
   // Function that handles DaemonCommands for the implementation
  static void handleCommand(DaemonCommand &com);
 
  static void setCallbacks();
  // GLUT Callback functions
  static void display(void);
  static void reshape(int width, int height);
  static void keyboard(unsigned char key, int x, int y);
  static void mouse(int button, int state, int x, int y);
  static void motion(int x, int y);
  static void passivemotion(int x, int y);
  static void entry(int state);
  static void special(int key, int x, int y);
  static void idle(void); 
  static void timer(int timerid);
  static void keyboardup(unsigned char key, int x, int y);
  static void specialup(int key, int x, int y);

  static KeyModifiers getModifiers();
  static int glutKeyToNamedKey(int key);

  // Utility functions and values for GL polygon tesselation
  static void CALLBACK handleTessVertex(double *vertex);
  static void CALLBACK handleTessBegin(int mode);
  static void CALLBACK handleTessEnd(void);
  static void CALLBACK handleTessError(int error);
  static bool tessfail;

  // More utility functions to reduce code clutter.
  static void enableLogicalOp(Style::RasterOp rop);
  static void disableLogicalOp(void);
  static void enableAntialias(void);
  static void disableAntialias(void);
  // spelling has to be different b/c you can't override a static function or something
  static int enableFullscreen(int height,int width,int bitdepth,int refreshrate);
  static void disableFullscreen(void);
  static void reshapeWindow(int width, int height);
  static void enableStyle(Window::SafeWindowPtr wind, const Style &style);
  static void disableStyle(Window::SafeWindowPtr wind, const Style &style);
  static void drawArcInternal(int x1, int y1, int x2, int y2, int start, int end);
  static void drawEllipseInternal(int x1, int y1, int x2, int y2);
  static void setColor(const Color &color);

  friend class EditableImage;
  friend class WindowManager;
  //// Internal classes for Fonts and Images ////
public: // need this class to be public
  class FontInstance {
   public:
    // Return a handle to a font with face "name" with a point size of "size"
    // pass in "" for name and number for fontID if you want a builtin font instead
    static int getFontHandle(const string name, const int size,int fontID=-1);
    // Release a font handle;
    static void releaseFontHandle(const int handle);
    static void bumpFontRefCount(const int handle);
    static bool checkFont(const string name, const int pointsize);

    // Draw text at x, y using the specified handle
    static void drawText(const int handle, Window* window, const int x, const int y, const string* text, const Transform *transform=NULL);
    static void getStringSize(int handle, const string &text, int &width, int &height);

    // Lazy font creation...
    void create(Window* window);

   private:
    string name;
    int size;
    FTFace face;
    GLTTFont *font;
    int fontID;
    void *glutFont;
    map<Window*, bool> created;

    FontInstance(const string name, const int size);
    FontInstance(int fontID, const int size);
    ~FontInstance();

    friend class GLUTImp;
  };

  public: // just be explicit about its security
  class ImageInstance {
   public:
    // Obtain a handle to a image with the specified name
    static int getImageHandle(const string name, const Image::Type type, unsigned short *width, unsigned short *height);
    // Release a image handle
    static void releaseImageHandle(const int handle);
    static void bumpImageRefCount(const int handle);
    static void saveImage(const int handle, const string file, const Image::Type type);
    static bool checkImage(const string file, const Image::Type type);

    // Draw the specified image at location (x, y)
    static void drawImage(const int handle, const int x, const int y);
    static void drawImage(const int handle, const int x, const int y,const Transform& transform);

    // Create a new image from the specified location
    static int createImage(const int windowheight, const int x, const int y, const int width, const int height);
    static int subImage(const int handle, int x, int y, int width, int height);
    static int createEditableImage(int imagehandle);
    static int createEditableImage(int width, int height);
    static unsigned char *getImageData(int imagehandle);
    static ImageInstance *getImageInstance(int imagehandle);

   private:
    unsigned short imagewidth;
    unsigned short imageheight;
    unsigned char *data;
	  // jsaks 12/13/01: 
    // this is where the texture map version of the data is stored.  it is the closest 2^n x 2^m appoximation
	  // of data that can be determined, only computed if you try to draw the rotated version
	  // jsaks 11/14/02:
	  // in editableimages, it doesn't make sense to keep resizing the image.  So when an image is made editable,
	  // it edits both the data and texdata.
	  unsigned char *texdata;
	  unsigned short texdatawidth, texdataheight;
    friend class EditableImage;
    Image::Type type;

    ImageInstance(const string name, const Image::Type type, unsigned short *width, unsigned short *height);
    ImageInstance(unsigned char *data, const Image::Type type, unsigned short width, unsigned short height);
    ~ImageInstance();

    #ifdef HAVE_LIBJPEG
    static void jpegload(const char *cpFileName, unsigned short *uspWidth, unsigned short *uspHeight, unsigned char **ucpImageData);
    static void jpegsave(const char *cpFileName, unsigned short usWidth, unsigned short usHeight, unsigned char *ucpImageData);
    #endif //HAVE_LIBJPEG

    #ifdef HAVE_LIBPNG
    static void pngload(const char *cpFileName, unsigned short *uspWidth, unsigned short *uspHeight, unsigned char **ucpImageData);
    static void pngsave(const char *cpFileName, unsigned short usWidth, unsigned short usHeight, unsigned char *ucpImageData);
    #endif //HAVE_LIBPNG

    friend class GLUTImp;
  };
};

#endif //__GLUTIMP_H__
