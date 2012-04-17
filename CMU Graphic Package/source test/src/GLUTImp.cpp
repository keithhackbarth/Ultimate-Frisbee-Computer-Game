/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: GLUTImp.cpp,v 1.13 2003/01/07 20:24:03 jsaks Exp $
*/

#include <iostream>
#include <stack>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <glut.h>
#include <version.h>
#include <GLUTImp.h>
#include <Debugging.h>
#include <Globals.h>
#include <Daemon.h>

using namespace std;
const int FUDGE = 100;

#ifdef HAVE_LIBJPEG
extern "C" {
#include <jpeglib.h>
}
#endif //HAVE_LIBJPEG

#ifdef HAVE_LIBPNG
#include <png.h>
#endif //HAVE_LIBPNG

enum TimerType {TIMER_ONCE, TIMER_REPEAT, TIMER_STOPPED};

struct GLUTImpStatics {
  // Keep track of how many timers we've created
  int timercount;

  // Mapping between Window pointers and GLUT Window ids
  map<Window::SafeWindowPtr, int> wid_to_gid;
  map<int, Window::SafeWindowPtr> gid_to_wid;

  // Mapping from timers to Winodws
  map<int, Window::SafeWindowPtr> tid_to_wid;
  map<int, TimerType>             tid_to_type;
  map<int, int>                   tid_to_interval;

  // Mapping between Window pointers and antialiasing and page flipping
  // settings
  map<Window::SafeWindowPtr, bool> wid_to_alias;
  map<Window::SafeWindowPtr, bool> wid_to_flip;
  map<Window::SafeWindowPtr, bool> wid_to_fullscreen;

  // Last button state
  MouseEvent::Button lastbutton;

  GLUTImpStatics() {
    lastbutton = MouseEvent::NO_BUTTON;
    timercount = 0;
  }
};

bool GLUTImp::tessfail;

typedef Globals<GLUTImpStatics> GIGlobals;

// this is stupid; don't know why I have to do this
#ifdef _MSC_VER
#define log2(x) (log((double)x)/log(2.0))
#else
#define log2(x) (std::log((double)x)/std::log(2.0))
#endif

///////////////////////////////////////////////////////////////////////////////

// Spawn the daemon thread when a new object is created
GLUTImp::GLUTImp() {
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );

  spawn_thread(start_glut);
}

// No cleanup necessary yet
GLUTImp::~GLUTImp() {
}

///////////////////////////////////////////////////////////////////////////////

// Font Instance stuff
struct GLUTImpFontInstanceStatics {
  map<string, int> cache;
  map<int, GLUTImp::FontInstance *> handles;
  map<int, int> refcounts;

  int handlecount;
  PortMutex handlemutex;

  GLUTImpFontInstanceStatics() {
    handlecount = 0;
  }
};

typedef Globals<GLUTImpFontInstanceStatics> GIFIGlobals;

GLUTImp::FontInstance::FontInstance(const string name, const int size) :
 name(name), size(size),font(NULL)  {

  font = new GLTTFont(&face);

  if(font == NULL)
    fatal_exit("Could not allocate sufficent memory to allocate a new font.  Free up some memory and try again", __FILE__, __LINE__);

  if(!face.open(name.c_str())) {
    string err = "Unable to open font in file \"" + name + "\". Check to be sure that this file exists";
    fatal_exit(err.c_str(), __FILE__, __LINE__);
  }
}

int getNearestIndex(int size,int *sizes) {
  int i = 0;
  char temp[10000];
  while (sizes[i] != 0 && size > sizes[i]) {
    i++;
  }
  // size > all possible sizes
  int ret;
  if (sizes[i] == 0) ret = i-1;
  else ret=i;

  if (size != sizes[ret]) {
    sprintf(temp,"%d is not a valid size for this font. rounding to %d",size,sizes[ret]);
    warning(temp,__FILE__, __LINE__);
  }

  return ret;
}


GLUTImp::FontInstance::FontInstance(int fontid, int s) :
  name(""),size(s),font(NULL),fontID(fontid),glutFont(NULL) {
  int helvsizes[] = {10,12,18,0};
  void *helvnames[] = {(void*)GLUT_BITMAP_HELVETICA_10,(void*)GLUT_BITMAP_HELVETICA_12,(void*)GLUT_BITMAP_HELVETICA_18};
  int timessizes[] = {10,24,0};
  void *timesnames[] = {(void*)GLUT_BITMAP_TIMES_ROMAN_10,(void*)GLUT_BITMAP_TIMES_ROMAN_24};
  int *sizes;
  void **names;

  if (fontid == Font::ROMAN) {
    glutFont = (void*)GLUT_STROKE_ROMAN;
  } else if (fontid == Font::MONO_ROMAN) {
    glutFont = (void*)GLUT_STROKE_MONO_ROMAN;
  } else {
    if (fontID == Font::HELVETICA) {
      sizes = helvsizes;
        names = helvnames;
    } else if (fontID == Font::TIMES) {
        sizes = timessizes;
        names = timesnames;
    } else {
      fatal_exit("Invalid Font ID -- use the constants!!",__FILE__,__LINE__);
    }
    glutFont = (void*)names[getNearestIndex(size,sizes)];
  }

}

void GLUTImp::FontInstance::create(Window *window) {
  fatal_assert((window == NULL), "I seem to have been passed a null window pointer", __FILE__, __LINE__);

  if(created.count(window) == 0) {

    if(!font->create(size)) {
    string err = "Unable to create a font using  \"" + name + "\" of the specified size";
      fatal_exit(err.c_str(), __FILE__, __LINE__);
    }

    created[window] = true;
  }
}

GLUTImp::FontInstance::~FontInstance() {
  if (!font) SafeDelete(font);
}

int GLUTImp::FontInstance::getFontHandle(const string name, const int size, int fontID) {
  int newhandle;
  static GIFIGlobals globals;  

  char tempstr[10000];
//  sprintf(tempstr,"%500s:%d:%d",name.c_str(),fontID,size);



  string cachename = name + ":";
  sprintf(tempstr,"%d",size);
  cachename += tempstr;
  cachename += ":";
  sprintf(tempstr,"%d",fontID);
  cachename += tempstr;

  globals->handlemutex.lock();
  if(globals->cache.count(cachename) != 0) {
    newhandle = globals->cache[cachename];
    globals->refcounts[newhandle]++;
    nonfatal_assert((globals->refcounts[newhandle] == INT_MAX), "Font refcounts for handle have overflown?", __FILE__, __LINE__);
  } else {
    newhandle =  globals->handlecount++;
    globals->cache[cachename] = newhandle;
    nonfatal_assert((newhandle == INT_MAX), "Did you really create that many fonts?  The Font handle counter has overflown", __FILE__, __LINE__);

    globals->refcounts[newhandle] = 1;

    if (name != "") {
      globals->handles[newhandle] = new FontInstance(name, size);
    } else {
      globals->handles[newhandle] = new FontInstance(fontID,size);
    }

    if(globals->handles[newhandle] == NULL)
      fatal_exit("Could not allocate sufficent memory to create a new FontInstance.  Free up some memory and try again", __FILE__, __LINE__);

  }
  globals->handlemutex.unlock();

  return newhandle;
}

void GLUTImp::FontInstance::releaseFontHandle(const int handle) {
  static GIFIGlobals globals;

  globals->handlemutex.lock();
  globals->refcounts[handle]--;

  nonfatal_assert((globals->refcounts[handle] <  0), "Refcounts for a font handle have somehow become negative", __FILE__, __LINE__);
  globals->handlemutex.unlock();
}

void GLUTImp::FontInstance::bumpFontRefCount(const int handle) {
  static GIFIGlobals globals;

  globals->handlemutex.lock();
  globals->refcounts[handle]++;
  nonfatal_assert((globals->refcounts[handle] == INT_MAX), "Font refcounts for handle have overflown?", __FILE__, __LINE__);
  globals->handlemutex.unlock();
}

bool GLUTImp::FontInstance::checkFont(const string face, const int /*pointsize*/) {

  FILE *fp = fopen(face.c_str(), "rb");
  if(!fp) {
    return false;
  } else {
    fclose(fp);
    return true;
  }
}

void GLUTImp::FontInstance::drawText(const int handle, Window* window, const int x, const int y, const string* text, const Transform *transform) {
  static GIFIGlobals globals;

  fatal_assert((window == NULL), "I seem to have been passed a NULL Window pointer", __FILE__, __LINE__);

  fatal_assert((text == NULL), "I seem to have been passed a NULL string pointer", __FILE__, __LINE__);

  glPushMatrix();

  globals->handlemutex.lock();
  FontInstance *fontinstance = globals->handles[handle];
  globals->handlemutex.unlock();

  fatal_assert((globals->handles.count(handle) != 1), "I seem to have been passed an invalid font handle", __FILE__, __LINE__);

  int i;

  glTranslatef(x, y, 0.0);
  glScalef(1.0, -1.0, 0.0);
  if (transform) {
  glScaled(transform->getScaleX(),transform->getScaleY(),0);
  // have to negate it because of the way fonts work
  glRotated(-transform->getRotation(),0.,0.,1.);
  }

  if (fontinstance->font != NULL) { // non-default font

    fontinstance->create(window);
    fontinstance->font->output(text->c_str());
  } else {
    if (fontinstance->glutFont == GLUT_STROKE_ROMAN || fontinstance->glutFont == GLUT_STROKE_MONO_ROMAN) {
        double d = 100.;
        glScaled(fontinstance->size/d,globals->handles[handle]->size/d,0.);
      for (i = 0; i < (int)text->length(); i++) glutStrokeCharacter(fontinstance->glutFont,(*text)[i]);
    } else {
      glPopMatrix();
        glRasterPos2f(x,y);
      for (i = 0; i < (int)text->length(); i++) glutBitmapCharacter(fontinstance->glutFont,(*text)[i]);
      glPushMatrix();
      if (transform != NULL) {
        warning("You can't transform a bitmapped font.",__FILE__,__LINE__);
      }
    }
  }

  glPopMatrix();


}


