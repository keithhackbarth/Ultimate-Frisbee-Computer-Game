/*
See "CarnegieMellonGraphics.h" for library version info and copyright information
$Id: DaemonCommand.h,v 1.4 2003/01/07 20:24:02 jsaks Exp $
*/

#ifndef __DAEMONCOMMAND_H__
#define __DAEMONCOMMAND_H__

#include <deque>
#include <queue>
#include <string>
#include <Debugging.h>
#include <CarnegieMellonGraphics.h>

class DaemonCommand {
 public:
  typedef enum {
    CREATE_WINDOW,
    DESTROY_WINDOW,
    OBSCURE_WINDOW,
    SHOW_WINDOW,
    SET_WINDOW_TITLE,
    RESIZE_WINDOW,
    SET_WINDOW_POSITION,
    GET_WINDOW_POSITION,
    CREATE_TIMER,
    START_TIMER,
    STOP_TIMER,
    ENABLE_ANTIALIASING,
    DISABLE_ANTIALIASING,
    ENABLE_AUTO_PAGE_FLIP,
    DISABLE_AUTO_PAGE_FLIP,
    ENABLE_FULLSCREEN,
    DISABLE_FULLSCREEN,
    FLIP_PAGE,
    DRAW_PIXEL,
    DRAW_LINE,
    DRAW_ARC,
    DRAW_RECTANGLE_FILLED, 
    DRAW_RECTANGLE_OUTLINE, 
    DRAW_TRIANGLE_FILLED, 
    DRAW_TRIANGLE_OUTLINE,
    DRAW_CIRCLE_FILLED,
    DRAW_CIRCLE_OUTLINE,
    DRAW_ELLIPSE_FILLED,
    DRAW_ELLIPSE_OUTLINE,
    DRAW_WEDGE_FILLED,
    DRAW_WEDGE_OUTLINE,
    DRAW_CHORD_FILLED,
    DRAW_CHORD_OUTLINE,
    DRAW_TEXT,
    DRAW_POLYGON_FILLED,
    DRAW_POLYGON_OUTLINE,
    DRAW_POLYLINE,
    DRAW_BEZIER_CURVE,
    DRAW_IMAGE,
	  DRAW_IMAGE_TRANSFORMED,
    CREATE_IMAGE,
    COPY_REGION,
    READ_PIXEL,
    IGNORE_KEY_REPEAT
  } DaemonCommand_t;

private:
  /*const*/ DaemonCommand_t comtype;
  deque<int> arguments;
  Window *wind;
  Style style;
  string stringarg;

  void init(DaemonCommand_t t, Window *win);

public:

  DaemonCommand(); 
  explicit DaemonCommand(const DaemonCommand &d);
	DaemonCommand(DaemonCommand_t t, Window *win);

  DaemonCommand &operator=(const DaemonCommand &rhs);

	~DaemonCommand();
	DaemonCommand_t type();
	Window *getWindow();

	void addArgument(const int newarg);
	int removeArgument(); 
	int countArguments();
	void setStyle(const Style& s); 
	Style getStyle(); 
	void setString(const string& s); 
	string getString(); 

};

#endif // __DAEMONCOMMAND_H__
