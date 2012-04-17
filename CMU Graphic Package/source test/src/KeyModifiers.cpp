/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: KeyModifiers.cpp,v 1.3 2003/01/06 03:53:10 jsaks Exp $
*/

#include <iostream>
#include <CarnegieMellonGraphics.h>

using namespace std;	

// Defined KeyModifiers Constants

KeyModifiers::KeyModifiers(int v) : value(v) {}

KeyModifiers::KeyModifiers()      : value(0) {}

KeyModifiers operator|(const KeyModifiers& lhs, const KeyModifiers& rhs) {
    return KeyModifiers(lhs.value | rhs.value);
}

KeyModifiers operator&(const KeyModifiers& lhs, const KeyModifiers& rhs) {
    return KeyModifiers(lhs.value & rhs.value);
}

KeyModifiers& KeyModifiers::operator|=(const KeyModifiers& rhs) {
    this->value |= rhs.value;
    return *this;
}

KeyModifiers& KeyModifiers::operator&=(const KeyModifiers& rhs) {
    this->value &= rhs.value;
    return *this;
}

KeyModifiers& KeyModifiers::operator~() {
  // Need to do some extra work to avoid creating invalid values
  KeyModifiers mask = ALL_MODIFIERS & ANY_MODIFIERS;
  this->value = (~this->value);
  *this &= mask;
  return *this;
}

bool KeyModifiers::operator==(const KeyModifiers& rhs) const {
  return (this->value == rhs.value);
}

bool KeyModifiers::operator!=(const KeyModifiers& rhs) const {
  return !(*this == rhs);
}

// Matches if there are no active modifiers 
const KeyModifiers KeyModifiers::NO_MODIFIERS      =  KeyModifiers(0x00);

// Matches "control" key
const KeyModifiers KeyModifiers::CONTROL           =  KeyModifiers(0x01);

// Matches the "shift" key
const KeyModifiers KeyModifiers::SHIFT             =  KeyModifiers(0x02);

// Matches the "alt" key
const KeyModifiers KeyModifiers::ALT               =  KeyModifiers(0x04);

// Matches if both "control" and "alt" are active
const KeyModifiers KeyModifiers::CONTROL_AND_ALT   = 
    KeyModifiers::CONTROL | KeyModifiers::ALT;
const KeyModifiers KeyModifiers::ALT_AND_CONTROL   = 
    KeyModifiers::CONTROL | KeyModifiers::ALT;

// Matches if both "control" and "shit" are active
const KeyModifiers KeyModifiers::CONTROL_AND_SHIFT = 
    KeyModifiers::CONTROL | KeyModifiers::SHIFT;
const KeyModifiers KeyModifiers::SHIFT_AND_CONTROL = 
    KeyModifiers::CONTROL | KeyModifiers::SHIFT;

// Matches if both "shift" and "alt" are active
const KeyModifiers KeyModifiers::SHIFT_AND_ALT     =
    KeyModifiers::SHIFT | KeyModifiers::ALT;
const KeyModifiers KeyModifiers::ALT_AND_SHIFT     = 
    KeyModifiers::SHIFT | KeyModifiers::ALT;

// Matches when all modifiers are active
const KeyModifiers KeyModifiers::ALL_MODIFIERS     = 
  KeyModifiers::CONTROL | KeyModifiers::SHIFT | KeyModifiers::ALT;

// Matches any modifier combination
const KeyModifiers KeyModifiers::ANY_MODIFIERS     = 
  KeyModifiers(0x08);

std::ostream& operator<<(std::ostream& s, const KeyModifiers& mod) {
  if(mod == KeyModifiers::NO_MODIFIERS) {
    return s << "Modifiers(none)";
  } else if (mod == KeyModifiers::CONTROL) {
    return s << "Modifiers(CONTROL)";
  } else if (mod == KeyModifiers::SHIFT) {
    return s << "Modifiers(SHIFT)";
  } else if (mod == KeyModifiers::ALT) {
    return s << "Modifiers(ALT)";
  } else if (mod == KeyModifiers::CONTROL_AND_ALT) {
    return s << "Modifiers(CONTROL, ALT)";
  } else if (mod == KeyModifiers::CONTROL_AND_SHIFT) {
    return s << "Modifiers(CONTROL, SHIFT)";  
  } else if (mod == KeyModifiers::SHIFT_AND_ALT) {
    return s << "Modifiers(SHIFT_AND_ALT)";
  } else if (mod == KeyModifiers::ALL_MODIFIERS) {
    return s << "Modifiers(CONTROL, SHIFT, ALT)";
  } else if ((mod & KeyModifiers::ANY_MODIFIERS) == KeyModifiers::ANY_MODIFIERS){
    return s << "Modifiers(any)";
  } else {
    return s << "WARNING: This should not be able to happen!";
  }
}
