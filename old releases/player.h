
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <fstream.h>
#include "randgen.cpp"
//#include "field.h"

class Player
	{
	private:
		int xpos, ypos;
		int columnsize, rowsize;
		int movex, movey;
		int animate;
		int speed;
		int height;
		float size;
		double offdirec;
		//bool caught;
		bool direction;
		bool jumped;
		Color shirt;
		Image runleft, runright;
		Image standleft, standright;
	public:
		
		Player() {
			RandGen r;
			size = (float)(2.15);
			speed = 7;
			animate = 1;
			offdirec = r.RandReal(0, (2*PI) );
			jumped = false;
			height = 0;
			movex = 0;
			movey = 0;
		}

		void initialize(int x, int y, Color jersey, bool direc,
			char * filerunleft, char * filerunright, char * filestandleft, char * filestandright ) {
			
			int column = 0;
			int row = 0;
			
			xpos = x;
			ypos = y;

			shirt = jersey;
			direction = direc;

			runleft = Image(filerunleft, Image::PNG);
			runright = Image(filerunright, Image::PNG);
			standleft = Image(filestandleft, Image::PNG);
			standright = Image(filestandright, Image::PNG);
			columnsize = runleft.getWidth(); 
			rowsize = runleft.getHeight();

		}
		
		void giveCords(int & x, int & y, int & csize, int & rsize, int & h)  {
			x = xpos;
			y = ypos;
			h = height;
			csize = columnsize*size;
			rsize = rowsize*size;
		}

		//void willCatch(bool possesion)  {
		//	caught = possesion;
		//}

		void direc(bool right)  {
			direction = right;
		}

		bool giveDirec()  {
			return direction;
		}

		void updateHeight()  {
			//jump
			if(jumped)  {
				height += 3;
				if(height > 40)
					jumped = false;
			}
			else  {
				if(height > 0) 
					height -= 3;
				else  
					height = 0;
			}
		}

		void move(int x, int y, int fieldx, int fieldy, int yard, int caught)  {
			if(height < 20)  {
				//change direction of player based on movement
				if(x > 0)
					direction = true;
				if(x < 0)
					direction = false;

				//move players as long as inside of field
				if( ypos+y+(rowsize*size*2/3) >= yard && ypos+y+(rowsize*size) <= fieldy+yard*2/3 && caught!=1)
					ypos += y;			
				if( xpos+x+(columnsize*size*1/2) >= yard && xpos+x+(columnsize*size*1/2) <= fieldx && caught!=1)
					xpos += x;

				//store movement
				movex = x;
				movey = y;
			}
		}

		bool moveToLocation(int x, int y, int fieldx, int fieldy, int yard)  {
			if(xpos>x+speed)
				move(-speed, 0, fieldx, fieldy, yard, 0);
			else if(xpos<x-speed)
				move(speed, 0, fieldx, fieldy, yard, 0);
			else
				xpos = x;
			if(ypos>y+speed)
				move(0,-speed,fieldx, fieldy, yard, 0);
			else if(ypos<y-speed)
				move(0, speed, fieldx, fieldy, yard, 0);
			else
				ypos = y;
			
			if(xpos == x && ypos == y)
				return true;
			else
				return false;
		}


		void defense(Player & guy, int fieldx, int fieldy, int yard) {
			int x, y;
			int junk;
			guy.giveCords(x,y,junk,junk,junk);
			if(x > xpos+30) 
				move(speed-3, 0, fieldx, fieldy, yard, 0);
			if(x < xpos-30)
				move(-speed+3, 0, fieldx, fieldy, yard, 0);
			if(y > ypos+30)
				move(0, speed, fieldx, fieldy, yard, 0);
			if(y < ypos-30)
				move(0,-speed+3, fieldx, fieldy, yard, 0);
		}

		void offense(int fieldx, int fieldy, int yard) {
			RandGen r;
			double random;
			random = r.RandReal(0, 120);
			if(random < 2*PI)
				offdirec = random;
			move(speed*cos(offdirec), speed*sin(offdirec), fieldx, fieldy, yard, 0);
		}
		
		void drawPlayer(Window & window, int x, int y, int caught)   {
			int walk=0;
			if(animate < 10 && caught!=1 && height == 0)
				walk = 0;
			else if(animate >= 20 && caught!=1 && height == 0)
				animate = 0;
			else
				walk = 1;

			animate++;

			if( Field::realx(xpos,ypos)+columnsize*size > x && (Field::realx(xpos,ypos) < x+window.getWidth()) ) {
				if( (ypos+rowsize*size > y) && (ypos < y+window.getHeight()) )  {
					if(direction)  {
						if(walk == 1)   
							window.drawImage(standright, Field::realx(xpos,ypos)-x, ypos-y-height, Transform(size, size));
						else
							window.drawImage(runright, Field::realx(xpos,ypos)-x, ypos-y-height, Transform(size, size));
					}
					else
						if(walk == 1)   
							window.drawImage(standleft, Field::realx(xpos,ypos)-x, ypos-y-height, Transform(size, size));
						else
							window.drawImage(runleft, Field::realx(xpos,ypos)-x, ypos-y-height, Transform(size, size));
				}
			}
		}

		void drawSelected(Window & window, int x, int y)  {
			window.drawCircleFilled(Style(Color(255,0,0,95)), 
				Field::realx(xpos,ypos)-x+(columnsize), ypos-y+(rowsize), columnsize+10);
		}
		
	//bool hasCaught () {
	//	return caught;
	//}

	void drawPixel(Window & window, int yard)  {
		window.drawRectangleFilled(Style(shirt,3), window.getWidth()*3/4+(xpos)/yard, window.getHeight()-55+(ypos)/yard,
			  window.getWidth()*3/4+(xpos+size*columnsize)/yard, window.getHeight()-55+(ypos+size*rowsize)/yard);
	}

	void drawPixelSelected(Window & window, int yard)  {
		window.drawRectangleFilled(Style(Color(255,0,255, 70),3), window.getWidth()*3/4+(xpos)/yard, window.getHeight()-55+(ypos)/yard,
			  window.getWidth()*3/4+(xpos+size*columnsize)/yard, window.getHeight()-55+(ypos+size*rowsize)/yard);
	}

	bool hasScored(int x1, int x2, int y1, int y2, int & score)  {
		if(xpos>x1 && xpos<x2 && ypos>y1 && ypos<y2)  {
			score++;
			return true;
		}
		return false;
	}

	void jump()  {
		if(height == 0)
		jumped = true;
	}

	int getMoveX() {
		return movex;
	}

	int getMoveY()  {
		return movey;
	}

};

