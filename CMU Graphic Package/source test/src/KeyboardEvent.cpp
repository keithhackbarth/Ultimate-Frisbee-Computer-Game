/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: KeyboardEvent.cpp,v 1.2 2002/10/23 18:50:27 jsaks Exp $
*/

#include <iostream>
#include <CarnegieMellonGraphics.h>
#include <Debugging.h>

using namespace std;

KeyboardEvent::KeyboardEvent(const int key, const KeyModifiers km) :
 value(key),  modifiers(km) {

  if(value < 0)
    fatal_exit("Tried to create a KeyboardEvent with a negative value", __FILE__, __LINE__);

  if(value > NamedKey::LAST)
    fatal_exit("Tried to create a KeyboardEvent with an invalid value (too large)", __FILE__, __LINE__);

  // If the user asks for a capitial letter, the shift modifier is implicit
  if((value >= 'A') && (value <= 'Z'))
    modifiers = modifiers | KeyModifiers::SHIFT; 

  // If the user asks for a lowercase letter, make sure there is no shift
  if((value >= 'a') && (value <= 'z')) {
    KeyModifiers temp = KeyModifiers::SHIFT;
    modifiers = modifiers & ~temp; 
  }
}

KeyboardEvent::KeyboardEvent(const KeyboardEvent &ke) : value(ke.value), modifiers(ke.modifiers) {
}

KeyboardEvent::KeyboardEvent() : value(0), modifiers(KeyModifiers::NO_MODIFIERS)
{
}

bool KeyboardEvent::operator==(const KeyboardEvent& rhs) const {
  if(((modifiers & KeyModifiers::ANY_MODIFIERS) == KeyModifiers::ANY_MODIFIERS) ||
   ((rhs.modifiers & KeyModifiers::ANY_MODIFIERS) == KeyModifiers::ANY_MODIFIERS))
     return (value == rhs.value);
  else {
    return ((modifiers == rhs.modifiers) && (value == rhs.value));
  }
}

bool KeyboardEvent::operator!=(const KeyboardEvent& rhs) const {
  return !(*this == rhs);
}

const int KeyboardEvent::getValue() const {
  return value;
}

const KeyModifiers KeyboardEvent::getModifiers() const {
  return modifiers;
}

ostream& operator<<(ostream& s, const KeyboardEvent& event) {

  return s << "KeyboardEvent(" << event.getValue() << ", " << event.getModifiers() << ")";

}
