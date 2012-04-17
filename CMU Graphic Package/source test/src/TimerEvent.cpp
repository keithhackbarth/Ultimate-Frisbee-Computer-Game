/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: TimerEvent.cpp,v 1.2 2002/10/29 18:10:31 jsaks Exp $
*/

#include <iostream>
#include <CarnegieMellonGraphics.h>

using namespace std;

TimerEvent::TimerEvent(int timerid) : timerid(timerid) { }

TimerEvent::TimerEvent() : timerid(-1) {}

TimerEvent::TimerEvent(const TimerEvent &te) : timerid(te.timerid) {}

int TimerEvent::getTimerID() const {
  return timerid;
}

bool TimerEvent::operator==(const TimerEvent& rhs) const {
  return (timerid == rhs.timerid);
}

bool TimerEvent::operator!=(const TimerEvent& rhs) const {
  return !(*this == rhs);
}

std::ostream& operator<<(std::ostream& s, const TimerEvent& timer) {
	return s << "TimerEvent(" << timer.getTimerID() << ")";
}
