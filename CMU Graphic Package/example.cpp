#include <iostream>
#include <math.h>
#include "CarnegieMellonGraphics.h"

using namespace std;

const double pi = 3.1415926;

int main() {

  int i = 0;
  int x = 150;
  int y = 150;

  Style THICK_GREEN = Style(Color::GREEN, 3);

  cout << "hi";

  Window test(300,400);

  Image img("scs.jpg",Image::JPEG);
  Image subimg = img.subImage(0,0,100,150);

  test.disableAutoPageFlip();

  while(true) {
    if(!test.isKeyboardQueueEmpty()) {
      KeyboardEvent event = test.getKeyboardEvent();
//      cout << event << endl;

      if(KeyboardEvent(NamedKey::ESCAPE) == event) {
        return 0;
      }

    }

    if(!test.isMouseQueueEmpty()) {
      MouseEvent event = test.getMouseEvent();
//      cout << event << endl;
    }

    x = test.getMouseX();
    y = test.getMouseY();

    test.drawLine(Style::BLACK, 
      (int)(150 + 100*cos((double)(i-1)/(8 * pi))), 
      (int)(150 + 100*sin((double)(i-1)/(8 * pi))),
      (int)(150 + 100*cos(pi + (double)(i-1)/(8 * pi))), 
      (int)(150 + 100*sin(pi + (double)(i-1)/(8 * pi))) );

    test.drawTriangleFilled(Style::RED, x, y, 250, 250, 50, 250);

    test.drawLine(THICK_GREEN, 
      (int)(150 + 100*cos((double)i/(8 * pi))), 
      (int)(150 +100*sin((double)i/(8 * pi))),
      (int)(150 + 100*cos(pi + (double)i/(8 * pi))), 
      (int)(150 + 100*sin(pi + (double)i/(8 * pi))) );
  i++;

    test.drawRectangleOutline(Style::WHITE, 140, 140, 160, 160);

//	test.drawImage(subimg,x,y,Transform(i,x/(double)test.getWidth(),y/(double)test.getHeight()));
	test.drawImage(img,x,y,Transform(i));
//	test.drawImage(img,x,y);
	test.drawLine(Style::WHITE,0,0,test.getWidth(),test.getHeight());

    test.flipPage();

    test.drawRectangleFilled(Style::BLACK, 0, 0, test.getWidth(), test.getHeight());
  }

  return 0;
}

