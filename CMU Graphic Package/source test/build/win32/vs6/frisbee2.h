#ifndef __FRISBEE_H__
#define __FRISBEE_H__

#include "player.h"

class Frisbee
	{
	private:
		bool caught;
		double xpos, ypos;
		int initx, inity;
		int height;
		double time;
		float size;
		int outone, outtwo;
	public:
		
		Frisbee(int x, int y)  {
			xpos = x;
			ypos = y;
			initx = x;
			inity = y;
			height = 0;
			size = (float)(2.15);
			caught = false;
			time = 0.1;
		}
		
		void drawFrisbee(Window & window, int x, int y)  {
			if(!caught && height>0)  {
				window.drawRectangleFilled(Style(Color(80,80,80,70), 5), (Field::realx(xpos,ypos)-x)-(6*2), (ypos-y)-size, (Field::realx(xpos,ypos)-x)+(6*2), (ypos-y)+size );
				window.drawRectangleFilled(Style(Color(80,80,80,70), 5), (Field::realx(xpos,ypos)-x)-(5*2), (ypos-y)-(size*2),(Field::realx(xpos,ypos)-x)+(5*2), (ypos-y) +(size*2) );
			}
			window.drawRectangleFilled(Style(Color::WHITE, 5), (Field::realx(xpos,ypos)-x)-(6*2), (ypos-y)-size-height, (Field::realx(xpos,ypos)-x)+(6*2), (ypos-y)+size-height );
			window.drawRectangleFilled(Style(Color::WHITE, 5), (Field::realx(xpos,ypos)-x)-(5*2), (ypos-y)-(size*2)-height,(Field::realx(xpos,ypos)-x)+(5*2), (ypos-y) +(size*2)-height );
		}

		void centerWindow(int & x, int & y, Window & window)   {
				if( Field::realx(xpos,ypos) < x+window.getWidth()/6 )
					x -= 3;
				if( Field::realx(xpos,ypos) > x+window.getWidth()*5/6)
					x +=3;
				if( ypos < y+window.getHeight()/6 )
					y -=3;
				if( ypos > y+window.getHeight()*5/6 )
					y +=3;
				if( Field::realx(xpos,ypos) < x+window.getWidth()/5 )
					x -= 4;
				if( Field::realx(xpos,ypos) > x+window.getWidth()*4/5)
					x +=4;
				if( ypos < y+window.getHeight()/5 )
					y -=4;
				if( ypos > y+window.getHeight()*4/5 )
					y +=4;
				if( Field::realx(xpos,ypos) < x+window.getWidth()/3 )
					x -=3;
				if( Field::realx(xpos,ypos) > x+window.getWidth()*2/3)
					x +=3;
				if( ypos < y+window.getHeight()/3 )
					y -=3;
				if( ypos > y+window.getHeight()*2/3 )
					y +=3;
			}
		
		
		void move(double direction, int power, double angle, int fieldw, int fieldh, int yard)  {
			if(!caught && height > 0)  {
				if(power >3)
					height += (power* sin(angle)*time)-(power*sin(angle)*1.85);
				else
					height += (3* sin(angle)*time)-(2*sin(angle)*1.85);//(3*sin(angle)*1.85);//(sin(angle)*time)-
				xpos += cos(direction)*power;
			    ypos += sin(direction)*power;
				time += 0.027;
			}
			//-1*v*sin(a)*t  - projectile motion for two point on the line
			//tempy = -1 * (power * sin(direction) * temptime) - ypos; 
			//v*cos(a)*t+w  - projectile motion plus wind factor for two points
			//tempx = power * cos(direction) * (temptime) + position;
			
			if(height <= 0)   {
				if(xpos < yard || xpos > fieldw || ypos < yard || ypos > fieldh)  {
					xpos = fieldw/2;
					ypos = fieldh/2-30;
				}
			}
		}

		void restart()  {
			xpos = initx;
			ypos = inity;
			height = 0;
		}

	/*	bool inArea(int x1, int x2, int y1, int y2)  {
			if(xpos > x1 && xpos < x2 && ypos > y1 && ypos < y2)
				return true;
			else
				return false;
		}*/

		bool hasCaught(Player & guy, int defense) {
			int guyx, guyy;
			int rsize, csize;

			guy.giveCords(guyx,guyy,csize,rsize);

			if(height<50)  {
				if( xpos>(guyx+csize/5) && xpos<(guyx+csize*4/5) && (ypos-height)>(guyy+rsize/12) && (ypos-height)<(guyy+rsize*21/22) )  {
				//if( xpos>(guyx+csize/5) && xpos<(guyx+csize*4/5) && ypos>(guyy+rsize/7) && ypos<(guyy+rsize*11/12) && height<rsize*11/12 )  {
					if(height > 0 || defense)  {
						//guy.willCatch(true);
						caught = true;
						height = 35;
						xpos = guyx+csize/2;
						ypos = guyy+rsize/2+height;
						time = 0.1;
						return true;
					}
				}
			}

			//return caught;
			return false;
		}

		void throwFrisbee()  {
			caught = false;
		}

		void drawArrow (Window & window, Player & guy, double x, double y, double i)  {
			Style ARROW = Style(Color::BLACK, 5);
			x = (Field::realx(xpos,ypos)-x);
			y = ypos - y;
			window.drawLine(ARROW, x, y-height, x+(22*size)*cos(i), y+(22*size)*sin(i)-height);
			window.drawTriangleFilled(ARROW, x+(22*size)*cos(i+.2), y+(22*size)*sin(i+.2)-height, x+(26*size)*cos(i), y+(26*size)*sin(i)-height, x+(22*size)*cos(i-.2), y+(22*size)*sin(i-.2)-height);	
			if( cos(i) < 0 )
				guy.direc(false);
			else
				guy.direc(true);
		}

		int dist(Player & guy)  {
			int x,y;
			int csize, rsize;
			guy.giveCords(x,y,csize,rsize);
			if(height<50) 
				return ( ((xpos-x-csize/2)*(xpos-x-csize/2)) + ((ypos-y-height-rsize/2)*(ypos-y-height-rsize/2)) );
			else
				return ( ((xpos-x-csize/2)*(xpos-x-csize/2)) + ((ypos-y-rsize/2)*(ypos-y-rsize/2)) );
		}

		void changeInit(int x, int y)  {
			initx = x;
			inity = y;
		}

		void drawPixel(Window & window, int yard)  {
			window.drawRectangleFilled(Style(Color(255,100,150),3), window.getWidth()*3/4+(xpos-size*8)/yard, window.getHeight()-55+(ypos-size*8)/yard,
			  window.getWidth()*3/4+(xpos+size*8)/yard, window.getHeight()-55+(ypos+size*8)/yard);
		}
};


#endif