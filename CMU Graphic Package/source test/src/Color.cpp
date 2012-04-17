/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: Color.cpp,v 1.6 2002/11/18 02:11:33 jsaks Exp $
*/

#include <iostream>
#include <string>
#include <CarnegieMellonGraphics.h>
#include <Debugging.h>

using namespace std;

unsigned char Color::checkValue(int value, ComponentName cn) {

  if(value < 0) { 
    string err = ComponentNameToString(cn) + " color component specified to be less than zero";
    warning(err.c_str(), __FILE__, __LINE__);
    return static_cast<unsigned char>(0);

  } else if(value > 255) { 
	  string err = ComponentNameToString(cn) + " color component specified to be less than zero";
      warning(err.c_str(), __FILE__, __LINE__);
    return static_cast<unsigned char>(255);
  } else {
    return static_cast<unsigned char>(value);    
  }
 
}

// Different constructors for color
Color::Color() : red(0), green(0), blue(0), alpha(255) {}

Color::Color(int red, int green, int blue) : red(checkValue(red, __RED)), 
  green(checkValue(green, __GREEN)), blue(checkValue(blue, __BLUE)), 
  alpha(255) { 
}

Color::Color(int red, int green, int blue, int alpha) : 
  red(checkValue(red, __RED)), green(checkValue(green, __GREEN)), 
  blue(checkValue(blue, __BLUE)), alpha(checkValue(alpha, __ALPHA)) { }

bool Color::operator==(const Color& rhs) const {
  return ((red == rhs.red) && (green == rhs.green) && (blue == rhs.blue) &&
     (alpha == rhs.alpha));
}

bool Color::operator!=(const Color& rhs) const {
  return !(*this == rhs);
}

/*
// Accessors
int Color::getRed() const { return static_cast<int>(red); }
int Color::getGreen() const { return static_cast<int>(green); }
int Color::getBlue() const { return static_cast<int>(blue); }
int Color::getAlpha() const { return static_cast<int>(alpha); }
*/
/*
// Mutators
void Color::setRed(int value) { red = checkValue(value, __RED); }
void Color::setGreen(int value) { green = checkValue(value, __GREEN); }
void Color::setBlue(int value) { blue = checkValue(value, __BLUE); }
void Color::setAlpha(int value) { alpha = checkValue(value, __ALPHA); }
*/
std::ostream& operator<<(std::ostream& s, const Color& color) {
  return s << "Color(" << static_cast<int>(color.getRed()) << ", " <<  
              static_cast<int>(color.getGreen()) << ", " << 
              static_cast<int>(color.getBlue()) << ", " << 
              static_cast<int>(color.getAlpha()) << ")";
}

const Color Color::BLACK = Color(0, 0, 0);
const Color Color::WHITE = Color(255, 255, 255);
const Color Color::RED = Color(255, 0, 0);
const Color Color::GREEN = Color(0, 255, 0);
const Color Color::BLUE = Color(0, 0, 255);

std::string Color::ComponentNameToString(ComponentName cn) {
  switch (cn) {
    case __RED: return "Red";
    case __BLUE: return "Blue";
    case __GREEN: return "Green";
    case __ALPHA: return "Alpha";
    default: return "Unknown";
  }
}
