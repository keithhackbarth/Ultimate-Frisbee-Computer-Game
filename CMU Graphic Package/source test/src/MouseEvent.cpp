/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: MouseEvent.cpp,v 1.4 2002/10/29 18:10:31 jsaks Exp $
*/

#include <iostream>
#include <string>
#include <CarnegieMellonGraphics.h>
#include <Debugging.h>

using namespace std;

MouseEvent::MouseEvent(const MouseEvent::Event e,
                       const MouseEvent::Button b,
                       const int x, const int y,
                       KeyModifiers km) :
 event(e), button(b), ex(x), ey(y), modifiers(km) {
//  if(ex < -1)
//    fatal_exit("Created a MouseEvent with a X coordinate that is negative, but not negative one (the \"any\" coordinate)", __FILE__, __LINE__);
//  if(ey < -1)
//    fatal_exit("Created a MouseEvent with a Y coordinate that is negative, but not negative one (the \"any\" coordinate", __FILE__, __LINE__);
//  if((ex == -1) && (ey != -1))
//    fatal_exit("Created a MouseEvent with the X coordinate negative one (the \"any\" coordinate), but not the Y coordinate",  __FILE__, __LINE__);
//  if((ey == -1) && (ex != -1))
//    fatal_exit("Created a MouseEvent with the Y coordinate negative one (the \"any\" coordinate), but not the X coordinate",  __FILE__, __LINE__);
}

MouseEvent::MouseEvent(const MouseEvent &me) : event(me.event), button(me.button), ex(me.ex), ey(me.ey), modifiers(me.modifiers)
{
}

MouseEvent::MouseEvent() : event(MouseEvent::NULL_EVENT), button(MouseEvent::NO_BUTTON), ex(0), ey(0), modifiers(KeyModifiers::NO_MODIFIERS) {
}

const MouseEvent::Event MouseEvent::getEvent() const {
  return event;
}

const MouseEvent::Button MouseEvent::getButton() const {
  return button;
}

const int MouseEvent::getX() const {
  return ex;
}

const int MouseEvent::getY() const {
  return ey;
}

const KeyModifiers MouseEvent::getModifiers() const {
  return modifiers;
}

bool MouseEvent::operator==(const MouseEvent& rhs) const {

  if(((modifiers & KeyModifiers::ANY_MODIFIERS) == KeyModifiers::ANY_MODIFIERS) ||
    ((rhs.modifiers & KeyModifiers::ANY_MODIFIERS) == KeyModifiers::ANY_MODIFIERS)) {
    if(((ex == -1) && (ey == -1)) || ((rhs.ex == -1) && (rhs.ey == -1))) {
      if((button == ANY_BUTTON) || (rhs.button == ANY_BUTTON))
        return (event == rhs.event);
      else
        return ((event == rhs.event) && (button == rhs.button));
    } else {
      if((button == ANY_BUTTON) || (rhs.button == ANY_BUTTON))
        return ((event == rhs.event) && (ex == rhs.ex) && (ey == rhs.ey));
      else
        return ((event == rhs.event) && (button == rhs.button)
                && (ex == rhs.ex) && (ey == rhs.ey));
    }
  } else {
    if(((ex == -1) && (ey == -1)) || ((rhs.ex == -1) && (rhs.ey == -1))) {
      if((button == ANY_BUTTON) || (rhs.button == ANY_BUTTON))
        return ((event == rhs.event) && (modifiers == rhs.modifiers));
      else
        return ((event == rhs.event) && (button == rhs.button)
                && (modifiers == rhs.modifiers));
    } else {
      if((button == ANY_BUTTON) || (rhs.button == ANY_BUTTON))
        return ((event == rhs.event) && (modifiers == rhs.modifiers)
                && (ex == rhs.ex) && (ey == rhs.ey));
      else
        return ((event == rhs.event) && (button == rhs.button)
                && (modifiers == rhs.modifiers) && (ex == rhs.ex)
                && (ey == rhs.ey));
    }
  }
}

bool MouseEvent::operator!=(const MouseEvent& rhs) const {
  return !(*this == rhs);
}


std::ostream& operator<<(std::ostream& s, const MouseEvent& e) {

  string eventString, buttonString;

  switch(e.getEvent()) {
   case MouseEvent::MOUSE_MOVE_EVENT:
    eventString = "MOUSE_MOVE_EVENT";
    break;

   case MouseEvent::ENTER_WINDOW_EVENT:
    eventString = "ENTER_WINDOW_EVENT";
    break;

   case MouseEvent::EXIT_WINDOW_EVENT:
    eventString = "EXIT_WINDOW_EVENT";
    break;

   case MouseEvent::BUTTON_CLICK_EVENT:
    eventString = "BUTTON_CLICK_EVENT";
    break;

   case MouseEvent::BUTTON_DOWN_EVENT:
    eventString = "BUTTON_DOWN_EVENT";
    break;

   case MouseEvent::BUTTON_UP_EVENT:
    eventString = "BUTTON_UP_EVENT";
    break;
   default:
    eventString = "NULL";
    break;
  }

  switch(e.getButton()) {
   case MouseEvent::NO_BUTTON:
    buttonString = "NO_BUTTON";
    break;

   case MouseEvent::LEFT_BUTTON:
    buttonString = "LEFT_BUTTON";
    break;

   case MouseEvent::RIGHT_BUTTON:
    buttonString = "RIGHT_BUTTON";
    break;

   case MouseEvent::MIDDLE_BUTTON:
    buttonString = "MIDDLE_BUTTON";
    break;

   case MouseEvent::ANY_BUTTON:
    buttonString = "ANY_BUTTON";
    break;
   default:
    buttonString = "NULL";
    break;
  }

  return s << "MouseEvent(" << eventString << ", " << buttonString <<
               ", " << e.getX() << ", " << e.getY() << ", " << e.getModifiers() << ")";

}