void GLUTImp::FontInstance::getStringSize(int handle, const string &text, int &width, int &height) {
  static GIFIGlobals globals;

  globals->handlemutex.lock();
  FontInstance *fontinstance = globals->handles[handle];
  globals->handlemutex.unlock();

  if (fontinstance->font != NULL) {
    height = fontinstance->font->getHeight();
    width = fontinstance->font->getWidth(text.c_str());
  } else {
    if (fontinstance->glutFont == GLUT_STROKE_ROMAN || fontinstance->glutFont == GLUT_STROKE_MONO_ROMAN) {
      width  = glutStrokeLength(fontinstance->glutFont, (const unsigned char*)text.c_str());
      height = fontinstance->size;
    } else {
      width  = glutBitmapLength(fontinstance->glutFont, (const unsigned char*)text.c_str());
      height = fontinstance->size;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

// Image Instance stuff

struct GLUTImpImageInstanceStatics {
  map<string, int> cache;
  map<int, GLUTImp::ImageInstance *> handles;
  map<int, int> refcounts;
  int handlecount;    
  PortMutex handlemutex;

  GLUTImpImageInstanceStatics() {
    handlecount = 0;
  }
};

typedef Globals<GLUTImpImageInstanceStatics> GIIIGlobals;

int GLUTImp::ImageInstance::getImageHandle(const string name, const Image::Type type, unsigned short *width, unsigned short *height) {
  static GIIIGlobals globals;

  int newhandle;

  globals->handlemutex.lock();
  // if we have a handle cached and it hasn't been deleted
  if(globals->cache.count(name) != 0 && globals->handles[globals->cache[name]] != NULL) {
    newhandle = globals->cache[name];
    globals->refcounts[newhandle]++ ;
    *width = globals->handles[newhandle]->imagewidth;
    *height = globals->handles[newhandle]->imageheight;
    nonfatal_assert((globals->refcounts[newhandle] == INT_MAX), "Image refcounts for handle have overflown?", __FILE__, __LINE__);
  } else {
    newhandle =  globals->handlecount++;
    globals->cache[name] = newhandle;
    nonfatal_assert((newhandle == INT_MAX), "Did you really create that many images?  The Image handle counter has overflown.", __FILE__, __LINE__);

    globals->refcounts[newhandle] = 1;

    globals->handles[newhandle] = new ImageInstance(name, type, width, height);

    if(globals->handles[newhandle] == NULL)
      fatal_exit("Could not allocate sufficent memory to create a new ImageInstance.  Free up some memory and try again", __FILE__, __LINE__);

  }
  globals->handlemutex.unlock();

  return newhandle;

}

void GLUTImp::ImageInstance::releaseImageHandle(const int handle) {
  static GIIIGlobals globals;

  if (handle >= 0) {
    globals->handlemutex.lock();
    globals->refcounts[handle]--; 

    nonfatal_assert((globals->refcounts[handle] <  0), "Refcounts for an image handle have somehow become negative", __FILE__, __LINE__);

    if (globals->refcounts[handle] == 0) {
      /* it's a good idea to delete this, but it causes a race condition --
       * if an image is scheduled to draw after the last instance of the image is destructed,
       * we run into major problems. */
      /* the way to fix this (and that i did) is to make sure that if the handle is passed into the other
       * thread it must bump/release the handle. */
      delete globals->handles[handle];
      globals->handles[handle] = NULL;
    }

    globals->handlemutex.unlock();
  }
}

void GLUTImp::ImageInstance::bumpImageRefCount(const int handle) {
  static GIIIGlobals globals;

  if (handle != -1) {
    globals->handlemutex.lock();
    fatal_assert(globals->refcounts[handle] == 0, "Image has been destroyed; cannot bump refcount", __FILE__, __LINE__);
    globals->refcounts[handle]++;
    nonfatal_assert((globals->refcounts[handle] == INT_MAX), "Image refcounts for handle have overflown?", __FILE__, __LINE__);
    globals->handlemutex.unlock();
  }
}

bool GLUTImp::ImageInstance::checkImage(const string file, const Image::Type type) {

  FILE *fp = fopen(file.c_str(), "rb");

  #ifndef HAVE_LIBJPEG
  if(type == Image::JPEG) {
    return false
  }
  #endif //HAVE_LIBJPEG

  #ifndef HAVE_LIBPNG
  if(type == Image::PNG) {
    return false
  }
  #endif //HAVE_LIBPNG

  if(!fp) {
    return false;
  }

  if(type == Image::PNG) {

    #ifdef HAVE_LIBPNG
    png_byte header[8];
    int number=8;
    fread(header, 1, number, fp);
    int is_png = !png_sig_cmp(header, 0, number);
    fclose(fp);
    if (!is_png) {
      return false;
    } else {
      return true;
    }
    #endif //HAVE_LIBPNG

  } else {
    fclose(fp);
    return true;
  }
}

void GLUTImp::ImageInstance::saveImage(const int handle, const string file, const Image::Type type) {
  static GIIIGlobals globals;

  globals->handlemutex.lock();

  ImageInstance* temp = globals->handles[handle];

  if(type == Image::SCREEN) {
    fatal_exit("Can't save an image to type SCREEN, it isn't a file format", __FILE__, __LINE__);
  }

  #ifndef HAVE_LIBJPEG
  if(type == Image::JPEG) {
    fatal_exit("Can't save an image to type JPEG, this version of the library wasn't compiled with JPEG support.", __FILE__, __LINE__);
  }
  #endif //HAVE_LIBJPEG

  #ifndef HAVE_LIBPNG
  if(type == Image::PNG) {
    fatal_exit("Can't save an image to type PNG, this version of the library wasn't compiled with PNG support.", __FILE__, __LINE__);
  }
  #endif //HAVE_LIBPNG

  if((temp->type == Image::JPEG) && (type == Image::JPEG)) {
    #ifdef HAVE_LIBJPEG
    jpegsave(file.c_str(), temp->imagewidth, temp->imageheight, temp->data);
    #endif //HAVE_LIBJPEG
  } else if(((temp->type == Image::PNG) || (temp->type == Image::SCREEN)) &&
            (type == Image::PNG)) {
    #ifdef HAVE_LIBPNG
    pngsave(file.c_str(), temp->imagewidth, temp->imageheight, temp->data);
    #endif //HAVE_LIBPNG
  } else if(((temp->type == Image::PNG) || (temp->type == Image::SCREEN)) &&
            (type == Image::JPEG)) {
    #ifdef HAVE_LIBJPEG
    // Need to remove alpha channel
    unsigned char *newdata = new unsigned char[(temp->imagewidth * temp->imageheight * 3)];
    unsigned char *oldtemp = temp->data;
    unsigned char *newtemp = newdata;

    for(int index = 0; index < temp->imagewidth * temp->imageheight; index++) {
      *(newtemp++) = *(oldtemp++);
      *(newtemp++) = *(oldtemp++);
      *(newtemp++) = *(oldtemp++);
      oldtemp++;
    }

    jpegsave(file.c_str(), temp->imagewidth, temp->imageheight, newdata);

    delete []newdata;
    #endif //HAVE_LIBJPEG
  } else if((temp->type == Image::JPEG) && (type == Image::PNG)) {
    #ifdef HAVE_LIBPNG
    // Need to pad for alpha channel
    unsigned char *newdata = new unsigned char[(temp->imagewidth * temp->imageheight * 4)];
    unsigned char *oldtemp = temp->data;
    unsigned char *newtemp = newdata;

    for(int index = 0; index < temp->imagewidth * temp->imageheight; index++) {
      *(newtemp++) = *(oldtemp++);
      *(newtemp++) = *(oldtemp++);
      *(newtemp++) = *(oldtemp++);
      *(newtemp++) = 255;
    }

    pngsave(file.c_str(), temp->imagewidth, temp->imageheight, newdata);

    delete [] newdata;
    #endif //HAVE_LIBPNG
  }

  globals->handlemutex.unlock();
}

GLUTImp::ImageInstance::ImageInstance(const string name, const Image::Type type, unsigned short *width, unsigned short *height) : type(type) {
  #ifndef HAVE_LIBJPEG
  if(type == Image::JPEG) {
    fatal_exit("Can't load an image of type JPEG, this version of the library wasn't compiled with JPEG support.", __FILE__, __LINE__);
  }
  #endif //HAVE_LIBJPEG

  #ifndef HAVE_LIBPNG
  if(type == Image::PNG) {
    fatal_exit("Can't load an image of type PNG, this version of the library wasn't compiled with PNG support.", __FILE__, __LINE__);
  }
  #endif //HAVE_LIBPNG

  if(type == Image::JPEG) {
    #ifdef HAVE_LIBJPEG
    jpegload(name.c_str(), width, height, &data);
    imagewidth = *width;
    imageheight = *height;
    #endif //HAVE_LIBJPEG
  } else if(type == Image::PNG) {
    #ifdef HAVE_LIBPNG
    pngload(name.c_str(), width, height, &data);
    imagewidth = *width;
    imageheight = *height;
    #endif //HAVE_LIBJPEG
  } else if(type == Image::SCREEN) {
    fatal_exit("Can't load an image of type SCREEN, it isn't a file format", __FILE__, __LINE__);
  }
  texdata = NULL;
}

GLUTImp::ImageInstance::ImageInstance(unsigned char *data, const Image::Type type, unsigned short width, unsigned short height)
 : imagewidth(width), imageheight(height), data(data), texdata(NULL), type(type) { }

GLUTImp::ImageInstance::~ImageInstance() {
  static GIIIGlobals globals;

  globals->handlemutex.lock();
  delete []data;
  if (texdata) delete []texdata;
  globals->handlemutex.unlock();
}

GLUTImp::ImageInstance *GLUTImp::ImageInstance::getImageInstance(int imagehandle) {
  static GIIIGlobals globals;

  return globals->handles[imagehandle];
}

void GLUTImp::ImageInstance::drawImage(const int handle, const int x, const int y) {
  static GIIIGlobals globals;


  if (handle < 0) {
    return;
  }

  globals->handlemutex.lock();
  ImageInstance *temp = globals->handles[handle];

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glDrawBuffer(GL_BACK);
  glRasterPos2i(x, y + temp->imageheight);

  if(temp->type == Image::JPEG) {
    glDrawPixels(temp->imagewidth, temp->imageheight, GL_RGB, GL_UNSIGNED_BYTE, temp->data);
  } else if ((temp->type == Image::PNG) || (temp->type == Image::SCREEN)) {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glDrawPixels(temp->imagewidth, temp->imageheight, GL_RGBA, GL_UNSIGNED_BYTE, temp->data);

    glDisable(GL_BLEND);
  }

  globals->handlemutex.unlock();
}

void GLUTImp::ImageInstance::drawImage(const int handle, const int x, const int y, const Transform &transform) {
  static GIIIGlobals globals;

  if (handle < 0) {
    return;
  }

  globals->handlemutex.lock();
  ImageInstance *temp = globals->handles[handle];

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glDrawBuffer(GL_BACK);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  int bpp = 0;
  GLenum pixelformat = 0;
  if (temp->type == Image::JPEG) {
    bpp = 3;
    pixelformat = GL_RGB;
  } else if ((temp->type == Image::PNG) || (temp->type == Image::SCREEN)) {
    bpp = 4;
    pixelformat = GL_RGBA;
  } else {
    // invalid pixel format
    return;
  }


  static int err = 0;
  if (temp->texdata == NULL) {
    // let's make a 2^n x 2^m image to appease the GL gods -- but only use part of it
    temp->texdatawidth = 1<<((int)ceil(log2(temp->imagewidth)));
    temp->texdataheight = 1<<((int)ceil(log2(temp->imageheight)));
    temp->texdata = new unsigned char[temp->texdatawidth*temp->texdataheight*bpp+FUDGE];
    // jsaks dbg
    temp->texdata[temp->texdatawidth*temp->texdataheight*bpp] = 'J';
    memset(temp->texdata, 0, temp->texdatawidth*temp->texdataheight*bpp);
    for (int row = 0; row < temp->imageheight; row++) {
	    // copy each row from data over to texdata
	    memcpy(temp->texdata + temp->texdatawidth*bpp*row, temp->data + temp->imagewidth*(bpp*row), temp->imagewidth*bpp);
    }
    //err = gluScaleImage(pixelformat,temp->imagewidth,temp->imageheight,GL_UNSIGNED_BYTE,temp->data,temp->texdatawidth,temp->texdataheight,GL_UNSIGNED_BYTE,temp->texdata);
    // jsaks dbg
    fatal_assert(temp->texdata[temp->texdatawidth*temp->texdataheight*bpp] != 'J',"wrote out of bounds on texdata",__FILE__,__LINE__);
  }

  glPushMatrix();
  glTranslatef(x,y,0);
  glRotated(transform.getRotation(),0,0,1);

  if (bpp == 4) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  glTexImage2D(GL_TEXTURE_2D,0,bpp,temp->texdatawidth,temp->texdataheight,0,pixelformat,GL_UNSIGNED_BYTE,temp->texdata);
  glEnable(GL_TEXTURE_2D);

  double yratio = temp->imageheight/(double)temp->texdataheight;
  double xratio = temp->imagewidth/(double)temp->texdatawidth;

  glColor4f(1,1,1,1);
  glBegin(GL_POLYGON);
      glTexCoord2d(0,yratio);
    glVertex2d(0,0);
      glTexCoord2d(0,0);
    glVertex2d(0,temp->imageheight*transform.getScaleY());
      glTexCoord2d(xratio,0);
    glVertex2d(temp->imagewidth*transform.getScaleX(),temp->imageheight*transform.getScaleY());
      glTexCoord2d(xratio,yratio);
    glVertex2d(temp->imagewidth*transform.getScaleX(),0);
    glEnd();

    glDisable(GL_TEXTURE_2D);

  if (bpp == 4) {
    glDisable(GL_BLEND);
  }

  glPopMatrix();

  globals->handlemutex.unlock();
}

int GLUTImp::ImageInstance::createImage(const int windowheight, const int x, const int y, const int width, const int height) {
  static GIIIGlobals globals;

  unsigned char *data = new unsigned char[(width * height * 4)+FUDGE];

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glReadBuffer(GL_BACK);
  glReadPixels(x, windowheight - y - height, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

  globals->handlemutex.lock();

  int newhandle =  globals->handlecount++;
  nonfatal_assert((newhandle == INT_MAX), "Did you really create that many images?  The Image handle counter has overflown.", __FILE__, __LINE__);

  globals->refcounts[newhandle] = 1;

  globals->handles[newhandle] = new ImageInstance(data, Image::SCREEN, width, height);

  if(globals->handles[newhandle] == NULL)
    fatal_exit("Could not allocate sufficent memory to create a new ImageInstance.  Free up some memory and try again", __FILE__, __LINE__);

  globals->handlemutex.unlock();

  return newhandle;
}

int GLUTImp::ImageInstance::subImage(const int srchandle, int x, int y, int width, int height) {
  static GIIIGlobals globals;

  int newhandle = globals->handlecount++;
  nonfatal_assert((newhandle == INT_MAX), "Did you really create that many images?  The Image handle counter has overflown.", __FILE__, __LINE__);
  globals->refcounts[newhandle] = 1;

  globals->handlemutex.lock();

  ImageInstance *srcimg = globals->handles[srchandle];
  int bpp = 0;
  if (srcimg->type == Image::JPEG) {
    bpp = 3;
  } else if (srcimg->type == Image::SCREEN || srcimg->type == Image::PNG) {
    bpp = 4;
  }

  unsigned char *subimg = new unsigned char[(width * height * bpp)+FUDGE];

  // jsaks dbg
  //subimg[width*height*bpp] = 'J';

  memset(subimg,0,width*height*bpp*sizeof(unsigned char));

  // go through and grab out the subimage, but don't overflow outside the source image
  // since GL represents things upside down, we have to actually start grabbing pixels
  // at the other end...
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      for (int k = 0; k < bpp; k++) {
        int xinsrc = x+i;
        //int yinsrc = (y+height-1)-j;
        int yinsrc = y+j;
        //int yindest = (height-1)-j;
        int yindest = j;
        if (xinsrc >= 0 && xinsrc < srcimg->imagewidth && yinsrc >= 0 && yinsrc < srcimg->imageheight) {
          subimg[yindest*width*bpp+i*bpp+k] = srcimg->data[yinsrc*srcimg->imagewidth*bpp+xinsrc*bpp+k];
        }
      }
    }
  }

  // jsaks dbg
  //fatal_assert(subimg[width*height*bpp] != 'J',"wrote out of bounds on subimg",__FILE__,__LINE__);

  globals->handles[newhandle] = new ImageInstance(subimg,srcimg->type,width,height);

  if(globals->handles[newhandle] == NULL)
    fatal_exit("Could not allocate sufficent memory to create a new ImageInstance.  Free up some memory and try again", __FILE__, __LINE__);

  globals->handlemutex.unlock();

  return newhandle;
}

int GLUTImp::ImageInstance::createEditableImage(int imagehandle) {
  static GIIIGlobals globals;

  int newhandle = globals->handlecount++;
  nonfatal_assert((newhandle == INT_MAX), "Did you really create that many images?  The Image handle counter has overflown.", __FILE__, __LINE__);
  globals->refcounts[newhandle] = 1;

  globals->handlemutex.lock();

  ImageInstance *srcimg = globals->handles[imagehandle];
  int bpp = 0;
  if (srcimg->type == Image::JPEG) {
    bpp = 3;
  } else if (srcimg->type == Image::SCREEN || srcimg->type == Image::PNG) {
    bpp = 4;
  }

  int height = srcimg->imageheight;
  int width = srcimg->imagewidth;

  unsigned char *img = new unsigned char[(width * height * 4)];

  memset(img,0xFF,width*height*4*sizeof(unsigned char));

  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      for (int k = 0; k < bpp; k++) {
        img[i*width*4+j*4+k] = srcimg->data[i*width*bpp+j*bpp+k];
      }
    }
  }

  globals->handles[newhandle] = new ImageInstance(img,Image::SCREEN,width,height);

  if(globals->handles[newhandle] == NULL)
    fatal_exit("Could not allocate sufficent memory to create a new ImageInstance.  Free up some memory and try again", __FILE__, __LINE__);

  globals->handlemutex.unlock();

  return newhandle;
}

