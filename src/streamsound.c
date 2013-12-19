/*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyright(C) 2006-2013  MRevenga
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
**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <pthread.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <unistd.h>
#include "streamsound.h"


#define BUFFER_MUSIC_SIZE 256
#define NUM_BUFFER_MUSIC 64 


ALuint *streamsource;
  ALuint *streambuffers;/* [NUM_BUFFER_MUSIC]; */

static void reportError(void){
  
  fprintf (stderr, "ALUT error: %s\n",
           alutGetErrorString (alutGetError ()));
  /*  exit (EXIT_FAILURE); */
}


void *Stream(void *);


struct StreamedSound * StreamSound (char *filename,int mode){
  /* 
     play the ogg sound-file filename with streaming
   */

  pthread_t t;
  pthread_attr_t attr;
  struct StreamedSound *stream_param;
  int i;
  ALint dec_music[BUFFER_MUSIC_SIZE];
  OggVorbis_File *music;
  int current = -1;
  ALenum format;
  vorbis_info *information = NULL;


  
  stream_param=malloc(sizeof(struct StreamedSound));
  if(stream_param==NULL){ 
    fprintf(stderr,"ERROR in malloc StreamSound()\n"); 
    exit(-1); 
  } 
  
  music=malloc(sizeof(OggVorbis_File));
  if(music==NULL){ 
    fprintf(stderr,"ERROR in malloc StreamSound()\n"); 
    exit(-1); 
  }
  
  streambuffers=malloc(NUM_BUFFER_MUSIC*sizeof(ALuint));
  if(streambuffers==NULL){ 
    fprintf(stderr,"ERROR in malloc StreamSound()\n"); 
    exit(-1); 
  } 
  
  streamsource=malloc(sizeof(ALuint));
  if(streamsource==NULL){ 
    fprintf(stderr,"ERROR in malloc StreamSound()\n"); 
    exit(-1); 
  } 
  
  pthread_attr_init (&attr);
  
  if ( ov_fopen ( filename, music ) < 0 ){
    fprintf (stderr,"This is not an ogg file\n");
    exit(1);
  }
  
  /* Get info about the file */
  information = ov_info ( music, -1 );
  
  if (( information->channels ) == 2 ){
    format = AL_FORMAT_STEREO16;
  }else{
    format = AL_FORMAT_MONO16;
  }
  
  /* Buffers and source */
  alGenBuffers ( NUM_BUFFER_MUSIC, streambuffers );
  if ( alGetError() != AL_NO_ERROR ){
    reportError();
    exit(1);
  }
  
  alGenSources ( 1, streamsource );
  if ( alGetError() != AL_NO_ERROR ){
    reportError();
    exit(1);
  }
  
  /* Fill buffers with sound */
  for ( i=0 ; i < NUM_BUFFER_MUSIC ; i++ ){
    ov_read ( music, (char *)dec_music, BUFFER_MUSIC_SIZE, 0, 2, 1, &current) ;
    alBufferData ( streambuffers[i], format, dec_music, BUFFER_MUSIC_SIZE, information->rate );
    if ( alGetError() != AL_NO_ERROR ){
      reportError();
    }
  }
  
  /* buffer to source */
  alSourceQueueBuffers ( streamsource[0], NUM_BUFFER_MUSIC, streambuffers );
  
  /* start playsound */
  alSourcePlay ( streamsource[0] );
  
  /* streaming  */

  stream_param->ogg_file=music;
  stream_param->format=format;
  stream_param->rate=information->rate;
  stream_param->streambuffers=streambuffers;
  stream_param->streamsource=streamsource;
  if(mode==SM_LOOP){
    stream_param->mode=SM_LOOP;
  }
  else{
    stream_param->mode=SM_ONCE;
  }
  stream_param->state=SS_PLAY;
  stream_param->nbuffers=NUM_BUFFER_MUSIC;
  stream_param->buffersize=BUFFER_MUSIC_SIZE;
  stream_param->order=0;
  stream_param->value=0;
  