#endif

		
/*ifstream filein(file1); 

			while(!filein.eof())  {
				filein >> get;
				//cout << get;

				if(column >= columnsize)  {
					row++;
				    column = 0;
				}
				else  {
					playerdraw[0][column][row] = get;
					column++;
				}

			}
			filein.close();

			ifstream filein2(file2);

			
			column = 0;
			row =0 ;
			while(!filein2.eof())  {
				filein2 >> get;
				//cout << get;

				if(column >= columnsize)  {
					row++;
				    column = 0;
				}
				else  {
					playerdraw[1][column][row] = get;
					column++;
				}

			}

			filein2.close();*/

/*void drawPlayerRight(Window & window, int x, int y) {
			int column;
			int row;
			int walk=0;

			if(animate < 10)
				walk = 0;
			else if(animate >= 20)
				animate = 0;
			else
				walk = 1;

			animate++;
			
			if(caught)
				walk = 1;

			Style HAIR = Style(Color(136,20,0), 5); //1
			Style EYES = Style(Color::WHITE, 5);    //2
			Style SKIN = Style(Color(248,136,48), 5); //3
			Style OUTLINE = Style(Color(0,60,160), 5); //4
			Style JERSEY = Style(shirt, 5); //5

			for(row = 0; row < rowsize; ++row)   {
				for(column=0; column < columnsize; ++column)   {
					//cout << playerdraw[column][row];
					if(playerdraw[walk][column][row] == '1')   {
						window.drawRectangleFilled(HAIR, column*size+x, row*size+y,(column+1)*size+x,(row+1)*size+y);
						}
					if(playerdraw[walk][column][row] == '2')   {
						window.drawRectangleFilled(EYES, column*size+x, row*size+y,(column+1)*size+x,(row+1)*size+y);
						}
					if(playerdraw[walk][column][row] == '3')   {
						window.drawRectangleFilled(SKIN, column*size+x, row*size+y,(column+1)*size+x,(row+1)*size+y);
						}
					if(playerdraw[walk][column][row] == '4')   {
						window.drawRectangleFilled(OUTLINE, column*size+x, row*size+y,(column+1)*size+x,(row+1)*size+y);
						}
					if(playerdraw[walk][column][row] == '5')   {
						window.drawRectangleFilled(JERSEY, column*size+x, row*size+y,(column+1)*size+x,(row+1)*size+y);
						}
				}
			}
			

		}

	void drawPlayerLeft(Window & window, int x, int y) {
			int column;
			int row;
			int walk =0;

			if(animate < 10)
				walk = 0;
			else if(animate >= 20)
				animate = 0;
			else
				walk = 1;

			animate++;	
			
			if(caught)
				walk = 1;

			Style HAIR = Style(Color(136,20,0), 5); //1
			Style EYES = Style(Color::WHITE, 5);    //2
			Style SKIN = Style(Color(248,136,48), 5); //3
			Style OUTLINE = Style(Color(0,60,160), 5); //4
			Style JERSEY = Style(shirt, 5); //5

			for(row = 0; row < rowsize; ++row)   {
				for(column=0; column < columnsize; ++column)   {
					//cout << playerdraw[column][row];
					if(playerdraw[walk][columnsize-column][row] == '1')   {
						window.drawRectangleFilled(HAIR, column*size+x, row*size+y,(column+1)*size+x,(row+1)*size+y);
						}
					if(playerdraw[walk][columnsize-column][row] == '2')   {
						window.drawRectangleFilled(EYES, column*size+x, row*size+y,(column+1)*size+x,(row+1)*size+y);
						}
					if(playerdraw[walk][columnsize-column][row] == '3')   {
						window.drawRectangleFilled(SKIN, column*size+x, row*size+y,(column+1)*size+x,(row+1)*size+y);
						}
					if(playerdraw[walk][columnsize-column][row] == '4')   {
						window.drawRectangleFilled(OUTLINE, column*size+x, row*size+y,(column+1)*size+x,(row+1)*size+y);
						}
					if(playerdraw[walk][columnsize-column][row] == '5')   {
						window.drawRectangleFilled(JERSEY, column*size+x, row*size+y,(column+1)*size+x,(row+1)*size+y);
						}
				}
			}

		}
*/