int GLUTImp::ImageInstance::createEditableImage(int width, int height) {
  static GIIIGlobals globals;

  int newhandle = globals->handlecount++;
  nonfatal_assert((newhandle == INT_MAX), "Did you really create that many images?  The Image handle counter has overflown.", __FILE__, __LINE__);
  globals->refcounts[newhandle] = 1;

  globals->handlemutex.lock();

  unsigned char *img = new unsigned char[(width * height * 4)];

  memset(img,0xFF,width*height*4*sizeof(unsigned char));

  globals->handles[newhandle] = new ImageInstance(img,Image::SCREEN,width,height);

  if(globals->handles[newhandle] == NULL)
    fatal_exit("Could not allocate sufficent memory to create a new ImageInstance.  Free up some memory and try again", __FILE__, __LINE__);

  globals->handlemutex.unlock();

  return newhandle;
}

unsigned char *GLUTImp::ImageInstance::getImageData(int imagehandle) {
  static GIIIGlobals globals;

  return globals->handles[imagehandle]->data;
}

#ifdef HAVE_LIBJPEG
void GLUTImp::ImageInstance::jpegload(const char *cpFileName, unsigned short *uspWidth, unsigned short *uspHeight, unsigned char **ucpImageData) {

  fatal_assert((cpFileName == NULL),"Passed a null pointer for the filename", __FILE__, __LINE__);

  fatal_assert((ucpImageData == NULL),"Passed a null pointer for the image data", __FILE__, __LINE__);

  fatal_assert((uspWidth == NULL),"Passed a null pointer for the width", __FILE__, __LINE__);

  fatal_assert((uspHeight == NULL),"Passed a null pointer for the height", __FILE__, __LINE__);

  struct jpeg_decompress_struct jdsInfo;
  struct jpeg_error_mgr jemError;

  JSAMPARRAY jsaTmpBuffer;
  unsigned long ulOffset, ulDataWidth;
  unsigned short i;

  FILE *fInput;

  fInput = fopen(cpFileName, "rb");
  if(fInput == NULL) {
    fatal_exit("Can't open specified image", __FILE__, __LINE__);
  }

  jdsInfo.err = jpeg_std_error(&jemError);
  jpeg_create_decompress(&jdsInfo);
  jpeg_stdio_src(&jdsInfo, fInput);
  jpeg_read_header(&jdsInfo, TRUE);
  jpeg_start_decompress(&jdsInfo);

  *uspWidth = (unsigned short) jdsInfo.output_width;
  *uspHeight = (unsigned short) jdsInfo.output_height;
  ulDataWidth = jdsInfo.output_width * jdsInfo.output_components;

  *ucpImageData = new unsigned char[ulDataWidth * (*uspHeight)];
  if(ucpImageData == NULL) {
    fatal_exit("Allocate enough memory to open the specified image", __FILE__, __LINE__);
  }

  memset(*ucpImageData, static_cast<unsigned char>(0), ulDataWidth * (*uspHeight));
  jsaTmpBuffer = (*jdsInfo.mem->alloc_sarray) ((j_common_ptr) &jdsInfo, JPOOL_IMAGE, ulDataWidth, 1);

  for(i = 0, ulOffset = 0; i < (*uspHeight); i++) {
    ulOffset += ulDataWidth;
    jpeg_read_scanlines(&jdsInfo, jsaTmpBuffer, 1);
     memcpy((*ucpImageData + (ulDataWidth * (*uspHeight)) - ulOffset), jsaTmpBuffer[0], ulDataWidth);
  }

  jpeg_finish_decompress(&jdsInfo);

  fclose(fInput);
}
#endif //HAVE_LIBJPEG