  pthread_create (&t,&attr,Stream,stream_param);
  return (stream_param); 
}


void *Stream(void /* struct StreamedSound */ *a){

  struct StreamedSound *arg=(struct StreamedSound *)a;
  int procesed_buffers;
  ALuint tmp_buffer;
  int bytes;
  ALint dec_music[BUFFER_MUSIC_SIZE];
  ALenum source_state;
  int current = -1;
  
  ALenum format=arg->format;
  ALfloat rate=arg->rate;
  ALuint *streambuffers=arg->streambuffers;
  ALuint *streamsource=arg->streamsource;
  int nbuffers=arg->nbuffers;
  int buffersize=arg->buffersize;
  


  while ( arg->state!=SS_STOP){
    usleep(50000);    /* 0.05 s */
    
    switch(arg->order){
    case SO_NONE:
      break;
    case SO_SETVOL:
      alSourcef(streamsource[0],AL_GAIN,arg->value); /* music volume */
      break;
    case SO_PLAY:
      arg->state=SS_PLAY;
      break;
    case SO_PAUSE:
      arg->state=SS_PAUSE;
      break;
    case SO_STOP:
      arg->state=SS_STOP;
      break;
    default:
      fprintf(stderr,"ERROR Stream(): not implemented %d\n",arg->order);
      exit(-1);
      break;
    }
    arg->order=SO_NONE;
    
    if(arg->state==SS_PAUSE){
      continue;
    }
    
    /* processed buffers */
    alGetSourcei ( streamsource[0], AL_BUFFERS_PROCESSED, &procesed_buffers );
    
    if ( procesed_buffers > 0){
      
      while ( procesed_buffers ){
	
	alSourceUnqueueBuffers ( streamsource[0], 1, &tmp_buffer );
	if ( alGetError() != AL_NO_ERROR ){
	  reportError();
	  break;
	}
	bytes = ov_read ( (arg->ogg_file), (char *)dec_music, buffersize, 0, 2, 1, &current );
	if ( alGetError() != AL_NO_ERROR ){
	  reportError();
	  break;
	}
	if(bytes==0){ /* EOF */
	  if(arg->mode==SM_LOOP){
	    ov_raw_seek((arg->ogg_file),0);
	    bytes = ov_read ( (arg->ogg_file), (char *)dec_music, buffersize, 0, 2, 1, &current );
	    arg->state=SS_PLAY;
	  }
	  else{
	    arg->state=SS_PAUSE;
	  }
	}
	alBufferData ( tmp_buffer, format, dec_music, buffersize, rate );
	if ( alGetError() != AL_NO_ERROR ){
	  reportError();
	  break;
	}
	alSourceQueueBuffers ( streamsource[0], 1, &tmp_buffer );
	if ( alGetError() != AL_NO_ERROR ){
	  reportError();
	  break;
	}
	procesed_buffers--;
      }
    }
    
    /* Checking the source state */
    alGetSourcei ( streamsource[0], AL_SOURCE_STATE, &source_state );
    if ( source_state != AL_PLAYING ){
      alSourcePlay ( streamsource[0] );
    }
  }
  alSourceStop ( streamsource[0] );
  
  /* Deleting buffers and sources */
  
  alDeleteSources ( 1, streamsource );
  alDeleteBuffers ( nbuffers, streambuffers );
  
  return((void*)NULL);
}


int ExitStreamSound(void){
  int i;


  /* deataching buffer*/
  alSourcei(*streamsource, AL_BUFFER, 0);  
  
  /* Deleting sources */
  alDeleteSources(1,streamsource);  

  /* Deleting buffers */
  for(i=0;i<NUM_BUFFER_MUSIC;i++){
    alDeleteBuffers(1,&streambuffers[i]);
  }
  return (0);
}
