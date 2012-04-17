#include "CarnegieMellonGraphics.h"
#include <Daemon.h>
#include <GLUTImp.h>

EditableImage::EditableImage(const Image &image) {
  width = image.getWidth();
  height = image.getHeight();
  type = Image::SCREEN;
  imagehandle = Daemon::createEditableImage(image.imagehandle);
  data = Daemon::getImageData(imagehandle);
  editable = true;
  imageinstance = Daemon::getImageInstance(imagehandle);
}

EditableImage::EditableImage(const EditableImage &image) : Image(image) {
  editable = image.editable;
  // copied the image, now need to retrieve a quick-access handle to the data
  data = Daemon::getImageData(imagehandle);
  imageinstance = Daemon::getImageInstance(imagehandle);
}

EditableImage::EditableImage(int w, int h) : Image() {
  width = w;
  height = h;
  type = Image::SCREEN;
  imagehandle = Daemon::createEditableImage(width, height);
  data = Daemon::getImageData(imagehandle);
  editable = true;
  imageinstance = Daemon::getImageInstance(imagehandle);
}

EditableImage &EditableImage::operator=(const EditableImage &rhs) {
  if (rhs.imagehandle == -1) return *this;
  Daemon::releaseImageHandle(imagehandle);
  imagehandle = rhs.imagehandle;
  Daemon::bumpImageRefCount(imagehandle);
  width = rhs.width;
  height = rhs.height;
  type = rhs.type;
  data = rhs.data;
  editable = rhs.editable;
  imageinstance = rhs.imageinstance;
  return *this;
}

EditableImage::~EditableImage() {
  if (imagehandle != -1) {
    Daemon::releaseImageHandle(imagehandle);
  }
}

Color EditableImage::getPixel(int x, int y) {
  if (x < 0 || x >= width || y < 0 || y >= width) return Color(0,0,0,0);
  unsigned char *pixel = (data+(y*width*4)+x*4);
  return Color(pixel[0], pixel[1], pixel[2], pixel[3]);
}

void EditableImage::setPixel(int x, int y, const Color &c) {
  setPixel(x, y, c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha());
}

void EditableImage::setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
  if (x < 0 || x >= width || y < 0 || y >= height) return;
  unsigned int p = ((int)r<<24)|((int)g<<16)|((int)b<<8)|a;
  unsigned int *pixel = (unsigned int*)(data+(y*width+x)*4);
  *pixel = p;

  GLUTImp::ImageInstance *img = (GLUTImp::ImageInstance*)imageinstance;
  if (img->texdata) {
    pixel = (unsigned int*)(img->texdata + (y*img->texdatawidth + x)*4);
    *pixel = p;
  }
}

EditableImage EditableImage::clone() {
  return clone(imagehandle, width, height);
}

EditableImage EditableImage::clone(int imagehandle, int width, int height) {
  return EditableImage(imagehandle, width, height);
}

EditableImage::EditableImage(int imagehandle, int w, int h) {
  width = w;
  height = h;
  type = Image::SCREEN;
  imagehandle = Daemon::createEditableImage(imagehandle);
  editable = true;
}