#ifdef HAVE_LIBPNG
void GLUTImp::ImageInstance::pngload(const char *cpFileName, unsigned short *uspWidth, unsigned short *uspHeight, unsigned char **ucpImageData) {

  fatal_assert((cpFileName == NULL),"Passed a null pointer for the filename", __FILE__, __LINE__);

  fatal_assert((ucpImageData == NULL),"Passed a null pointer for the image data", __FILE__, __LINE__);

  fatal_assert((uspWidth == NULL),"Passed a null pointer for the width", __FILE__, __LINE__);

  fatal_assert((uspHeight == NULL),"Passed a null pointer for the height", __FILE__, __LINE__);

  int number = 0;
  png_byte header[8];

  FILE *fp = fopen(cpFileName, "rb");
    if(!fp)
      fatal_exit("Can't open the specified image", __FILE__, __LINE__);
    fread(header, 1, number, fp);
    int is_png = !png_sig_cmp(header, 0, number);
    if (!is_png) {
      fclose(fp);
    fatal_exit("Can't open the specified image", __FILE__, __LINE__);
    }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if(!png_ptr) {
    fclose(fp);
    fatal_exit("Error creating the specified image", __FILE__, __LINE__);
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if(!info_ptr) {
    png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
    fclose(fp);
    fatal_exit("Error creating the specified image", __FILE__, __LINE__);
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if(!end_info) {
    png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
    fclose(fp);
    fatal_exit("Error creating the specified image", __FILE__, __LINE__);
  }

  if(setjmp(png_ptr->jmpbuf)) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    fatal_exit("Error creating the specified image", __FILE__, __LINE__);
  }

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, number);

  png_read_info(png_ptr, info_ptr);

  png_uint_32 width, height, rowbytes;
  png_byte bit_depth, color_type;
  width = png_get_image_width(png_ptr, info_ptr);
  height = png_get_image_height(png_ptr, info_ptr);
  bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  color_type = png_get_color_type(png_ptr, info_ptr);

  if(color_type == PNG_COLOR_TYPE_PALETTE && bit_depth <= 8)
    png_set_palette_to_rgb(png_ptr);

  if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_gray_1_2_4_to_8(png_ptr);

  if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png_ptr);

  if(bit_depth == 16)
    png_set_strip_16(png_ptr);

  if(color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png_ptr);

  if(bit_depth == 8 && color_type == PNG_COLOR_TYPE_RGB)
    png_set_filler(png_ptr, 255, PNG_FILLER_AFTER);

  if(color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_filler(png_ptr, 255, PNG_FILLER_AFTER);

  png_read_update_info(png_ptr, info_ptr);

  rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  png_bytep* row_pointers = new png_bytep[height];
  if(row_pointers == NULL)
    fatal_exit("Error allocating memory for the specified image", __FILE__, __LINE__);

  for(png_uint_32 i = 0; i < height; i++) {
    row_pointers[i] = new png_byte[rowbytes];
    if(row_pointers[i] == NULL)
      fatal_exit("Error allocating memory for the specified image", __FILE__, __LINE__);
  }

  png_read_image(png_ptr, row_pointers);

  *ucpImageData = new unsigned char[rowbytes * height];
  if(ucpImageData == NULL) {
      fatal_exit("Error allocating memory for the specified image", __FILE__, __LINE__);
  }

  memset(*ucpImageData, static_cast<unsigned char>(0), rowbytes * height);

  png_uint_32 offset = 0;
  for(png_uint_32 j = 0; j < height; j++) {
    offset += rowbytes;
    memcpy((*ucpImageData + (rowbytes * height) - offset), row_pointers[j], rowbytes);
  }

  *uspWidth = width;
  *uspHeight = height;

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

  fclose(fp);

  for(png_uint_32 k = 0; k < height; k++) {
    delete []row_pointers[k];
  }
  delete [](row_pointers);
}
#endif //HAVE_LIBPNG

#ifdef HAVE_LIBJPEG
void GLUTImp::ImageInstance::jpegsave(const char *cpFileName, unsigned short usWidth, unsigned short usHeight, unsigned char *ucpImageData) {

  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;

  FILE * outfile;
  JSAMPROW row_pointer[1];
  int row_stride;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);

  if ((outfile = fopen(cpFileName, "wb")) == NULL) {
    fatal_exit("Error opening the specified file", __FILE__, __LINE__);
  }
  jpeg_stdio_dest(&cinfo, outfile);

  cinfo.image_width = usWidth;
  cinfo.image_height = usHeight;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;

  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, 100, TRUE);

  jpeg_start_compress(&cinfo, TRUE);

  row_stride = usWidth * 3;

  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = &ucpImageData[(usHeight * row_stride) - ((cinfo.next_scanline + 1) * row_stride)];
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  jpeg_finish_compress(&cinfo);
  fclose(outfile);
  jpeg_destroy_compress(&cinfo);
}
#endif //HAVE_LIBJPEG

#ifdef HAVE_LIBPNG
void GLUTImp::ImageInstance::pngsave(const char *cpFileName, unsigned short usWidth, unsigned short usHeight, unsigned char *ucpImageData) {

  fatal_assert((cpFileName == NULL),"Passed a null pointer for the filename", __FILE__, __LINE__);

  fatal_assert((ucpImageData == NULL),"Passed a null pointer for the image data", __FILE__, __LINE__);

  FILE *fp = fopen(cpFileName, "wb");
  if(!fp) {
    fatal_exit("Error opening the specified file", __FILE__, __LINE__);
  }

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp) NULL, NULL, NULL);
  if(!png_ptr) {
    fclose(fp);
    fatal_exit("Error opening writing the specified file", __FILE__, __LINE__);
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if(!info_ptr) {
    png_destroy_write_struct(&png_ptr, (png_infop *) NULL);
    fclose(fp);
    fatal_exit("Error opening writing the specified file", __FILE__, __LINE__);
  }

  if(setjmp(png_ptr->jmpbuf)) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    fatal_exit("Error opening writing the specified file", __FILE__, __LINE__);
  }

  png_init_io(png_ptr, fp);

  png_set_filter(png_ptr, 0, PNG_FILTER_NONE);

  png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

  png_set_IHDR(png_ptr, info_ptr, usWidth, usHeight, static_cast<png_byte>(8), PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  png_write_info(png_ptr, info_ptr);

  png_uint_32 rowbytes = usWidth * 4;

  png_bytep* row_pointers = new png_bytep[usHeight];
  if(row_pointers == NULL)
    fatal_exit("Error allocating memory for the specified image", __FILE__, __LINE__);
  for(png_uint_32 i = 0; i < usHeight; i++) {
    row_pointers[i] = new png_byte[rowbytes];
    if(row_pointers[i] == NULL)
      fatal_exit("Error allocating memory for the specified image", __FILE__, __LINE__);
    memset(row_pointers[i], static_cast<unsigned char>(0), rowbytes);
  }

  png_uint_32 offset = 0;
  for(png_uint_32 j = 0; j < usHeight; j++) {
    offset += rowbytes;
    memcpy(row_pointers[j], (ucpImageData + (rowbytes * usHeight) - offset), rowbytes);
  }

  png_write_image(png_ptr, row_pointers);

  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);

  fclose(fp);

  for(png_uint_32 k= 0; k < usHeight; k++) {
    delete [] (row_pointers[k]);
  }
  delete [] (row_pointers);
}
#endif //HAVE_LIBPNG

///////////////////////////////////////////////////////////////////////////////

void GLUTImp::setCallbacks() {
    glutDisplayFunc(GLUTImp::display);
    glutReshapeFunc(GLUTImp::reshape);
    glutKeyboardFunc(GLUTImp::keyboard);
    glutSpecialFunc(GLUTImp::special);
    glutMouseFunc(GLUTImp::mouse);
    glutMotionFunc(GLUTImp::motion);
    glutPassiveMotionFunc(GLUTImp::passivemotion);
    glutEntryFunc(GLUTImp::entry);
    glutIdleFunc(GLUTImp::idle);
    glutKeyboardUpFunc(GLUTImp::keyboardup);
    glutSpecialUpFunc(GLUTImp::specialup);
}

void GLUTImp::display() {
  static GIGlobals globals;

  if(globals->wid_to_flip[globals->gid_to_wid[glutGetWindow()]])
    glutSwapBuffers();

  if(glutLayerGet((GLenum) GLUT_NORMAL_DAMAGED))
    glutSwapBuffers();

  // Necessary for doing cooperative threading
  yield();
}

void GLUTImp::reshape(int width, int height) {
  static GIGlobals globals;

  ((Window*)globals->gid_to_wid[glutGetWindow()])->_setWindowSize(width, height);
  reshapeWindow(width, height);
  glutPostRedisplay();
}

KeyModifiers GLUTImp::getModifiers() {
  KeyModifiers temp = KeyModifiers::NO_MODIFIERS;
  int modifiers = glutGetModifiers();

  if((modifiers & GLUT_ACTIVE_SHIFT) == GLUT_ACTIVE_SHIFT)
    temp = temp | KeyModifiers::SHIFT;

  if((modifiers & GLUT_ACTIVE_CTRL) == GLUT_ACTIVE_CTRL)
    temp = temp | KeyModifiers::CONTROL;

  if((modifiers & GLUT_ACTIVE_ALT) == GLUT_ACTIVE_ALT)
    temp = temp | KeyModifiers::ALT;

  return temp;
}

void GLUTImp::keyboard(unsigned char key, int, int) {
  static GIGlobals globals;

  KeyModifiers temp = getModifiers();

  Window *w = ((Window*)globals->gid_to_wid[glutGetWindow()]);
  w->addKeyboardEvent(KeyboardEvent(key, temp));

  w->key_state[tolower(key)] = true;
  w->key_state[toupper(key)] = true;
  w->currentmodifiers = temp;

 // Necessary for doing cooperative threading
  yield();
}

void GLUTImp::keyboardup(unsigned char key, int x, int y) {
  static GIGlobals globals;

  KeyModifiers temp = getModifiers();

  Window *w = ((Window*)globals->gid_to_wid[glutGetWindow()]);
  w->key_state[tolower(key)] = false;
  w->key_state[toupper(key)] = false;
  w->currentmodifiers = temp;

  yield();
}

void GLUTImp::mouse(int button, int state, int x, int y) {
  static GIGlobals globals;

  KeyModifiers temp = getModifiers();

  MouseEvent::Button buttons = MouseEvent::NO_BUTTON;

  if(button == GLUT_LEFT_BUTTON)
    buttons = MouseEvent::LEFT_BUTTON;
  else if (button == GLUT_MIDDLE_BUTTON)
    buttons = MouseEvent::MIDDLE_BUTTON;
  else if (button == GLUT_RIGHT_BUTTON)
    buttons = MouseEvent::RIGHT_BUTTON;

  globals->lastbutton = buttons;

  Window *w = ((Window*)globals->gid_to_wid[glutGetWindow()]);

  if(state == GLUT_UP) {
    w->addMouseEvent(MouseEvent(MouseEvent::BUTTON_UP_EVENT, buttons, x, y, temp));
    w->addMouseEvent(MouseEvent(MouseEvent::BUTTON_CLICK_EVENT, buttons, x, y, temp));
    globals->lastbutton = MouseEvent::NO_BUTTON;
    w->button_state[buttons] = false;
  } else {
    w->addMouseEvent(MouseEvent(MouseEvent::BUTTON_DOWN_EVENT, buttons, x, y, temp));
    w->button_state[buttons] = true;
  }

 // Necessary for doing cooperative threading
  yield();
}

void GLUTImp::motion(int x, int y) {
  static GIGlobals globals;

  ((Window*)globals->gid_to_wid[glutGetWindow()])->addMouseEvent(
    MouseEvent(MouseEvent::MOUSE_MOVE_EVENT, globals->lastbutton, x, y,
               KeyModifiers::NO_MODIFIERS));

 // Necessary for doing cooperative threading
  yield();
}

void GLUTImp::passivemotion(int x, int y) {
  static GIGlobals globals;

  ((Window*)globals->gid_to_wid[glutGetWindow()])->addMouseEvent(
    MouseEvent(MouseEvent::MOUSE_MOVE_EVENT, MouseEvent::NO_BUTTON, x, y,
      KeyModifiers::NO_MODIFIERS));

 // Necessary for doing cooperative threading
  yield();
}

void GLUTImp::entry(int state) {
  static GIGlobals globals;

  if(state == GLUT_LEFT)
    ((Window*)globals->gid_to_wid[glutGetWindow()])->addMouseEvent(
      MouseEvent(MouseEvent::EXIT_WINDOW_EVENT, globals->lastbutton, -1, -1,
                 KeyModifiers::NO_MODIFIERS));
  else
    ((Window*)globals->gid_to_wid[glutGetWindow()])->addMouseEvent(
      MouseEvent(MouseEvent::ENTER_WINDOW_EVENT, globals->lastbutton, -1, -1,
                 KeyModifiers::NO_MODIFIERS));

 // Necessary for doing cooperative threading
  yield();
}

