/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: Image.cpp,v 1.3 2002/10/31 20:56:50 jsaks Exp $
*/

#include <CarnegieMellonGraphics.h>
#include <Daemon.h>
#include <Debugging.h>

Image::Image(const string &file, const Type type) : type(type) {
  if(file.length() == 0)
      fatal_exit("\"\" is an invalid filename for an image", __FILE__, __LINE__);
  imagehandle = Daemon::getImageHandle(file, type, &width, &height);
}

Image::Image(int handle, int width, int height, Type type) : width(width), height(height), imagehandle(handle), type(type) {

}

Image::Image(const Image& image) : width(image.width), height(image.height), imagehandle(image.imagehandle), type(image.type) {
  if (imagehandle != -1) Daemon::bumpImageRefCount(imagehandle);
}

Image::Image() : width(0), height(0), imagehandle(-1), type(Image::NO_TYPE) {

}

Image& Image::operator=(const Image &rhs) {
  if (rhs.imagehandle == -1) return *this;
  Daemon::releaseImageHandle(imagehandle);
  imagehandle = rhs.imagehandle;
  Daemon::bumpImageRefCount(imagehandle);
  width = rhs.width;
  height = rhs.height;
  type = rhs.type;
  return *this;
}

Image::~Image() {
  if (imagehandle != -1) {
    Daemon::releaseImageHandle(imagehandle);
  }
}

bool Image::operator==(const Image& rhs) const {
  return ((imagehandle == rhs.imagehandle) && (width == rhs.width) && (height == rhs.height) && (type == rhs.type));
}

bool Image::operator!=(const Image& rhs) const {
  return !(*this == rhs);
}

void Image::save(const string &file, Type type) const {
  if (imagehandle == -1) return;
  if(file.length() == 0)
      fatal_exit("\"\" is an invalid filename for an image", __FILE__, __LINE__);
  Daemon::saveImage(imagehandle, file, type);
}

int Image::getWidth() const {
  return width;
}

int Image::getHeight() const {
  return height;
}

Image::Type Image::getType() const {
  return type;
}

ostream& operator<<(ostream& s, const Image& image) {
  string type;
  switch(image.getType()) {
   case Image::JPEG:
    type = "JPEG";
    break;
   case Image::PNG:
    type =  "PNG";
    break;
   case Image::SCREEN:
    type = "SCREEN";
    break;   
   case Image::NO_TYPE:
    type = "NOT INITIALIZED";
    break;
  }

  return s << "Image(" << image.getWidth() << ", " << image.getHeight() << ", " << type << ")";
}

bool Image::checkImage(const string &file, const Type type) {
  return Daemon::checkImage(file, type);
}

Image Image::subImage(int x,int y,int w,int h) {
  if (imagehandle == -1) return Image(*this);
  int temphandle = Daemon::subImage(imagehandle,x,y,w,h);
  return Image(temphandle,w,h,type);
}
