/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: Font.cpp,v 1.2 2002/10/23 18:50:27 jsaks Exp $
*/

#include <iostream>
#include <CarnegieMellonGraphics.h>
#include <Daemon.h>
#include <Debugging.h>

using namespace std;

const int Font::ROMAN = 1;
const int Font::MONO_ROMAN = 2;
const int Font::HELVETICA = 3;
const int Font::TIMES = 4;



Font::Font(const string &face, const int pointsize) : name(face), 
 size(pointsize) {

  if(face.length() == 0)
    fatal_exit("\"\" is an invalid filename for a font", __FILE__, __LINE__);
  if(size < 1)
    fatal_exit("Font pointsizes must be positive integers", __FILE__, __LINE__);

  fonthandle = Daemon::getFontHandle(name, size);
}

Font::Font(int fontID, int pointsize) {
    size = pointsize;
//    char temp[100];
//    sprintf(temp,
    name = "default font #" + fontID;
    fonthandle = Daemon::getFontHandle(fontID,size);
}


Font::~Font() {
  Daemon::releaseFontHandle(fonthandle);
}

Font::Font(const Font& font) : name(font.name), size(font.size), fonthandle(font.fonthandle) {
  Daemon::bumpFontRefCount(fonthandle);
}

Font& Font::operator=(const Font &rhs) {
  Daemon::releaseFontHandle(fonthandle);
  fonthandle = rhs.fonthandle;
  Daemon::bumpFontRefCount(fonthandle);
  name = rhs.name;
  size = rhs.size;
  return *this;
}

bool Font::operator==(const Font& rhs) const {
  return ((name == rhs.name) && (size == rhs.size));
}

bool Font::operator!=(const Font& rhs) const {
  return !(*this == rhs);
}

string Font::getFace() const { return name; }
int Font::getPointSize() const { return size; }

void Font::setFace(const string &face) {

  if(face != name) {
    name = face;
 
    if(name.length() == 0)
      fatal_exit("\"\" is an invalid filename for a font", __FILE__, __LINE__);

    Daemon::releaseFontHandle(fonthandle);
    fonthandle = Daemon::getFontHandle(name, size);
  }
}

void Font::setPointSize(const int pointsize) {

  if(pointsize != size) {
    size = pointsize;
  
    if(size < 1)
      fatal_exit("Font pointsizes must be positive integers", __FILE__, __LINE__);

    Daemon::releaseFontHandle(fonthandle);
    fonthandle = Daemon::getFontHandle(name, size);
  }
}

bool Font::checkFont(const string &face, int pointsize) {
  if(pointsize <= 0) {
    return false;
  } else {
    return Daemon::checkFont(face, pointsize);
  }
}

void Font::getStringSize(const string &text, int &width, int &height) {
  Daemon::getStringSize(fonthandle, text, width, height);
}

ostream& operator<<(ostream& s, const Font& font) {
  
  return s << "Font(" << font.getFace() << ", " << font.getPointSize() << ")";
  
}