int GLUTImp::glutKeyToNamedKey(int key) {
  int newkey = 0;
  switch(key) {
   case GLUT_KEY_F1:
    newkey = NamedKey::FUNCTION_1;
    break;
   case GLUT_KEY_F2:
    newkey = NamedKey::FUNCTION_2;
    break;
   case GLUT_KEY_F3:
    newkey = NamedKey::FUNCTION_3;
    break;
   case GLUT_KEY_F4:
    newkey = NamedKey::FUNCTION_4;
    break;
   case GLUT_KEY_F5:
    newkey = NamedKey::FUNCTION_5;
    break;
   case GLUT_KEY_F6:
    newkey = NamedKey::FUNCTION_6;
    break;
   case GLUT_KEY_F7:
    newkey = NamedKey::FUNCTION_7;
    break;
   case GLUT_KEY_F8:
    newkey = NamedKey::FUNCTION_8;
    break;
   case GLUT_KEY_F9:
    newkey = NamedKey::FUNCTION_9;
    break;
   case GLUT_KEY_F10:
    newkey = NamedKey::FUNCTION_10;
    break;
   case GLUT_KEY_F11:
    newkey = NamedKey::FUNCTION_11;
    break;
   case GLUT_KEY_F12:
    newkey = NamedKey::FUNCTION_12;
    break;
   case GLUT_KEY_LEFT:
    newkey = NamedKey::LEFT_ARROW;
    break;
   case GLUT_KEY_UP:
    newkey = NamedKey::UP_ARROW;
    break;
   case GLUT_KEY_RIGHT:
    newkey = NamedKey::RIGHT_ARROW;
    break;
   case GLUT_KEY_DOWN:
    newkey = NamedKey::DOWN_ARROW;
    break;
   case GLUT_KEY_PAGE_UP:
    newkey = NamedKey::PAGE_UP;
    break;
   case GLUT_KEY_PAGE_DOWN:
    newkey = NamedKey::PAGE_DOWN;
    break;
   case GLUT_KEY_HOME:
    newkey = NamedKey::HOME;
    break;
   case GLUT_KEY_END:
    newkey = NamedKey::END;
    break;
   case GLUT_KEY_INSERT:
    newkey = NamedKey::INSERT;
    break;
  }

  return newkey;
}

void GLUTImp::special(int key, int, int) {
  static GIGlobals globals;

  KeyModifiers temp = getModifiers();

  int newkey = glutKeyToNamedKey(key);

  Window *w = ((Window*)globals->gid_to_wid[glutGetWindow()]);
  w->addKeyboardEvent(KeyboardEvent(newkey, temp));

  w->key_state[newkey] = true;
  w->currentmodifiers = temp;
 // Necessary for doing cooperative threading
  yield();
}

void GLUTImp::specialup(int key, int x, int y) {
  static GIGlobals globals;

  KeyModifiers temp = getModifiers();


  int newkey = glutKeyToNamedKey(key);

  Window *w = ((Window*)globals->gid_to_wid[glutGetWindow()]);

  w->key_state[newkey] = false;
  w->currentmodifiers = temp;

  yield();
}

void GLUTImp::idle() {
  deque<DaemonCommand*> frozen;
  commutex.lock();
  frozen = comqueue;
  comqueue.clear();
  commutex.unlock();

  for (deque<DaemonCommand*>::iterator iter = frozen.begin(); iter != frozen.end(); iter++) {
      DaemonCommand * temp = *iter;
      handleCommand(*temp);
      delete temp;
  }
  frozen.clear();

  yield();
}

void GLUTImp::timer(int timerid) {
  static GIGlobals globals;

  switch (globals->tid_to_type[timerid]) {
    case TIMER_ONCE:
      // if it happens once, report that it happened and stop it
      ((Window*)globals->tid_to_wid[timerid])->addTimerEvent(TimerEvent(timerid));
      globals->tid_to_type[timerid] = TIMER_STOPPED;
      // TODO: remove the state of the timer?
      break;
    case TIMER_REPEAT:
      {
      // if it's supposed to repeat, report it and restart it
      ((Window*)globals->tid_to_wid[timerid])->addTimerEvent(TimerEvent(timerid));
      int time = globals->tid_to_interval[timerid];
      glutTimerFunc(time, GLUTImp::timer, timerid);
      }
      break;
    case TIMER_STOPPED:
      // TODO: perhaps remove the state?
      break;
  };

  // Necessary for doing cooperative threading
  yield();
}

///////////////////////////////////////////////////////////////////////////////

THREAD_RETURN_TYPE GLUTImp::start_glut(void *) {

  bool more = true;
  bool processedCreate = false;
  do {
    commutex.lock();
    if (!comqueue.empty()) {
      DaemonCommand *temp = comqueue.front();
      comqueue.pop_front();
      if (temp->type() == DaemonCommand::CREATE_WINDOW) processedCreate = true;
      commutex.unlock();
      handleCommand(*temp);
      delete temp;
    } else {
      commutex.unlock();
      more = false;
    }

  } while (!processedCreate);

  glutMainLoop();

  return THREAD_RETURN_VAL;
}

///////////////////////////////////////////////////////////////////////////////

int GLUTImp::getFontHandle(const string face, const int pointsize) {
  return FontInstance::getFontHandle(face, pointsize);
}

int GLUTImp::getFontHandle(int fontID, const int pointsize) {
  return FontInstance::getFontHandle("",pointsize,fontID);
}

void GLUTImp::releaseFontHandle(const int fonthandle) {
  FontInstance::releaseFontHandle(fonthandle);
}

void GLUTImp::bumpFontRefCount(const int handle) {
  FontInstance::bumpFontRefCount(handle);
}

bool GLUTImp::checkFont(const string face, const int pointsize) {
  return FontInstance::checkFont(face, pointsize);
}

void GLUTImp::getStringSize(int fonthandle, const string &text, int &width, int &height) {
  FontInstance::getStringSize(fonthandle, text, width, height);
}

int GLUTImp::getImageHandle(const string file, const Image::Type type, int * imagewidth, int * imageheight) {
  unsigned short width, height;
  int handle;

  fatal_assert((imagewidth == NULL),"Passed a null pointer for the width", __FILE__, __LINE__);

  fatal_assert((imageheight == NULL),"Passed a null pointer for the height", __FILE__, __LINE__);

  handle = ImageInstance::getImageHandle(file, type, &width, &height);
  *imagewidth = static_cast<int>(width);
  *imageheight = static_cast<int>(height);
  return handle;
}

void GLUTImp::releaseImageHandle(const int imagehandle) {
  ImageInstance::releaseImageHandle(imagehandle);
}

void GLUTImp::bumpImageRefCount(const int handle) {
  ImageInstance::bumpImageRefCount(handle);
}

bool GLUTImp::checkImage(const string file, const Image::Type type) {
  return ImageInstance::checkImage(file, type);
}


void GLUTImp::saveImage(const int imagehandle, const string file, const Image::Type type) {
  ImageInstance::saveImage(imagehandle, file, type);
}

int GLUTImp::subImage(const int imagehandle, int x, int y, int w, int h) {
  return ImageInstance::subImage(imagehandle,x,y,w,h);
}

int GLUTImp::createEditableImage(int imagehandle) {
  return ImageInstance::createEditableImage(imagehandle);
}

int GLUTImp::createEditableImage(int w, int h) {
  return ImageInstance::createEditableImage(w,h);
}

unsigned char *GLUTImp::getImageData(int imagehandle) {
  return ImageInstance::getImageData(imagehandle);
}

void *GLUTImp::getImageInstance(int imagehandle) {
  return ImageInstance::getImageInstance(imagehandle);
}

int GLUTImp::registerNewTimerEvent(Window *window, const int milliseconds) {
  static GIGlobals globals;

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::CREATE_TIMER, window);

//  fatal_assert((temp == NULL), "Could not allocate sufficent memory to create a new command.  Free up some memory and try again", __FILE__, __LINE__);

  int timerid = globals->timercount++;

  temp->addArgument(milliseconds);
  temp->addArgument(timerid);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();

  return timerid;
}

int GLUTImp::startTimer(Window *window, int milliseconds) {
  static GIGlobals globals;

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::START_TIMER, window);

  int timerid = globals->timercount++;

  temp->addArgument(milliseconds);
  temp->addArgument(timerid);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();

  return timerid;
}

