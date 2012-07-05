 /*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyright(C) 2006-2012  MRevenga
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

/*************  SpaceZero  M.R.H. 2006-2012 ******************
		Author: MRevenga
		E-mail: mrevenga at users.sourceforge.net
		version 0.82 Jan 2012
****/


#define BUFFER_MUSIC_SIZE 256
#define NUM_BUFFER_MUSIC 64 


/* Stream states */
#define SS_STOP 0
#define SS_PLAY 1
#define SS_PAUSE 2

/* Steam modes */
#define SM_ONCE 0
#define SM_LOOP 1

/* stream orders */

#define SO_NONE 0
#define SO_SETVOL 1
#define SO_PLAY 2
#define SO_PAUSE 3
#define SO_STOP 4


struct StreamedSound{
  OggVorbis_File *ogg_file;
  ALenum format;
  ALfloat rate;
  ALuint *streambuffers;
  ALuint *streamsource;
  int nbuffers;
  int buffersize;
  int mode;
  int state;
  int order;
  float value;
};


struct StreamedSound * StreamSound (char *filename,int mode);
void *Stream(struct StreamedSound *arg);
