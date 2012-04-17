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
			outone = 0;
			outtwo = 0;
		}
		
		void drawFrisbee(Window & window, int x, int y)  {
			//draw shadow
			if(!caught && height>0)  {
				window.drawRectangleFilled(Style(Color(80,80,80,70), 5), (Field::realx(xpos,ypos)-x)-(6*2), (ypos-y)-size, (Field::realx(xpos,ypos)-x)+(6*2), (ypos-y)+size );
				window.drawRectangleFilled(Style(Color(80,80,80,70), 5), (Field::realx(xpos,ypos)-x)-(5*2), (ypos-y)-(size*2),(Field::realx(xpos,ypos)-x)+(5*2), (ypos-y) +(size*2) );
			}
			//draw disc
			window.drawRectangleFilled(Style(Color::WHITE, 5), (Field::realx(xpos,ypos)-x)-(6*2), (ypos-y)-size-height, (Field::realx(xpos,ypos)-x)+(6*2), (ypos-y)+size-height );
			window.drawRectangleFilled(Style(Color::WHITE, 5), (Field::realx(xpos,ypos)-x)-(5*2), (ypos-y)-(size*2)-height,(Field::realx(xpos,ypos)-x)+(5*2), (ypos-y) +(size*2)-height );
		}

		void centerWindow(int & x, int & y, Window & window)   {
			//different speeds of centering
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
		
		
		void move(double direction, int power, double angle, int fieldw, int fieldh, int yard, int endzone, int init)  {
			//-1*v*sin(a)*t  - projectile motion for two point on the line
			//tempy = -1 * (power * sin(direction) * temptime) - ypos; 
			//v*cos(a)*t+w  - projectile motion plus wind factor for two points
			//tempx = power * cos(direction) * (temptime) + position;
			if(!caught && height > 0)  {
				if(power >3)
					height += (power* sin(angle)*time)-(power*sin(angle)*1.85);
				else
					height += (3* sin(angle)*time)-(2*sin(angle)*1.85);//(3*sin(angle)*1.85);//(sin(angle)*time)-
				xpos += cos(direction)*power;
			    ypos += sin(direction)*power;
				time += 0.027;
			}
				
			//if out of bounds
			if((xpos < yard+endzone+size*5 || xpos > fieldw-endzone-size*2 || ypos < yard+size*2 || ypos > fieldh-size*2))   {
				if(height <= 0 && init)  {
					xpos = outone;
					ypos = outtwo;
				}
				else if((xpos < yard+size*2 || xpos > fieldw-size*2 || ypos < yard+size*2 || ypos > fieldh-size*2))   {
					 if(height <= 0) {
						xpos = outone;
						ypos = outtwo;
					}
				}
			}			
			else  {
				outone = xpos;
				outtwo = ypos;
			}
		}

		void restart()  {
			xpos = initx;
			ypos = inity;
			height = 0;
			caught = false;
		}

		bool hasCaught(Player & guy, int defense) {
			int guyx, guyy, guyh;
			int rsize, csize;

			guy.giveCords(guyx,guyy,csize,rsize,guyh);

			if(height<50)  {
				//if within bounds of player
				if( xpos>(guyx+csize/5) && xpos<(guyx+csize*4/5) 
							&& (ypos)>(guyy+rsize/12) && (ypos)<(guyy+rsize*21/22) )  {
					if(height > 0 || defense)  {
						caught = true;
						height = 35;
						xpos = guyx+csize/2;
						ypos = guyy+rsize/2+height;
						time = 0.1;
						return true;
					}
				}
			}
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
			int junk;
			guy.giveCords(x,y,csize,rsize, junk);
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