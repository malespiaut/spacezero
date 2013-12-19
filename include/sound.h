 /*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyright (C) 2006-2013  MRevenga
 **
 **  This program is free software; you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License (version 3), or
 **  (at your option) any later version, as published by the Free Software 
 **  Foundation.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program; if not, write to the Free Software
 **  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ******************************************************************************/

/*************  SpaceZero  M.R.H. 2006-2013 ******************
		Author: MRevenga
		E-mail: mrevenga at users.sourceforge.net
		version 0.86 December 2013
****/

#ifndef _SOUND_
#define _SOUND_  

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vorbis/vorbisfile.h>


/* sound modes */
#define SDEFAULT 0
#define SLOOP 1

/* sound */
#define NUM_SOUNDS 5
#define NUM_SOURCES 5
#define NUM_BUFFERS 10

/* sound names */ 
#define MUSIC 0
#define FIRE0 1
#define EXPLOSION0 2
#define THRUST 3
#define CRASH 4


#define SSTOP 0
#define SPLAY 1
#define SPAUSE 2

/* sound actions */

#define VOLSET 0
#define VOLGET 1
#define VOLINC 2

struct Sourcetable{
  ALuint source;
  ALuint buffer;
  int sid;
};

int Sound(int mode,int sid);
int InitSound(void);	/* Program init */
int ExitSound(void);
int PlaySound(int sid,int mode,float vol);
int Wait2FinishSound(void);
int CleanBuffers(void);
int StopSound(int sid);
float SetMasterVolume(float vol,int action);
float SetSoundVolume(float vol,int action);
float SetMusicVolume(float vol,int action);

#endif