void GLUTImp::stopTimer(Window *window, int timerid) {

  fatal_assert((window == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  throttle();

  DaemonCommand *temp = new DaemonCommand(DaemonCommand::STOP_TIMER, window);

  temp->addArgument(timerid);

  commutex.lock();
  comqueue.push_back(temp);
  commutex.unlock();
}

///////////////////////////////////////////////////////////////////////////////

void GLUTImp::handleCommand(DaemonCommand &com) {

  Window *windptr = com.getWindow();
  Window::SafeWindowPtr wind = (Window::SafeWindowPtr)windptr;
  /*
  Style style;
  string stringarg;
  double **vertices = NULL;
  int window_id, width, height;
  int left, top, right, bottom;
  int start, end, start_x, start_y, end_x, end_y;
  int x, y, x1, y1, x2, y2, x3, y3, radius;
  double theta, scalex, scaley;
  int bitdepth,refreshrate,fullscreen;
  int handle, time, index;
  unsigned char *data;
  GLUtriangulatorObj* tesselator;
  Transform *transform=NULL;
  */
  static GIGlobals globals;

  fatal_assert(!wind, "I seem to have been passed a null Window pointer", __FILE__, __LINE__);

  switch(com.type()) {
   case DaemonCommand::CREATE_WINDOW:
    {
    string stringarg = com.getString();

    int xpos = com.removeArgument();
    int ypos = com.removeArgument();
    int width = com.removeArgument();
    int height = com.removeArgument();
    int fullscreen = com.removeArgument();
    int bitdepth = com.removeArgument();
    int refreshrate = com.removeArgument();
    int window_id;
    if (!fullscreen) {
//      cout << width << "," << height << endl;
      window_id = glutCreateWindow(stringarg.c_str());
      globals->wid_to_fullscreen[(Window::SafeWindowPtr)wind] = false;
    } else {
      window_id = enableFullscreen(width,height,bitdepth,refreshrate);
      globals->wid_to_fullscreen[(Window::SafeWindowPtr)wind] = true;
    }

    globals->wid_to_gid[(Window::SafeWindowPtr)wind] = window_id;
    globals->wid_to_alias[(Window::SafeWindowPtr)wind] = false;
    globals->wid_to_flip[(Window::SafeWindowPtr)wind] = true;
    globals->gid_to_wid[window_id] = (Window::SafeWindowPtr)wind;

    setCallbacks();

    glutReshapeWindow(width, height);
    if (xpos != INT_MAX && ypos != INT_MAX) glutPositionWindow(xpos, ypos);
    glutPostRedisplay();
    }
    break;

   case DaemonCommand::DESTROY_WINDOW:
    {
    // FIX, remove from maps?
    if (globals->wid_to_fullscreen[wind]) {
      disableFullscreen();
    }
    glutDestroyWindow(globals->wid_to_gid[wind]);
    }
    break;

   case DaemonCommand::OBSCURE_WINDOW:
    {
    glutSetWindow(globals->wid_to_gid[wind]);
    glutHideWindow();
    }
    break;

   case DaemonCommand::SHOW_WINDOW:
    {
    glutSetWindow(globals->wid_to_gid[wind]);
    glutShowWindow();
    }
    break;

   case DaemonCommand::RESIZE_WINDOW:
    {
    glutSetWindow(globals->wid_to_gid[wind]);
    int w = com.removeArgument();
    int h = com.removeArgument();
    // already reshapes the window and redisplays
    glutReshapeWindow(w, h);
    }
    break;

   case DaemonCommand::SET_WINDOW_POSITION:
    {
    glutSetWindow(globals->wid_to_gid[wind]);
    int x = com.removeArgument();
    int y = com.removeArgument();
    // already reshapes the window and redisplays
    glutPositionWindow(x, y);
    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::GET_WINDOW_POSITION:
    {
    glutSetWindow(globals->wid_to_gid[wind]);
    // already reshapes the window and redisplays
    int x = glutGet(GLUT_WINDOW_X);
    int y = glutGet(GLUT_WINDOW_Y);
    ((Window*)wind)->positionsignal->message = std::pair<int, int>(x, y);
    ((Window*)wind)->positionsignal->signal();
    }
    break;

   case DaemonCommand::SET_WINDOW_TITLE:
    {
    glutSetWindow(globals->wid_to_gid[wind]);
    std::string title = com.getString();
    // already reshapes the window and redisplays
    glutSetWindowTitle(title.c_str());
    }
    break;

   case DaemonCommand::ENABLE_ANTIALIASING:
    {
    globals->wid_to_alias[wind] = true;
    }
    break;

   case DaemonCommand::DISABLE_ANTIALIASING:
    {
    globals->wid_to_alias[wind] = false;
    }
    break;

   case DaemonCommand::ENABLE_FULLSCREEN:
    {
    globals->wid_to_fullscreen[wind] = true;
    int bitdepth = com.removeArgument();
    int refreshrate = com.removeArgument();
    enableFullscreen(windptr->width,windptr->height,bitdepth,refreshrate);
    }
    break;

   case DaemonCommand::DISABLE_FULLSCREEN:
    {
    globals->wid_to_fullscreen[wind] = false;
    disableFullscreen();
    }
    break;

   case DaemonCommand::ENABLE_AUTO_PAGE_FLIP:
    {
    globals->wid_to_flip[wind] = true;
    }
    break;

   case DaemonCommand::DISABLE_AUTO_PAGE_FLIP:
    {
    globals->wid_to_flip[wind] = false;
    }
    break;

   case DaemonCommand::FLIP_PAGE:
    {
    glutSetWindow(globals->wid_to_gid[wind]);
    glutSwapBuffers();
    glutPostRedisplay();
    }
    break;

   case DaemonCommand::CREATE_TIMER:
    {
    int time = com.removeArgument();
    int handle = com.removeArgument();
    globals->tid_to_wid[handle] = (Window::SafeWindowPtr)wind;
    globals->tid_to_type[handle] = TIMER_ONCE; // do it once
    globals->tid_to_interval[handle] = time;
    glutTimerFunc(time, GLUTImp::timer, handle);
    }
    break;

  case DaemonCommand::START_TIMER:
    {
    int time = com.removeArgument();
    int handle = com.removeArgument();
    globals->tid_to_wid[handle] = (Window::SafeWindowPtr)wind;
    globals->tid_to_type[handle] = TIMER_REPEAT; // do it forever
    globals->tid_to_interval[handle] = time;
    // start the timer going
    glutTimerFunc(time, GLUTImp::timer, handle);
    }
    break;

  case DaemonCommand::STOP_TIMER:
    {
    int handle = com.removeArgument();
    globals->tid_to_type[handle] = TIMER_STOPPED;
    }
    break;

   case DaemonCommand::DRAW_PIXEL:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int x = com.removeArgument(), y = com.removeArgument();

    glBegin(GL_POINTS);
      glVertex2i(x, y);
    glEnd();

    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_LINE:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int start_x = com.removeArgument(), start_y = com.removeArgument();
    int end_x = com.removeArgument(), end_y = com.removeArgument();

    glBegin(GL_LINES);
     glVertex2i(start_x, start_y); glVertex2i(end_x, end_y);
    glEnd();

    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
     glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_ARC:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int left = com.removeArgument();
    int top = com.removeArgument();
    int right = com.removeArgument();
    int bottom = com.removeArgument();
    int start = com.removeArgument();
    int end = com.removeArgument();

    glBegin(GL_LINE_STRIP);
      drawArcInternal(left, top, right, bottom, start, end);
    glEnd();

    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_RECTANGLE_FILLED:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int left = com.removeArgument();
    int top = com.removeArgument();
    int right = com.removeArgument();
    int bottom = com.removeArgument();

    glRecti(left, top, right, bottom);

    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_RECTANGLE_OUTLINE:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int left = com.removeArgument();
    int top = com.removeArgument();
    int right = com.removeArgument();
    int bottom = com.removeArgument();

    glBegin(GL_LINE_LOOP);
      glVertex2i(left, top); glVertex2i(right, top);
      glVertex2i(right, bottom); glVertex2i(left, bottom);
    glEnd();

    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_TRIANGLE_FILLED:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int x1 = com.removeArgument(), y1 = com.removeArgument();
    int x2 = com.removeArgument(), y2 = com.removeArgument();
    int x3 = com.removeArgument(), y3 = com.removeArgument();

    glBegin(GL_TRIANGLES);
      glVertex2i(x1, y1); glVertex2i(x2, y2); glVertex2i(x3, y3);
    glEnd();

    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_TRIANGLE_OUTLINE:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int x1 = com.removeArgument(), y1 = com.removeArgument();
    int x2 = com.removeArgument(), y2 = com.removeArgument();
    int x3 = com.removeArgument(), y3 = com.removeArgument();

    glBegin(GL_LINE_LOOP);
      glVertex2i(x1, y1); glVertex2i(x2, y2); glVertex2i(x3, y3);
    glEnd();

    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_CIRCLE_FILLED:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int x = com.removeArgument();
    int y = com.removeArgument();
    int radius = com.removeArgument();

    GLUquadric *quad = gluNewQuadric();
    glPushMatrix();
    glTranslated(x,y,0);
    gluQuadricDrawStyle(quad, GLU_FILL);
    gluDisk(quad, 0, radius, radius, 1);
    glPopMatrix();
    gluDeleteQuadric(quad);

    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_CIRCLE_OUTLINE:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int x = com.removeArgument();
    int y = com.removeArgument();
    int radius = com.removeArgument();

    GLUquadric *quad = gluNewQuadric();
    glPushMatrix();
    glTranslated(x,y,0);
    gluQuadricDrawStyle(quad, GLU_SILHOUETTE);
    gluDisk(quad, 0, radius, radius*10, 1);
    glPopMatrix();
    gluDeleteQuadric(quad);

    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_ELLIPSE_FILLED:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int left = com.removeArgument();
    int top = com.removeArgument();
    int right = com.removeArgument();
    int bottom = com.removeArgument();

    int x = (left+right)/2;
    int y = (top+bottom)/2;
    double xrad = (right-left)/2;
    double yrad = (bottom-top)/2;

    GLUquadric *quad = gluNewQuadric();
    glPushMatrix();
    glTranslated(x,y,0);
    glScalef(1.0,yrad/xrad,1);
    gluQuadricDrawStyle(quad, GLU_FILL);
    gluDisk(quad, 0.0, xrad, (int)(xrad+yrad), 1);
    glPopMatrix();
    gluDeleteQuadric(quad);

    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_ELLIPSE_OUTLINE:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int left = com.removeArgument();
    int top = com.removeArgument();
    int right = com.removeArgument();
    int bottom = com.removeArgument();

    int x = (left+right)/2;
    int y = (top+bottom)/2;
    double xrad = (right-left)/2;
    double yrad = (bottom-top)/2;

    GLUquadric *quad = gluNewQuadric();
    glPushMatrix();
    glTranslated(x,y,0);
    glScalef(1.0,yrad/xrad,1);
    gluQuadricDrawStyle(quad, GLU_SILHOUETTE);
    gluDisk(quad, 0.0, xrad, (int)((xrad+yrad)*5), 1);
    glPopMatrix();
    gluDeleteQuadric(quad);

    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_WEDGE_FILLED:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int left = com.removeArgument();
    int top = com.removeArgument();
    int right = com.removeArgument();
    int bottom = com.removeArgument();
    int start = com.removeArgument();
    int end = com.removeArgument();

    int x = (left+right)/2;
    int y = (top+bottom)/2;
    double xrad = (right-left)/2;
    double yrad = (bottom-top)/2;
    double startangle = ((double)start)/ARC_PRECISION;
    double endangle = ((double)end)/ARC_PRECISION;
    double sweepangle = (endangle-startangle);
    while (sweepangle < 0) sweepangle += 360.0;

    GLUquadric *quad = gluNewQuadric();
    glPushMatrix();
    glTranslated(x,y,0);
    glScalef(1.0,yrad/xrad,1);
    gluQuadricDrawStyle(quad, GLU_FILL);
    gluPartialDisk(quad, 0.0, xrad, (int)(xrad+yrad), 1, startangle, sweepangle);
    glPopMatrix();
    gluDeleteQuadric(quad);

/*
    glBegin(GL_POLYGON);
      glVertex2i((left + right) / 2, (top + bottom) / 2);
      drawArcInternal(left, top, right, bottom, start, end);
    glEnd();
*/
    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_WEDGE_OUTLINE:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int left = com.removeArgument();
    int top = com.removeArgument();
    int right = com.removeArgument();
    int bottom = com.removeArgument();
    int start = com.removeArgument();
    int end = com.removeArgument();

    int x = (left+right)/2;
    int y = (top+bottom)/2;
    double xrad = (right-left)/2;
    double yrad = (bottom-top)/2;
    double startangle = ((double)start)/ARC_PRECISION;
    double endangle = ((double)end)/ARC_PRECISION;
    double sweepangle = (endangle-startangle);
    while (sweepangle < 0) sweepangle += 360.0;

    GLUquadric *quad = gluNewQuadric();
    glPushMatrix();
    glTranslated(x,y,0);
    glScalef(1.0,yrad/xrad,1);
    gluQuadricDrawStyle(quad, GLU_SILHOUETTE);
    gluPartialDisk(quad, 0.0, xrad, (int)(xrad+yrad), 1, startangle, sweepangle);
    glPopMatrix();
    gluDeleteQuadric(quad);

    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_CHORD_FILLED:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int left = com.removeArgument();
    int top = com.removeArgument();
    int right = com.removeArgument();
    int bottom = com.removeArgument();
    int start = com.removeArgument();
    int end = com.removeArgument();

    glBegin(GL_POLYGON);
      drawArcInternal(left, top, right, bottom, start, end);
    glEnd();

    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_CHORD_OUTLINE:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int left = com.removeArgument();
    int top = com.removeArgument();
    int right = com.removeArgument();
    int bottom = com.removeArgument();
    int start = com.removeArgument();
    int end = com.removeArgument();

    glBegin(GL_LINE_LOOP);
      drawArcInternal(left, top, right, bottom, start, end);
    glEnd();

    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_TEXT:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    string stringarg = com.getString();
//    fatal_assert((stringarg == NULL), "I seem to have been passed a null string pointer", __FILE__, __LINE__);

    int x = com.removeArgument();
    int y = com.removeArgument();
    int handle = com.removeArgument();
    Transform *transform = NULL;
    if (com.countArguments() > 0) {
      double theta = com.removeArgument()/Transform::SCALE;
      double scalex = com.removeArgument()/Transform::SCALE;
      double scaley = com.removeArgument()/Transform::SCALE;
      transform = new Transform(theta,scalex,scaley);
    }

    FontInstance::drawText(handle, windptr, x, y, &stringarg,transform);

    disableStyle(wind, style);

    if (transform != NULL) SafeDelete(transform);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_POLYGON_FILLED:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int end = com.removeArgument();

    GLUtriangulatorObj *tesselator = gluNewTess();

    gluTessCallback(tesselator, (GLenum)(GLU_VERTEX), (void (CALLBACK*) ()) GLUTImp::handleTessVertex);
    gluTessCallback(tesselator, (GLenum)(GLU_ERROR), (void (CALLBACK*) ()) GLUTImp::handleTessError);
    gluTessCallback(tesselator, (GLenum)(GLU_BEGIN), (void (CALLBACK*) ()) GLUTImp::handleTessBegin);
    gluTessCallback(tesselator, (GLenum)(GLU_END), (void (CALLBACK*) ()) GLUTImp::handleTessEnd);

    /*
    double vertices[3];
    int index;

    gluBeginPolygon(tesselator);

      for(index = 0; index < end; index++) {
        int x = com.removeArgument();
        int y = com.removeArgument();
        vertices[0] = static_cast<double>(x);
        vertices[1] = static_cast<double>(y);
        vertices[2] = 0.0;
        gluTessVertex(tesselator, vertices, vertices);
      }

    gluEndPolygon(tesselator);

    gluDeleteTess(tesselator);

    */
    double **vertices = new double*[end];
    int index;

    gluBeginPolygon(tesselator);

      for(index = 0; index < end; index++) {
        int x = com.removeArgument();
        int y = com.removeArgument();
        vertices[index] = new double[3];
        vertices[index][0] = static_cast<double>(x);
        vertices[index][1] = static_cast<double>(y);
        vertices[index][2] = 0.0;
      }

      for(index = 0; index < end; index++) {
        gluTessVertex(tesselator, vertices[index], vertices[index]);
      }
    gluEndPolygon(tesselator);

    for(index = 0; index < end; index++) {
      delete [] (vertices[index]);
    }
    delete [] (vertices);

    gluDeleteTess(tesselator);

    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_POLYGON_OUTLINE:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int end = com.removeArgument();

    GLUtriangulatorObj *tesselator = gluNewTess();

    gluTessCallback(tesselator, (GLenum)(GLU_ERROR), (void (CALLBACK*) ()) GLUTImp::handleTessError);
    gluTessCallback(tesselator, (GLenum)(GLU_BEGIN), (void (CALLBACK*) ()) GLUTImp::handleTessBegin);
    gluTessCallback(tesselator, (GLenum)(GLU_END), (void (CALLBACK*) ()) GLUTImp::handleTessEnd);

    double **vertices = new double*[end];
    int index;

    bool tessfail = false;

    gluBeginPolygon(tesselator);

      for(index = 0; index < end; index++) {
        int x = com.removeArgument();
        int y = com.removeArgument();
        vertices[index] = new double[3];
        vertices[index][0] = static_cast<double>(x);
        vertices[index][1] = static_cast<double>(y);
        vertices[index][2] = 0.0;
      }

      for(index = 0; index < end; index++) {
        gluTessVertex(tesselator, vertices[index], NULL);
      }
    gluEndPolygon(tesselator);

    gluDeleteTess(tesselator);

    if(tessfail != true) {
      glBegin(GL_LINE_LOOP);
        for(index = 0; index < end; index++) {
          glVertex2dv(vertices[index]);
        }
      glEnd();
    }

    for(index = 0; index < end ; index++) {
      delete [] (vertices[index]);
    }
    delete [] (vertices);

    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_POLYLINE:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int end = com.removeArgument();
    int index;

    glBegin(GL_LINE_STRIP);
      for(index = 0; index < end; index++) {
        int x = com.removeArgument();
        int y = com.removeArgument();
        glVertex2i(x, y);
      }
    glEnd();

    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_BEZIER_CURVE:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    Style style(com.getStyle());
    enableStyle(wind, style);

    int end = com.removeArgument();

    double **vertices = new double*;
    *vertices = new double[end * 3];

    int width = 0;
    int index;

    for(index = 0; index < end; index++) {
      int x = com.removeArgument();
      int y = com.removeArgument();
      vertices[0][index * 3 + 0] = static_cast<double>(x);
      vertices[0][index * 3 + 1] = static_cast<double>(y);
      vertices[0][index * 3 + 2] = 0.0;
      if(index != 0) {
        x = static_cast<int>(vertices[0][index * 3 + 0] - vertices[0][(index - 1) * 3 + 0]);
        y = static_cast<int>(vertices[0][index * 3 + 1] - vertices[0][(index - 1) * 3 + 1]);
        int height = static_cast<int>(sqrt(static_cast<double>(x*x + y*y)));
        width += height;
      }
    }

    glEnable(GL_MAP1_VERTEX_3);
    glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, end, vertices[0]);
    glMapGrid1d(width, 0.0, 1.0);

    glEvalMesh1(GL_LINE, 0, width);

    glDisable(GL_MAP1_VERTEX_3);

    delete [] (vertices[0]);

    delete (vertices);

    disableStyle(wind, style);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::DRAW_IMAGE:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    int handle = com.removeArgument();
    int x = com.removeArgument();
    int y = com.removeArgument();

    ImageInstance::drawImage(handle, x, y);

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    Daemon::releaseImageHandle(handle);
    }
    break;

   case DaemonCommand::DRAW_IMAGE_TRANSFORMED:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    int handle = com.removeArgument();
    int x = com.removeArgument();
    int y = com.removeArgument();
    double theta = com.removeArgument()/Transform::SCALE;
    double scalex = com.removeArgument()/Transform::SCALE;
    double scaley = com.removeArgument()/Transform::SCALE;

    ImageInstance::drawImage(handle, x, y,Transform(theta,scalex,scaley));

    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    Daemon::releaseImageHandle(handle);
    }
    break;

   case DaemonCommand::CREATE_IMAGE:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    int x = com.removeArgument();
    int y = com.removeArgument();
    int width = com.removeArgument();
    int height = com.removeArgument();

    windptr->imagesignal->message = Image(ImageInstance::createImage(windptr->height, x, y, width, height), width, height, Image::SCREEN);
    windptr->imagesignal->signal();
    }
    break;

   case DaemonCommand::COPY_REGION:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    int x1 = com.removeArgument();
    int y1 = com.removeArgument();
    int x2 = com.removeArgument();
    int y2 = com.removeArgument();
    int width = com.removeArgument();
    int height = com.removeArgument();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);
    glRasterPos2i(x2, y2 + height);
    glCopyPixels(x1, windptr->height - y1 - height, width, height, GL_COLOR);


    if(globals->wid_to_flip[wind]) {
      glutPostRedisplay();
    }
    }
    break;

   case DaemonCommand::READ_PIXEL:
    {
    glutSetWindow(globals->wid_to_gid[wind]);

    int x = com.removeArgument();
    int y = com.removeArgument();

    unsigned char data[4];

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);
    glReadPixels(x, windptr->height - y - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

    windptr->colorsignal->message = Color(static_cast<int>(data[0]),
                                      static_cast<int>(data[1]),
                                      static_cast<int>(data[2]),
                                      static_cast<int>(data[3]));

    // let the requester know that we've processed their request
    windptr->colorsignal->signal();
    }
    break;
   case DaemonCommand::IGNORE_KEY_REPEAT:
   {
     glutSetWindow(globals->wid_to_gid[wind]);

     int ignore = com.removeArgument();

     glutIgnoreKeyRepeat(ignore);
   }
   default:
      warning("Renderer recieved a command that it could not process", __FILE__, __LINE__);
    break;
  }
}

