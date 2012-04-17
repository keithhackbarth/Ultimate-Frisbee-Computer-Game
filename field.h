#ifndef __FIELD_H__
#define __FIELD_H__

class Field
	{
	private:
		int yardsize;
		int fieldh;
		int fieldw;
		int endzone;
	public:
		
		Field(int yard, int width, int height, int ezone)  {
			yardsize = yard;
			fieldh = height;
			fieldw = width;
			endzone = ezone;
		}
		
		static int realx(int x, int y)  {
			return (x+abs(800-y)/40);
		}

		void drawField(Window & myWindow, int x, int y) {
			Style THICK_WHITE = Style(Color::WHITE, 5);
			Style GRASS_STREAKS = Style(Color(0,213,0), 20);

			myWindow.drawRectangleFilled(Style::GREEN, 0, 0, myWindow.getWidth(), myWindow.getHeight());

			myWindow.drawRectangleFilled(GRASS_STREAKS, 0, 0, myWindow.getWidth(), yardsize-y); 
			myWindow.drawRectangleFilled(GRASS_STREAKS, 0, (fieldh/7)-y, myWindow.getWidth(), (fieldh*2/7)-y); 
			myWindow.drawRectangleFilled(GRASS_STREAKS, 0, (fieldh*3/7)-y, myWindow.getWidth(), (fieldh*4/7)-y); 
			myWindow.drawRectangleFilled(GRASS_STREAKS, 0, (fieldh*5/7)-y, myWindow.getWidth(), (fieldh*6/7)-y);
			myWindow.drawRectangleFilled(GRASS_STREAKS, 0, (fieldh)-y, myWindow.getWidth(), myWindow.getHeight());
			//myWindow.drawRectangleFilled(GRASS_STREAKS, yardsize-myWindow.getWidth()-x, -y, yardsize-x, fieldh-y);
			//myWindow.drawRectangleFilled(GRASS_STREAKS, fieldw-x, fieldh-y, fieldw+myWindow.getWidth()-x, -y);
	
			//top boundary
			myWindow.drawLine(THICK_WHITE,realx(yardsize,yardsize)-x,yardsize-y,realx(fieldw,yardsize)-x,yardsize-y);
			//bottom boundary
			myWindow.drawLine(THICK_WHITE,realx(yardsize,fieldh)-x,fieldh-y,realx(fieldw,fieldh)-x,fieldh-y);
			//mid field
			myWindow.drawLine(THICK_WHITE, realx(fieldw/2,yardsize)-x,yardsize-y,realx(fieldw/2,fieldh)-x, fieldh-y);
			//left back
			myWindow.drawLine(THICK_WHITE,realx(yardsize,yardsize)-x, yardsize-y, realx(yardsize,fieldh)-x, fieldh-y);
			//right back
			myWindow.drawLine(THICK_WHITE,realx(fieldw,yardsize)-x, yardsize-y, realx(fieldw,fieldh)-x, fieldh-y);
			//left endzone
			myWindow.drawLine(THICK_WHITE,realx(yardsize+endzone,yardsize)-x,yardsize-y,realx(yardsize+endzone,fieldh)-x,fieldh-y);
			//right endzone
			myWindow.drawLine(THICK_WHITE,realx(fieldw-endzone,yardsize)-x,yardsize-y,realx(fieldw-endzone,fieldh)-x,fieldh-y);

		}

};


#endif