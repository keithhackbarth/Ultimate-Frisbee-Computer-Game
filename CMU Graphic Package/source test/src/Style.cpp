/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: Style.cpp,v 1.2 2002/10/23 18:50:28 jsaks Exp $
*/

#include <iostream>
#include <CarnegieMellonGraphics.h>
#include <Debugging.h>

using namespace std;

Style::Style(Color col, int lwidth, Style::RasterOp rop, 
  const vector<bool> &newpattern): color(col), linewidth(lwidth), rasterop(rop), 
  pattern(bvectorToUShort(newpattern)) 
{
  if(linewidth < 1)
    fatal_exit("You tried to create a Style with a linewidth less than one", __FILE__, __LINE__);
}

Style::Style(const Style &s) :
  color(s.color),linewidth(s.linewidth),rasterop(s.rasterop),pattern(s.pattern)
{
}

Style &Style::operator=(const Style &rhs) {
  color = rhs.color;
  linewidth = rhs.linewidth;
  rasterop = rhs.rasterop;
  pattern = rhs.pattern;
  return *this;
}

/*
Style::Style() : color(Color::BLACK), linewidth(0), rasterop(Style::COPY_OP), pattern(0)
{
}
*/


// Accessors
Color Style::getColor() const { return color; }
int Style::getLineWidth() const { return linewidth; }
Style::RasterOp Style::getRasterOp() const { return rasterop; }

vector<bool> Style::getPattern() const {
  vector<bool> temp(16, false);
  for(unsigned short index = 0; index < 16; index++) {
    if((pattern & (1 << index)) == (1 << index))
      temp[index] = true;
  }
  return vector<bool>(temp);
}

unsigned short Style::getPatternAsUShort() const {
  return pattern;
}

// Mutators

void Style::setRasterOp(RasterOp rop) { rasterop = rop; }

void Style::setColor(Color col) { color = col; }

void Style::setLineWidth(int value) { 

  if(value < 1)
    fatal_exit("You tried to set a Style's linewidth less than one", __FILE__, __LINE__);

  linewidth = value; 
}

void Style::flipPatternBit(int position) {
  if(position < 0) {
    fatal_exit("You tried to flip a bit position less than zero", __FILE__, __LINE__);
  } else if(position >= 16) {
    fatal_exit("You tried to flip a bit position greater than 15", __FILE__, __LINE__);
  } else {
   pattern ^= (1 << position);
  }
}

void Style::setPattern(const vector<bool> &newpattern) {
  pattern = bvectorToUShort(newpattern);
}

vector<bool> Style::defaultpattern = vector<bool>(16, true);

unsigned short Style::bvectorToUShort(const vector<bool> &pattern) {

  unsigned short newpattern = 0;
    
  if(pattern.size() < 16) {
    fatal_exit("Your pattern size is too small, less than 16", __FILE__, __LINE__);
  } else if (pattern.size() > 16) {
    fatal_exit("Your pattern size is too large, greater than 16", __FILE__, __LINE__);
  } else {

    for(unsigned short index = 0; index < pattern.size(); index++) {
      unsigned short bitmask;
      if(pattern[index])
        bitmask = 1;
      else
        bitmask = 0;
  
      newpattern |= (bitmask << index);
    }
  }
  return newpattern;
}


const Style Style::BLACK = Style(Color(0,0,0));
const Style Style::WHITE = Style(Color(255,255,255));
const Style Style::RED = Style(Color(255,0,0));
const Style Style::GREEN = Style(Color(0,255,0));
const Style Style::BLUE = Style(Color(0,0,255));
const Style Style::INVERTED = Style(Color(0,0,0), 1, Style::INVERT_OP);

// Comparison operators for equality on style objects
bool Style::operator==(const Style& rhs) const {
  return ((color == rhs.color) && (linewidth == rhs.linewidth) && 
          (rasterop == rhs.rasterop) && (pattern == rhs.pattern));
}

bool Style::operator!=(const Style& rhs) const {
  return !(*this == rhs);
}

// << Stream extraction operator on style objects

ostream& operator<<(ostream& s, const Style& style) {
  string rasterString;
  
  switch(style.getRasterOp()) {
   case Style::COPY_OP:
    rasterString = "COPY_OP";
    break;

   case Style::INVERT_OP:
    rasterString = "INVERT_OP";
    break;
   case Style::XOR_OP:
    rasterString = "XOR_OP";
    break;

   case Style::AND_OP:
    rasterString = "AND_OP";
    break;

   case Style::OR_OP:
    rasterString = "OR_OP";
    break;
  }

  return s << "Style(" << style.getColor() << ", " << style.getLineWidth() << ", " <<
              rasterString << ", " << style.getPatternAsUShort() << ")";
              
}