void CALLBACK GLUTImp::handleTessVertex(double *vertex) {
  fatal_assert(vertex == NULL, "I seem to have recieved a NULL vertex pointer", __FILE__, __LINE__);
  glVertex2dv((GLdouble *) vertex);
}

void CALLBACK GLUTImp::handleTessBegin(int mode) {
  glBegin((GLenum) mode);
}

void CALLBACK GLUTImp::handleTessEnd() {
  glEnd();
}

void GLUTImp::setColor(const Color &color) {
  glColor4ub(static_cast<unsigned char>(color.getRed()),
             static_cast<unsigned char>(color.getGreen()),
             static_cast<unsigned char>(color.getBlue()),
             static_cast<unsigned char>(color.getAlpha()));
}

void CALLBACK GLUTImp::handleTessError(int error) {
  switch((GLenum) error) {
    case GLU_TESS_ERROR4:
      // vertex/edge interesection
      warning("Error tesselating polygon, a vertex edge intersection was found", __FILE__, __LINE__);
      break;

    case GLU_TESS_ERROR5:
      // misoriented or self-intersecting loop
      warning("Error tesselating polygon, a misoriented or self-intersecting loop was found", __FILE__, __LINE__);
      break;

    case GLU_TESS_ERROR6:
      // coincident vertices
      warning("Error tesselating polygon, coincident vertices were found", __FILE__, __LINE__);
      break;

    case GLU_TESS_ERROR7:
      // all vertices colinear
      warning("Error tesselating polygon, a vertices in you polygon are colinear", __FILE__, __LINE__);
      break;

    case GLU_TESS_ERROR8:
      // interesecting eges
      warning("Error tesselating polygon, some of your edges have been found to interesect", __FILE__, __LINE__);
      break;

    default:
      warning("Error tesselating polygon, unknown cause", __FILE__, __LINE__);
      // unknown
  }
  tessfail = true;
}

void GLUTImp::reshapeWindow(int width, int height) {
  glViewport(0,0, width, height);
  glLoadIdentity();
  gluOrtho2D(0, width, 0, height);
  glTranslatef(0, height, 0.0);
  glScalef(1.0, -1.0, 1.0);
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glutSwapBuffers();
}

void GLUTImp::enableStyle(Window::SafeWindowPtr wind, const Style &style) {
  static GIGlobals globals;

  fatal_assert(!wind, "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  Color color = style.getColor();

  if(style.getRasterOp() != Style::COPY_OP) {
    enableLogicalOp(style.getRasterOp());
  } else if(globals->wid_to_alias[wind]) {
    enableAntialias();
  }

  if(style.pattern != 65535) {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, style.pattern);
  }

  if(color.getAlpha() != 255) {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
  }

  setColor(color);
  glLineWidth(static_cast<float>(style.getLineWidth()));
}

void GLUTImp::disableStyle(Window::SafeWindowPtr wind, const Style &style) {
  static GIGlobals globals;

  fatal_assert(!wind, "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);

  Color color = style.getColor();

  if(color.getAlpha() != 255) {
    glDisable(GL_BLEND);
  }

  if(style.pattern != 65535) {
    glDisable(GL_LINE_STIPPLE);
  }

  if(style.getRasterOp() != Style::COPY_OP) {
    disableLogicalOp();
  } else if(globals->wid_to_alias[wind]) {
    disableAntialias();
  }

}

void GLUTImp::enableLogicalOp(Style::RasterOp rop) {
  switch(rop) {
   case Style::COPY_OP:
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_COPY);
    break;

   case Style::INVERT_OP:
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_INVERT);
    break;

   case Style::XOR_OP:
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_XOR);
    break;

   case Style::AND_OP:
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_AND);
    break;

   case Style::OR_OP:
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_OR);
    break;
  }
}

