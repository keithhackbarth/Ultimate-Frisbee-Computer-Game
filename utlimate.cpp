const double PI = 3.1415926;

#include <iostream.h>
#include <math.h>
#include "CMU\CarnegieMellonGraphics.h"
#include "sound.h"
#include "field.h"
#include "team.h"
#include "frisbee.h"

//using namespace std;

int main() {


  //constants
  const int WINDOW_WIDTH =  748;//873
  const int WINDOW_HEIGHT = 440;//491
  const int YARDSIZE = 12;
  const int FIELDW = (131*YARDSIZE);
  const int FIELDH = (41*YARDSIZE);
  const int ENDZONE = (25*YARDSIZE);
  const int NUMPLAYERS = 12;

  //variables
  int x = FIELDW-WINDOW_WIDTH;
  int y = 0;
  int loop = 0;
  int stall = 0;
  double rotation = 0;
  double height = -1*PI/6 ;
  float power = 0;
  bool direction = true;
  bool zkey = false;
  bool xkey = false;
  bool ckey = false;
  bool nkey = false;
  bool mkey = false;
  bool hkey = false;

  Team teams[2];

  cout << "Welcome to the first test of College Ultimate\n";

  Window test(WINDOW_WIDTH,WINDOW_HEIGHT, "College Ultimate Test", false);

  Field stndfield(YARDSIZE, FIELDW, FIELDH, ENDZONE);
										//255
  teams[0].initialize(275, FIELDH, Color(255,255,255), true, "guy1runleft.png","guy1runright.png","guy1standleft.png","guy1standright.png");
  teams[1].initialize(1275, FIELDH, Color(173,0,0), false, "guy2runleft.png","guy2runright.png","guy2standleft.png","guy2standright.png");
	  
  Frisbee disc(290,FIELDH/2);
  
  test.disableAutoPageFlip();

  TimerEvent clock = test.startTimer(25); 

  //openMidi("town.mid","town"); 
  //play("town",false); 

  //game loop
  while(true) {
	test.waitForTimerEvent();

	stndfield.drawField(test, x, y);
		
	if(test.isKeyDown(NamedKey::ESCAPE)) 
		return 0;

	disc.centerWindow(x,y,test);

	//See who has caught disc (defense has priority)
	if(teams[0].getStatus() == 2)  {
		if(teams[0].getIXpos() < (FIELDW/2) )  {
			teams[0].hasCaught(teams[1], disc, teams[0].getCaught()+teams[1].getCaught(), 
				rotation, teams[0].getInit()+teams[1].getInit(), FIELDW-ENDZONE, FIELDW, YARDSIZE, FIELDH, stall);
			teams[1].hasCaught(teams[0], disc, teams[0].getCaught()+teams[1].getCaught(), 
				rotation, teams[0].getInit()+teams[1].getInit(), YARDSIZE, ENDZONE+YARDSIZE, YARDSIZE, FIELDH, stall);
		}
		else  {
			teams[0].hasCaught(teams[1], disc, teams[0].getCaught()+teams[1].getCaught(), 
				rotation, teams[0].getInit()+teams[1].getInit(), YARDSIZE, ENDZONE+YARDSIZE, YARDSIZE, FIELDH, stall);
			teams[1].hasCaught(teams[0], disc, teams[0].getCaught()+teams[1].getCaught(), 
				rotation, teams[0].getInit()+teams[1].getInit(), FIELDW-ENDZONE, FIELDW, YARDSIZE, FIELDH, stall);
		}
	}
	else  {
		if(teams[0].getIXpos() < (FIELDW/2) )  {
			teams[0].hasCaught(teams[1], disc, teams[0].getCaught()+teams[1].getCaught(), 
				rotation, teams[0].getInit()+teams[1].getInit(), FIELDW-ENDZONE, FIELDW, YARDSIZE, FIELDH, stall);
			teams[1].hasCaught(teams[0], disc, teams[0].getCaught()+teams[1].getCaught(), 
				rotation, teams[0].getInit()+teams[1].getInit(), YARDSIZE, ENDZONE+YARDSIZE, YARDSIZE, FIELDH, stall);
		}
		else  {
			teams[0].hasCaught(teams[1], disc, teams[0].getCaught()+teams[1].getCaught(), 
				rotation, teams[0].getInit()+teams[1].getInit(), YARDSIZE, ENDZONE+YARDSIZE, YARDSIZE, FIELDH, stall);
			teams[1].hasCaught(teams[0], disc, teams[0].getCaught()+teams[1].getCaught(), 
				rotation, teams[0].getInit()+teams[1].getInit(), FIELDW-ENDZONE, FIELDW, YARDSIZE, FIELDH, stall);
		}
	}

	//if disc is in air, move disc
	if(!(teams[0].getCaught()+teams[1].getCaught()) )  {
		disc.move(rotation,power,height, FIELDW, FIELDH, YARDSIZE, ENDZONE, teams[0].getInit()+teams[1].getInit()-1);
	}
	else  {
		//////throw sceen
		//////move arrows
		if(teams[1].getCaught() && (teams[1].getCaught()-1 == teams[1].isSelected()) )  {
			if(test.isKeyDown(NamedKey::LEFT_ARROW)) {
				  rotation -= (0.075);
			}
			if(test.isKeyDown(NamedKey::RIGHT_ARROW)) {
				  rotation += (0.075);
			}
			
		}
		if(teams[0].getCaught() && (teams[0].getCaught()-1 == teams[0].isSelected()) )  {
			if(test.isKeyDown('1')) {
				  rotation -= (0.075);
			}
			if(test.isKeyDown('3')) {
				  rotation += (0.075);
			}
			//if(test.isKeyDown('5')) {
			//	if(height > -1*PI/6 )
			//		height -= (0.075);
			//}
			//if(test.isKeyDown('2')) {
			//	if(height < -0.075 ) 
			//		height += (0.075);
			//}
		}

		//rotate to selected player
		if(teams[0].getCaught() && (teams[0].getCaught()-1 != teams[0].isSelected()) && teams[0].getInit()) {
			rotation = teams[0].findRotation();
		}
		if(teams[1].getCaught() && (teams[1].getCaught()-1 != teams[1].isSelected()) && teams[1].getInit()) {
			rotation = teams[1].findRotation();
		}
		//draw arrow
		if(teams[0].getCaught())
			disc.drawArrow(test, teams[0].givePlayer(teams[0].getCaught()-1), x, y, rotation);
		if(teams[1].getCaught())
			disc.drawArrow(test, teams[1].givePlayer(teams[1].getCaught()-1), x, y, rotation);
		
		//first throw push
		if(test.isKeyDown('x') && !xkey && teams[1].getCaught() && !ckey) {
			xkey = true;
			power = 0;
			height = -0.075;
		}
		if(test.isKeyDown('c') && !ckey && teams[1].getCaught() && !xkey) {
			ckey = true;
			power = 0;
			height = -1*PI/6;
		}
		if(test.isKeyDown('9') && !mkey && teams[0].getCaught() && !hkey) {
			mkey = true;
			power = 0;
			height = -1*PI/6;
		}
		if(test.isKeyDown('8') && !hkey && teams[0].getCaught() && !mkey) {
			hkey = true;
			power = 0;
			height = -0.075;
		}
		//second throw hold
		if( (test.isKeyDown('x') && xkey && teams[1].getCaught()) || (test.isKeyDown('c') && ckey && teams[1].getCaught() != false)) {
			if(power<8.9)
				power += (float)(.15);
		}
		if( (test.isKeyDown('8') && hkey && teams[0].getCaught()) || (test.isKeyDown('9') && mkey && teams[0].getCaught() != false)) {
			if(power<8.9)
				power += (float)(.15);
		}
		//third throw release
		if((!test.isKeyDown('8') && hkey && teams[0].getStatus()!=2 && teams[0].getCaught() ) ||
			(!test.isKeyDown('9') && mkey && teams[0].getStatus()!=2 && teams[0].getCaught() ))  {
			hkey = false;
			mkey = false;
			if( teams[0].getInit() == false && teams[0].getCaught() )  {
				teams[0].changeInit(true);
				teams[0].changeStatus(2);
			}			
			teams[0].clearCaught();
			teams[1].clearCaught();
			disc.throwFrisbee();
			for(loop = 0; loop<7; ++loop)  {
				disc.move(rotation,power, height, FIELDW, FIELDH, YARDSIZE, ENDZONE, 1);
			}
			PlaySound("whoosh.wav",NULL,SND_FILENAME|SND_ASYNC);
		}
		if((!test.isKeyDown('x') && xkey && teams[1].getStatus()!=2 && teams[1].getCaught() )  ||
			(!test.isKeyDown('c') && ckey && teams[1].getStatus()!=2 && teams[1].getCaught() ))   {
			ckey = false;
			xkey = false;
			if( teams[1].getInit() == false && teams[1].getCaught() )  {
				teams[1].changeInit(true);
				teams[1].changeStatus(2);
			}
			teams[0].clearCaught();
			teams[1].clearCaught();			
			disc.throwFrisbee();
			for(loop = 0; loop<7; ++loop)  {
				disc.move(rotation,power, height, FIELDW, FIELDH, YARDSIZE, ENDZONE, 1);
			}
			PlaySound("whoosh.wav",NULL,SND_FILENAME|SND_ASYNC);
		}
		/////////////////////end throw section////////
	}

	//turbo
	if(test.isKeyDown('0'))
		teams[0].drainTurbo();
	else
		teams[0].replenishTurbo();
	if(test.isKeyDown('v'))
		teams[1].drainTurbo();
	else
		teams[1].replenishTurbo();

	
	//if score move disc to endzone line
	if(teams[0].getStatus() == 3 && teams[1].getStatus() != 3 && teams[0].getInit() != false)
		disc.changeInit(teams[0].getIXpos()+30, FIELDH/2);
	if(teams[1].getStatus() == 3 && teams[0].getStatus() != 3 && teams[1].getInit() != false)
		disc.changeInit(teams[1].getIXpos()+30, FIELDH/2);

	//if score run back to init location
	if(teams[0].getStatus() == 3 || teams[1].getStatus() == 3)  {
		teams[0].changeStatus(3);
		teams[1].changeStatus(3);
		teams[0].clearCaught();
		teams[1].clearCaught();
		teams[0].changeInit(false);
		teams[1].changeInit(false);
		if(teams[0].lineUp(FIELDW, FIELDH, YARDSIZE) )
			teams[0].changeStatus(0);
		if(teams[1].lineUp(FIELDW, FIELDH, YARDSIZE) )
			teams[1].changeStatus(0);
		disc.restart();
	}
	
	//sort closest to disc
	//teams[0].sortClosest(disc);
	//teams[1].sortClosest(disc);

	//select next player
	if(test.isKeyDown('z') && !zkey) {
		teams[1].nextSelected();
		zkey = true;
	}
	if(test.isKeyDown('7') && !nkey)  {
		teams[0].nextSelected();
		nkey = true;
	}
	//reset select
	if(!test.isKeyDown('z') && zkey)
		zkey = false;
	if(!test.isKeyDown('7') && nkey)
		nkey = false;

	//jump
	if(test.isKeyDown('9') && !teams[0].getCaught())
		teams[0].jump(teams[0].isSelected());
	if(test.isKeyDown('c') && !teams[1].getCaught())
		teams[1].jump(teams[1].isSelected());

	//move selected players
	if(teams[0].getInit()+teams[1].getInit() && (teams[1].getCaught()-1!= teams[1].isSelected()) )  {
		teams[1].move(teams[1].isSelected(), 0, 0, FIELDW, FIELDH, YARDSIZE,false);
		if(test.isKeyDown(NamedKey::LEFT_ARROW))  {
			teams[1].move(teams[1].isSelected(), -teams[1].givePlayer(0).getSpeed(), 0, FIELDW, FIELDH, YARDSIZE,test.isKeyDown('v'));
		}
		if(test.isKeyDown(NamedKey::RIGHT_ARROW))  {
			teams[1].move(teams[1].isSelected(), teams[1].givePlayer(0).getSpeed(), 0, FIELDW, FIELDH, YARDSIZE,test.isKeyDown('v'));
		}
		if(test.isKeyDown(NamedKey::UP_ARROW))  {
			teams[1].move(teams[1].isSelected(), 0,-teams[1].givePlayer(0).getSpeed(), FIELDW, FIELDH, YARDSIZE,test.isKeyDown('v'));
		}
		if(test.isKeyDown(NamedKey::DOWN_ARROW))  {
			teams[1].move(teams[1].isSelected(), 0, teams[1].givePlayer(0).getSpeed(), FIELDW, FIELDH, YARDSIZE,test.isKeyDown('v'));
		}
	}
	if(teams[0].getInit()+teams[1].getInit() && (teams[0].getCaught()-1!=teams[0].isSelected()) )  {
		teams[0].move(teams[1].isSelected(), 0, 0, FIELDW, FIELDH, YARDSIZE,false);
		if(test.isKeyDown('1'))  {
			  teams[0].move(teams[0].isSelected(), -teams[0].givePlayer(0).getSpeed(), 0, FIELDW, FIELDH, YARDSIZE,test.isKeyDown('0'));
		}
		if(test.isKeyDown('3'))  {
			 teams[0].move(teams[0].isSelected(), teams[0].givePlayer(0).getSpeed(), 0, FIELDW, FIELDH, YARDSIZE,test.isKeyDown('0'));
		}
		if(test.isKeyDown('5'))  {
			  teams[0].move(teams[0].isSelected(), 0, -teams[0].givePlayer(0).getSpeed(), FIELDW, FIELDH, YARDSIZE,test.isKeyDown('0'));
		}
		if(test.isKeyDown('2'))  {
			  teams[0].move(teams[0].isSelected(), 0, teams[0].givePlayer(0).getSpeed(), FIELDW, FIELDH, YARDSIZE,test.isKeyDown('0'));
		}
	}
	
	//movement of players under computer control - AI
	teams[0].decisiveMove(disc, teams[1], teams[0].getInit()+teams[1].getInit(), FIELDW, FIELDH, YARDSIZE);
	teams[1].decisiveMove(disc, teams[0], teams[0].getInit()+teams[1].getInit(), FIELDW, FIELDH, YARDSIZE);

	//updateheight of players
	teams[0].updateHeight();
	teams[1].updateHeight();

	//stall count
	if(teams[0].getInit() && teams[1].getInit() && (teams[0].getCaught() || teams[1].getCaught()) )  {
		if(stall < 400)
			stall += 1;
		else  {
			PlaySound("bass.wav",NULL,SND_FILENAME|SND_ASYNC);
			power = 0;
			disc.drop();
			teams[0].clearCaught();
			teams[1].clearCaught();
			stall = 0;
		}
		if(stall==320 || stall==360)
			PlaySound("beep.wav",NULL,SND_FILENAME|SND_ASYNC);
	}
	else 
		stall = 0;

	//draw players
	teams[0].drawTeam(test, x, y);
	teams[1].drawTeam(test, x, y);

	disc.drawFrisbee(test, x, y);

	//Scoreboard
	//test.drawRectangleFilled(Style::BLACK, 0, test.getHeight()*6/7, test.getWidth(), test.getHeight() );
	test.drawText( Style(Color::WHITE, 2), Font(Font::ROMAN, 16), 5, test.getHeight()-42, "UCSD B"); 
	test.drawText( Style(Color::WHITE, 2), Font(Font::ROMAN, 16), 160, test.getHeight()-42, test.numberToString(teams[0].getScore() ) ); 
	test.drawText( Style(Color::WHITE, 2), Font(Font::ROMAN, 16), 5, test.getHeight()-17, "Stanford B"); 
	test.drawText( Style(Color::WHITE, 2), Font(Font::ROMAN, 16), 160, test.getHeight()-17, test.numberToString(teams[1].getScore() ));

	//Powerbar
	test.drawText( Style(Color::WHITE, 1.5), Font(Font::ROMAN, 10), test.getWidth()/3+40, test.getHeight()-40, "Power:");
	test.drawRectangleFilled(Style(Color(255,145,0), 1), test.getWidth()/3-10, test.getHeight()-33, test.getWidth()/3-10+(power*16.666), test.getHeight()-21);
	test.drawRectangleOutline(Style(Color::WHITE, 1), test.getWidth()/3-10, test.getHeight()-33, test.getWidth()/3 + 140, test.getHeight()-21);
	
	//Stall Count
	test.drawText( Style(Color::WHITE, 1.5), Font(Font::ROMAN, 14), test.getWidth()*3/5, test.getHeight()-35, "Stall:");
	test.drawText( Style(Color::WHITE, 3), Font(Font::ROMAN, 16), test.getWidth()*3/5+18, test.getHeight()-15, test.numberToString(stall/40) );
	
	//Turbo Bar
	test.drawRectangleFilled(Style(Color(255,155,0), 1), 15, 7, 15+teams[0].getTurbo(), 18);
	test.drawRectangleFilled(Style(Color(255,155,0), 1), test.getWidth()-125, 7, test.getWidth()-125+teams[1].getTurbo(), 18);
	
	//Heightbar
	//test.drawLine( Style(Color::WHITE, 5), test.getWidth()/3 +202.5+(27.5*cos(height)), test.getHeight()-35+(27.5*sin(height)),
	//test.getWidth()/3+202.5+(27.5*cos(height+PI)), test.getHeight()-35+(27.5*sin(height+PI)));
	
	//Field Map
	test.drawRectangleFilled(Style::GREEN, test.getWidth()*3/4, test.getHeight()-55, test.getWidth()*3/4+(FIELDW/YARDSIZE), test.getHeight()-55+(FIELDH/YARDSIZE));
	test.drawRectangleOutline(Style(Color::WHITE, 1), test.getWidth()*3/4, test.getHeight()-55, test.getWidth()*3/4+(FIELDW/YARDSIZE), test.getHeight()-55+(FIELDH/YARDSIZE)); 
	test.drawRectangleOutline(Style(Color::WHITE, 1), test.getWidth()*3/4+(ENDZONE/YARDSIZE), test.getHeight()-55, test.getWidth()*3/4+(FIELDW-ENDZONE)/YARDSIZE, test.getHeight()-55+(FIELDH/YARDSIZE));

	//Draw Pixels for mini-map
	teams[0].drawPixels(test, YARDSIZE);
	teams[1].drawPixels(test, YARDSIZE);

	disc.drawPixel(test, YARDSIZE);

	test.flipPage();

  }

  return 0;
}


  /*  
  guy[1].initialize(400, 400, 20, 32, Color(15,145, 125), "guyrun2.txt", "guystand2.txt");
  guy[2].initialize(1000, 288, 20, 32, Color(0,0,0), "guyrun2.txt", "guystand2.txt");
  guy[3].initialize(1400, 0, 20, 32, Color(160,220, 200), "guyrun2.txt", "guystand2.txt");
  guy[4].initialize(768, 175, 20, 32, Color(248, 237, 12), "guyrun2.txt", "guystand2.txt");
  guy[5].initialize(600, 50, 20, 32, Color(55, 173, 59), "guyrun2.txt", "guystand2.txt");*/
  
  //disc.drawArrow(test, teams[0].givePlayer(caught-1), x, y, rotation);
  	/*for(loop = 0; loop<NUMPLAYERS; ++loop)  {		
		if(!caught)
			if(disc.hasCaught(guy[loop]) )
				if(guy[loop].hasCaught() )  {
					caught = loop+1;
					if(guy[loop].giveDirec())
						rotation = 0;
					else
						rotation = PI;
				}
	}*/

	//draw
	//for(loop = 0; loop<NUMPLAYERS; ++loop)  {		
	//	guy[loop].drawPlayer(test, x, y);
	//}

	//test.drawArc( Style(Color::WHITE, 2), test.getWidth()/3 + 190, 500, test.getWidth()/3+230, 430, 0, 80);
		//for(loop = 0; loop<NUMPLAYERS; ++loop)  {
	//	guy[loop].drawPixel(test, YARDSIZE);
	//}
	
  /*for(loop = 0; loop < NUMPLAYERS/2; ++loop)   {
	  guy[loop].initialize(275,FIELDH*(1+loop)/9,Color(255,255,255), true, "guy1runleft.png", "guy1runright.png", "guy1standleft.png", "guy1standright.png");
  }
  for(loop = NUMPLAYERS/2; loop < NUMPLAYERS; ++loop)   {
	  guy[loop].initialize(1275,FIELDH*(1+loop-NUMPLAYERS/2)/9,Color(0,0,0), false, "guy2runleft.png", "guy2runright.png", "guy2standleft.png", "guy2standright.png");
  }*/

  	/*	//movement with keys
	if(pulled)  {
		if(test.isKeyDown('a'))  {
			  guy[NUMPLAYERS/2+3].move(-8, 0, FIELDW, FIELDH, YARDSIZE);
		}
		if(test.isKeyDown('d'))  {
			  guy[NUMPLAYERS/2+3].move(8, 0 , FIELDW, FIELDH, YARDSIZE);
		}
		if(test.isKeyDown('w'))  {
			  guy[NUMPLAYERS/2+3].move(0, -8, FIELDW, FIELDH, YARDSIZE);
		}
		if(test.isKeyDown('s'))  {
			  guy[NUMPLAYERS/2+3].move(0, 8 , FIELDW, FIELDH, YARDSIZE);
		}*/

			//defense move
		//for(loop = 0; loop<(NUMPLAYERS/2); ++loop)  {
		//    guy[loop].defense(guy[loop+NUMPLAYERS/2], FIELDW, FIELDH, YARDSIZE);
		//}

			//offensive move
		//for(loop = NUMPLAYERS/2; loop<(NUMPLAYERS); ++loop)  {
		//   if(loop != NUMPLAYERS/2+3)
		//		guy[loop].offense(FIELDW, FIELDH, YARDSIZE);
		//}

		//guy[caught-1].willCatch(false);

			/*if(!caught)  { 
		teams[1].hasCaught(teams[0], disc, caught, rotation, pulled, YARDSIZE, ENDZONE+YARDSIZE, YARDSIZE, FIELDH, scoretwo);
		if(caught)
			caught += teams[1].number();
	}*/
			/*if(teams[0].getCaught() )
			teams[0].willCatch(teams[0].getCaught()-1, false);
		else
			teams[1].willCatch(teams[1].getCaught()-1, false);
		//caught = false;
		teams[0].changeCaught(false);
		teams[1].changeCaught(false);*/