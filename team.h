#ifndef __TEAM_H__
#define __TEAM_H__

#include "player.h"
#include "frisbee.h"

class Team
	{
	private:
		Player myplayers[7];
		int distance[7];
		int numplayers;
		int status;
		int initxpos;
		int closest;
		int mycaught;
		int score;
		int active;
		int turbo;
		/////for some reason it wouldnt let me add another variable!!!
		int lastcaught;
		int selected;
		
	public:

		Team()  {
			numplayers = 7;
			status = 0;
			closest = 0;
			mycaught = 0;
			lastcaught = 0;
			score = 0;
			active = 0;
			turbo = 210;
			selected = 3;
		}
		
		void initialize(int xpos, int fieldh, Color jersey, bool direction, 
				char * runleft, char * runright, char * standleft, char * standright)  {			
						
			initxpos = xpos;
			
			//initialize players
			for(int loop = 0; loop < numplayers; ++loop)   {
				myplayers[loop].initialize(xpos,fieldh*(1+loop)/9,jersey, direction, runleft, runright, standleft, standright);
			}
			//direction array
			for(loop = 0; loop < numplayers; ++loop)  {
				distance[loop] = loop;
			}	
		}

		void hasCaught(Team & otherTeam, Frisbee & disc, int caught, double & rotation, int pulled, int x1, int x2, int y1, int y2, int & stall)   {
			for(int loop = 0; loop<numplayers; ++loop)  {		
				if( !caught && status!=3 && ((lastcaught != loop+1) || status !=1) && (!pulled || otherTeam.getInit()) )
					if(disc.hasCaught(myplayers[loop], status-1)) { 
							mycaught = (loop+1);
							lastcaught = (loop+1);
							selected = (loop);
							stall = 0;
							//change offense
							if(pulled && active && otherTeam.getInit())  {
								status = 1;
								otherTeam.changeStatus(2);
							}
							if(pulled && !active)  {
								active = true;
								status = 1;
								otherTeam.changeStatus(2);
							}
							if(active) 
								PlaySound("punch.wav",NULL,SND_FILENAME|SND_ASYNC); 
							//change direction
							if(myplayers[loop].giveDirec())
								rotation = 0;
							else
								rotation = PI;
							//check scored
							if(myplayers[loop].hasScored(x1, x2, y1, y2, score) )  {
								status = 3;
								if(otherTeam.initxpos > initxpos) {
									initxpos += 1000;
									otherTeam.initxpos -= 1000;
								}
								else if(otherTeam.initxpos < initxpos) {
									initxpos -= 1000;
									otherTeam.initxpos += 1000;
								}
								PlaySound("clap.wav",NULL,SND_FILENAME|SND_ASYNC);
							}
						}
			}
		}

		//void willCatch(int playernumb, bool will)  {
		//	myplayers[playernumb].willCatch(will);
		//}

		void drawTeam(Window & window, int x, int y)  {
			//myplayers[closest].drawSelected(window,x,y);
			myplayers[selected].drawSelected(window,x,y);

			for(int loop = 0; loop<numplayers; ++loop)  {		
				myplayers[loop].drawPlayer(window, x, y, mycaught-loop);
			}
		}

		void drawPixels(Window & window, int yard)  {
			for(int loop = 0; loop<numplayers; ++loop)  {
				myplayers[loop].drawPixel(window, yard);
				if(loop == selected)
					myplayers[loop].drawPixelSelected(window,yard);
			}
		}

		void move(int number, int xamount, int yamount, int fieldw, int fieldh, int yard,bool tdown)  {
			if(tdown && turbo > 0)
				myplayers[number].move(xamount*1.5, yamount*1.5, fieldw, fieldh, yard, mycaught-number);
			else
				myplayers[number].move(xamount, yamount, fieldw, fieldh, yard, mycaught-number);
		}

		void defenseMove(Team & otherTeam, int fieldw, int fieldh, int yard)  {
			for(int loop = 0; loop<numplayers; ++loop)  {
				if(loop != selected)
					myplayers[loop].defense(otherTeam.givePlayer(loop), fieldw, fieldh, yard);
			}
		}

		void offensiveMove(int fieldw, int fieldh, int yard)  {
			for(int loop = 0; loop<numplayers; ++loop)  {
				if(loop != selected && loop != mycaught-1) // || mycaught)
			  		myplayers[loop].offense(fieldw,fieldh,yard);
			}
		}

		bool lineUp(int fieldw, int fieldh, int yard)  {
			int count = 0;
			for(int loop = 0; loop < numplayers; ++loop)   {
				if(myplayers[loop].moveToLocation(initxpos,fieldh*(1+loop)/9,fieldw,fieldh,yard))
					count++;
			}
			if( (count-numplayers+1) > 0)
				return true;
			else
				return false;
		}

		void decisiveMove(Frisbee & disc, Team & otherTeam, int pulled, int fieldw, int fieldh, int yard)  {
			if(status == 1 && pulled) 
				offensiveMove(fieldw, fieldh, yard);
			if(status == 2 && pulled)
				defenseMove(otherTeam, fieldw, fieldh, yard);
			//if(status == 3)
			//	lineUp(fieldw, fieldh, yard);

		}

		void sortClosest(Frisbee & disc)  {
			int loopone, looptwo;
			int temp;
			
			for (loopone = 0; loopone < numplayers; loopone++) {  // Go through 10 times
				for (looptwo = 0; looptwo < numplayers-1; looptwo++)  // Check all elements
					if(disc.dist(myplayers[distance[looptwo]]) > disc.dist(myplayers[distance[looptwo+1]]) )  {     // except last 
						temp = distance[looptwo];
						distance[looptwo] = distance[looptwo+1];
						distance[looptwo+1] = temp;
					}
			}

			if(distance[0] != (mycaught-1))
				closest = distance[0];
			else
				closest = distance[1];
		}

		double findRotation()  {
			int x1, y1;
			int x2, y2;
			int junk;

			myplayers[mycaught-1].giveCords(x1,y1,junk,junk,junk);
			myplayers[selected].giveCords(x2,y2,junk,junk,junk);
			x2 += 3*myplayers[selected].getMoveX();
			y2 += 3*myplayers[selected].getMoveY();

			if(x2 >= x1)
				return atan( double(y2-y1)/double(x2-x1) );
			else
				return (PI + atan( double(y2-y1)/double(x2-x1) ) ); 
		}

		void drainTurbo()  {
			if(turbo>-100)
				turbo -= 3;
		}
		void replenishTurbo()  {
			if(turbo < 210)
				turbo += 1;
		}
		int getTurbo()  {
			if(turbo > 0)
				return turbo/2;
			return 0;
		}


		void jump(int number)  {
			myplayers[number].jump();
		}

		void updateHeight()  {
			for(int loop = 0; loop<numplayers; ++loop)  {
				myplayers[loop].updateHeight();
			}
		}

		int isClosest()  {
			return closest;
		}

		Player & givePlayer(int number)  {
				return myplayers[number];
		}

		int number()  {
			return numplayers;
		}

		void changeStatus(int change)  {
			status = change;
		}

		int getStatus() {
			return status;
		}

		int getCaught() {
			return mycaught;
		}
		
		void changeCaught(int caught)  {
			mycaught = caught;
		}

		void clearCaught()  {
			//myplayers[mycaught-1].willCatch(false);
			mycaught = false;
		}

		int getInit()  {
			return active;
		}

		void changeInit(int init)  {
			active = init;
		}

		int getScore()  {
			return score;
		}

		void changeIXpos(int xpos)  {
			initxpos = xpos;
		}
		
		int getIXpos()  {
			return initxpos;
		}

		void nextSelected() {
			selected++;
			if(selected >= numplayers)
				selected = 0;
		}

		int isSelected() {
			return selected;
		}

};


#endif

			
			/*for (loopone  = 0; loopone < numplayers; loopone++) {
				tempone = distance[loopone];
				temptwo = loopone;
		        // insertion point found.
				for (looptwo = loopone+1; looptwo < numplayers; looptwo++)  {
					if(disc.dist(myplayers[distance[temptwo]]) > disc.dist(myplayers[distance[looptwo]]))
						temptwo = looptwo;
				}
		        // insert
		        distance[loopone] = distance[looptwo];
				distance[looptwo] = tempone;
			}*/