/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: DaemonCommand.cpp,v 1.2 2002/10/22 19:52:14 jsaks Exp $
*/

#include <DaemonCommand.h>

  DaemonCommand::DaemonCommand(DaemonCommand_t t, Window *win) : comtype(t),
    wind(win),style(Style::BLACK) {
      fatal_assert((wind == NULL), "I seem to have recieved a NULL Window pointer", __FILE__, __LINE__);
  }

  DaemonCommand::DaemonCommand() : style(Style::BLACK) {
    comtype = (DaemonCommand_t)-1;
  //arguments = NULL;
  wind = NULL;
  //style = NULL;
  //stringarg = NULL;
  }



  DaemonCommand::DaemonCommand(const DaemonCommand &d) : comtype(d.comtype), arguments(d.arguments), wind(d.wind),
    style(d.style), stringarg(d.stringarg)
  {
  }

  DaemonCommand &DaemonCommand::operator=(const DaemonCommand &rhs) {
    comtype = rhs.comtype;
    arguments = rhs.arguments;
    wind = rhs.wind;
    style = rhs.style;
    stringarg = rhs.stringarg;
    return *this;
  }

  DaemonCommand::~DaemonCommand() {
    int size = arguments.size();
    for (int i = 0; i < size; i++) {
      arguments.pop_front();
    }
  }

  DaemonCommand::DaemonCommand_t DaemonCommand::type() {
    return comtype;
  }

  Window *DaemonCommand::getWindow() {
    fatal_assert((wind == NULL), "Tried to remove a NULL Window pointer from a command", __FILE__, __LINE__);
    return wind;
  }

  void DaemonCommand::addArgument(int newarg) {
//    if(arguments == NULL) {
//      arguments = new queue<int>;
//    }

    arguments.push_back(newarg);
  }

  int DaemonCommand::removeArgument() {
    fatal_assert((arguments.empty()), "Tried to remove more arguments from a command than it contained", __FILE__, __LINE__);
    int temp = arguments.front();
    arguments.pop_front();
    return temp;
  }

  void DaemonCommand::setStyle(const Style& s) {
//    fatal_assert((style != NULL), "Tried to set a command's style more than once", __FILE__, __LINE__);

    style = s;

//    fatal_assert((style == NULL), "Could not allocate sufficent memory to allocate a new style object for a command.  Free up some memory and try again", __FILE__, __LINE__);
  }

  Style DaemonCommand::getStyle() {
//    nonfatal_assert((style == NULL), "Tried to remove a NULL style from a command", __FILE__, __LINE__);
    return style;
  }

  void DaemonCommand::setString(const string& s) {
    fatal_assert((stringarg != ""), "Tried to set a command's string argument more than once", __FILE__, __LINE__);

    stringarg = s;

//    fatal_assert((stringarg == NULL), "Could not allocate sufficent memory to allocate a new string object for a command.  Free up some memory and try again", __FILE__, __LINE__);
  }

  string DaemonCommand::getString() {
//    nonfatal_assert((stringarg == NULL), "Tried to remove a NULL stringarg from a command", __FILE__, __LINE__);
    return stringarg;
  }

  int DaemonCommand::countArguments() {
    return arguments.size();
  }
