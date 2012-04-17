#ifndef __SOUND_H__
#define __SOUND_H__



void reportIfError(MCIERROR err) { 
	char errstr[1000]; 
	if (err != 0) { 
	mciGetErrorString(err,errstr,1000); 
	MessageBox(NULL,errstr,"blah",MB_OK); 
	} 
} 

void openMidi(const char *file,const char *alias) { 
	char cmd[1000]; 
	wsprintf(cmd,"open %s type sequencer alias %s",file,alias); 
	MCIERROR err = mciSendString(cmd,NULL,0,NULL); 
	reportIfError(err); 
} 

void openWav(const char *file,const char *alias) { 
	char cmd[1000]; 
	wsprintf(cmd,"open %s alias %s %s",file,alias,""); 
	MCIERROR err = mciSendString(cmd,NULL,0,NULL); 
	reportIfError(err); 
} 

//you might want to define a separate playMidiRepeat function...just add repeat at the end of the command string. 
void play(const char *alias) { 
	char cmd[1000]; 
	wsprintf(cmd,"play %s %s %s",alias,"",""); 
	MCIERROR err = mciSendString(cmd,NULL,0,NULL); 
	reportIfError(err); 
} 

void stop(const char *alias) { 
	char cmd[1000]; 
	wsprintf(cmd,"stop %s %s %s",alias,"",""); 
	MCIERROR err = mciSendString(cmd,NULL,0,NULL); 
	if (err != 0) { 
	mciGetErrorString(err,cmd,1000); 
	MessageBox(NULL,cmd,"blah",MB_OK); 
	} 
} 

#endif