/*
			EditableImage guyrunleft(Image(filerunleft, Image::PNG));
			EditableImage guyrunright(Image(filerunright, Image::PNG));
			EditableImage guystandleft(Image(filestandleft, Image::PNG));
			EditableImage guystandright(Image(filestandright, Image::PNG));

			columnsize = guyrunleft.getWidth(); 
			rowsize = guyrunleft.getHeight();

			for(row=0; row < rowsize; ++row)  {
				for(column=0; column < columnsize; ++column)  {
					if(guyrunleft.getPixel(column, row) == Color(0,248,248) )
						guyrunleft.setPixel(column, row, jersey);
					if(guyrunright.getPixel(column, row) == Color(0,248,248) )
						guyrunright.setPixel(column, row, jersey);
					if(guystandleft.getPixel(column, row) == Color(0,248,248) )
						guystandleft.setPixel(column, row, jersey);
					if(guystandright.getPixel(column, row) == Color(0,248,248) )
						guystandright.setPixel(column, row, jersey);
				}
			}
			
			guyrunleft.save("guy2runleft.png", Image::PNG);
			guyrunright.save("guy2runright.png", Image::PNG);
			guystandleft.save("guy2standleft.png", Image::PNG);
			guystandright.save("guy2standright.png", Image::PNG);

			runleft = Image("guy2runleft.png", Image::PNG);
			runright = Image("guy2runright.png", Image::PNG);
			standleft = Image("guy2standleft.png", Image::PNG);
			standright = Image("guy2standright.png", Image::PNG);*/