void GLUTImp::disableLogicalOp() {
  glDisable(GL_COLOR_LOGIC_OP);
  glLogicOp(GL_COPY);
}

void GLUTImp::enableAntialias() {
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
}

void GLUTImp::disableAntialias() {
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_BLEND);
}

int GLUTImp::enableFullscreen(int width,int height,int bitdepth,int refreshrate) {
  // is this really necessary?
  char temp[1000];
  sprintf(temp,"%dx%d:%d@%d",width,height,bitdepth,refreshrate);
  glutGameModeString(temp);
  return glutEnterGameMode();
}

void GLUTImp::disableFullscreen() {
  glutLeaveGameMode();
}

void GLUTImp::drawEllipseInternal(int x1, int y1, int x2, int y2) {

  int WorkingX, WorkingY;
  long Threshold;

  long XAdjust, YAdjust;
  int index, size, dx, dy;

  deque<int> xpointlist;
  deque<int> ypointlist;
  stack<int> tx, ty;

  int X = (x1 + x2) / 2;
  int Y = (y1 + y2) / 2;
  int A = abs(X - x1);
  int B = abs(Y - y1);

  long ASquared = static_cast<long>(A * A);
  long BSquared = static_cast<long>(B * B);

  WorkingX = 0;
  WorkingY = B;
  XAdjust = 0;
  YAdjust = ASquared * 2 * B;
  Threshold = ASquared / 4 - ASquared * B;

  for(;;) {
    Threshold += XAdjust + BSquared;
    if(Threshold >= 0) {
      YAdjust -= ASquared * 2;
      Threshold -= YAdjust;
      WorkingY--;
    }
    XAdjust += BSquared * 2;
    WorkingX++;
    if(XAdjust >= YAdjust)
      break;
    xpointlist.push_back(WorkingX);
    ypointlist.push_back(WorkingY);
  }

  WorkingX = A;
  WorkingY = 0;
  XAdjust = BSquared * 2 * A;
  YAdjust = 0;
  Threshold = BSquared / 4 - BSquared * A;

  for(;;) {
    Threshold += YAdjust + ASquared;
    if(Threshold >= 0) {
      XAdjust -= BSquared * 2;
      Threshold -= XAdjust;
      WorkingX--;
    }
    YAdjust += ASquared * 2;
    WorkingY++;
    if(YAdjust >= XAdjust)
      break;
    tx.push(WorkingX);
    ty.push(WorkingY);
  }

  while(!tx.empty()) {
    xpointlist.push_back(tx.top());
    tx.pop();
    ypointlist.push_back(ty.top());
    ty.pop();
  }


  index = 0;
  size = xpointlist.size();
  while(index < size) {
    dx = xpointlist.front();
    xpointlist.pop_front();

    dy = ypointlist.front();
    ypointlist.pop_front();

    glVertex2i(X - dx, Y - dy);
    xpointlist.push_back(dx);
    ypointlist.push_back(dy);
    index++;
  }

  index = 0;
  size = xpointlist.size();
  while(index < size) {
    dx = xpointlist.back();
    xpointlist.pop_back();

    dy = ypointlist.back();
    ypointlist.pop_back();

    glVertex2i(X - dx, Y + dy);
    xpointlist.push_front(dx);
    ypointlist.push_front(dy);
    index++;
  }

  index = 0;
  size = xpointlist.size();
  while(index < size) {
    dx = xpointlist.front();
    xpointlist.pop_front();

    dy = ypointlist.front();
    ypointlist.pop_front();

    glVertex2i(X + dx, Y + dy);
    index++;
    xpointlist.push_back(dx);
    ypointlist.push_back(dy);
  }

  index = 0;
  size = xpointlist.size();
  while(index < size) {
    dx = xpointlist.back();
    xpointlist.pop_back();

    dy = ypointlist.back();
    ypointlist.pop_back();

    glVertex2i(X + dx, Y - dy);
    xpointlist.push_front(dx);
    ypointlist.push_front(dy);
    index++;
  }

}

void GLUTImp::drawArcInternal(int x1, int y1, int x2, int y2, int start, int end) {

  deque<int> xpointlist;
  deque<int> ypointlist;
  stack<int> tx, ty;

  long XAdjust, YAdjust;
  int index, size;
  int dx, dy;

  int WorkingX, WorkingY;
  long Threshold;

  int startquad, endquad;
  float startrad, endrad;

  double newstart = static_cast<double>(start) / ARC_PRECISION;
  double newend = static_cast<double>(end) / ARC_PRECISION;

  bool active = false;
  bool ended = false;

  int X = (x1 + x2) / 2;
  int Y = (y1 + y2) / 2;
  int A = abs(X - x1);
  int B = abs(Y - y1);

  // Adjust to match screen
  newstart -= 90.0;
  newend -= 90.0;

  newstart = fmod(newstart, 360.0);
  newend = fmod(newend, 360.0);

  if(newstart < 0.0) newstart += 360.0;
  if(newend < 0.0) newend += 360.0;

  startrad = tan((static_cast<float>(newstart) * 3.1415926) / (180.0));

  endrad = tan((static_cast<float>(newend) * 3.1415926) / (180.0));

  startquad = 0;
  if((newstart >= 0.0) && (newstart < 90.0)) startquad = 0;
  else if((newstart >= 90.0) && (newstart < 180.0)) startquad = 1;
  else if((newstart >= 180.0) && (newstart < 270.0)) startquad = 2;
  else if((newstart >= 270.0) && (newstart < 360.0)) startquad = 3;

  endquad = 3;
  if((newend >= 0.0) && (newend < 90.0)) endquad = 0;
  else if((newend >= 90.0) && (newend < 180.0)) endquad = 1;
  else if((newend >= 180.0) && (newend < 270.0)) endquad = 2;
  else if((newend >= 270.0) && (newend < 360.0)) endquad = 3;

  long ASquared = static_cast<long>(A * A);
  long BSquared = static_cast<long>(B * B);

  WorkingX = 0;
  WorkingY = B;
  XAdjust = 0;
  YAdjust = ASquared * 2 * B;
  Threshold = ASquared / 4 - ASquared * B;

  xpointlist.push_back(WorkingX);
  ypointlist.push_back(WorkingY);

  for(;;) {
    Threshold += XAdjust + BSquared;
    if(Threshold >= 0) {
      YAdjust -= ASquared * 2;
      Threshold -= YAdjust;
      WorkingY--;
    }
    XAdjust += BSquared * 2;
    WorkingX++;
    if(XAdjust >= YAdjust)
      break;
    xpointlist.push_back(WorkingX);
    ypointlist.push_back(WorkingY);
  }

  xpointlist.push_back(WorkingX);
  ypointlist.push_back(WorkingY);

  WorkingX = A;
  WorkingY = 0;
  XAdjust = BSquared * 2 * A;
  YAdjust = 0;
  Threshold = BSquared / 4 - BSquared * A;

  tx.push(WorkingX);
  ty.push(WorkingY);

  for(;;) {
    Threshold += YAdjust + ASquared;
    if(Threshold >= 0) {
      XAdjust -= BSquared * 2;
      Threshold -= XAdjust;
      WorkingX--;
    }
    YAdjust += ASquared * 2;
    WorkingY++;
    if(YAdjust >= XAdjust)
      break;
    tx.push(WorkingX);
    ty.push(WorkingY);
  }

  tx.push(WorkingX);
  ty.push(WorkingY);

  while(!tx.empty()) {
    xpointlist.push_back(tx.top());
    tx.pop();
    ypointlist.push_back(ty.top());
    ty.pop();
  }


  for(;;) {

    if((newstart == 0.0) && (ended == false))
      active = true;

    if((newend == 0.0) && (active == true)) {
      active = false;
      ended = true;
    }

    index = 0;
    size = xpointlist.size();
    while(index < size) {
      dx = xpointlist.front();
      xpointlist.pop_front();
      dy = ypointlist.front();
      ypointlist.pop_front();

      if((startquad == 0) && (ended == false))
        if(static_cast<float>(dx) > (startrad * static_cast<float>(dy)))
          active = true;

      if(active)
        glVertex2i(X - dx, Y - dy);

      if((endquad == 0) && (active == true))
        if(static_cast<float>(dx) > (endrad * static_cast<float>(dy))) {
           active = false;
           ended = true;
        }

      xpointlist.push_back(dx);
      ypointlist.push_back(dy);
      index++;
    }

    if(((newstart > 89.0) && (newstart < 91.0)) && (ended == false))
      active = true;

    if(((newend > 89.0) && (newend < 91.0))&& (active == true)) {
      active = false;
      ended = true;
    }

    if(ended == true)
      break;

    index = 0;
    size = xpointlist.size();
    while(index < size) {
      dx = xpointlist.back();
      xpointlist.pop_back();
      dy = ypointlist.back();
      ypointlist.pop_back();

      if((startquad == 1) && (ended == false))
        if(static_cast<float>(dx) < (startrad * static_cast<float>(-dy)))
          active = true;

      if(active)
        glVertex2i(X - dx, Y + dy);

      if((endquad == 1) && (active == true))
        if(static_cast<float>(dx) < (endrad * static_cast<float>(-dy))) {
          active = false;
          ended = true;
  }

      xpointlist.push_front(dx);
      ypointlist.push_front(dy);
      index++;
    }

    if((newstart == 180.0) && (ended == false))
      active = true;

    if((newend == 180.0) && (active == true)) {
      active = false;
      ended = true;
    }

    if(ended == true)
      break;

    index = 0;
    size = xpointlist.size();
    while(index < size) {
      dx = xpointlist.front();
      xpointlist.pop_front();
      dy = ypointlist.front();
      ypointlist.pop_front();

      if((startquad == 2) && (ended == false))
        if(static_cast<float>(-dx) < (startrad * static_cast<float>(-dy)))
          active = true;

      if(active)
        glVertex2i(X + dx, Y + dy);


      if((endquad == 2) && (active == true))
        if(static_cast<float>(-dx) < (endrad * static_cast<float>(-dy))) {
          active = false;
          ended = true;
  }

      index++;
      xpointlist.push_back(dx);
      ypointlist.push_back(dy);
    }

    if(((newstart > 269.0) && (newstart < 271.0))&& (ended == false))
      active = true;

    if(((newend > 269.0) && (newend < 271.0)) && (active == true)) {
      active = false;
      ended = true;
    }

    if(ended == true)
      break;

    index = 0;
    size = xpointlist.size();
    while(index < size) {
      dx = xpointlist.back();
      xpointlist.pop_back();
      dy = ypointlist.back();
      ypointlist.pop_back();

      if((startquad == 3) && (ended == false))
        if(static_cast<float>(-dx) > (startrad * static_cast<float>(dy)))
          active = true;

      if(active)
        glVertex2i(X + dx, Y - dy);

     if((endquad == 3) && (active == true))
        if(static_cast<float>(-dx) > (endrad * static_cast<float>(dy))) {

          active = false;
          ended = true;
  }

      xpointlist.push_front(dx);
      ypointlist.push_front(dy);
      index++;
    }

    if(ended == true)
      break;
  }
}

