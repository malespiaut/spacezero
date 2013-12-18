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
		version 0.84 april 2013
**************************************************************/

/*
NUM_SOURCES simultaneus sounds
sound server
Aprox. add 4MB in the executable $
version-0.1.5
*/
#include "functions.h"
#include "sound.h"
#include "general.h"
#include "streamsound.h"

#define SOUNDDEBUG FALSE

#if DEBUG
int debugsound=0;
#endif

char filesoundnames[NUM_SOUNDS][MAXTEXTLEN];
char *soundnames[NUM_SOUNDS]={ 
  "music.ogg",
  "bfire.ogg",
  "explos.ogg",
  "thrust.ogg",
  "crash.ogg"};

ALuint sources[NUM_SOURCES];
ALuint buffers[NUM_BUFFERS];
int buffertable[NUM_BUFFERS]; /* buffer id's -- sound id's */

struct Sourcetable sourcetable[NUM_SOURCES];

int Ssoundenabled=TRUE;
float Smusicvol=1.0;
float Ssoundvol=1.0;
float Smastervol=1.0;

struct StreamedSound *music=NULL;

ALvoid* LoadMemoryFromOggFile(char *fname,ALenum *format,ALsizei *size,ALfloat *frequency);

static void reportError(void){
  
  fprintf (stderr, "ALUT error: %s\n",
           alutGetErrorString (alutGetError ()));
  /*  exit (EXIT_FAILURE); */
}


int InitSound(void){
  /*
    returns:


   */

  int i,j;
  int error;
  char *datadir;
  FILE *fp;
  ALenum format; 
  ALsizei size; 
  ALfloat frequency; 
  ALvoid *data;

  ALfloat sourcePos[] = {0.0f, 0.0f, 0.0f};
  ALfloat listenerPos[]={0.0,0.0,0.0};/* At the origin */
  ALfloat listenerVel[]={0.0,0.0,0.0};/* The velocity (no doppler here) */
  ALfloat listenerOri[]={0.0,0.0,-1.0, 0.0,1.0,0.0};/* LookAt then Up */


  /*****
   checking file sound directory 
	
   Sound data dir defined in Makefile.
   
   First look in ./dat (no previous instalation or own defined
   sounds.)
   
   If the directory ./dat doesn't exists, look in the directory
   defined by INSTALL_DATA_DIR
   
  *****/ 

  datadir=DATADIR;
  snprintf(filesoundnames[0],MAXTEXTLEN,"%s/sounds/%s",datadir,soundnames[0]);
  
  printf("Checking for sound(%d): %s\n",0,filesoundnames[0]);
  

  if((fp=fopen(filesoundnames[0],"rb"))==NULL){
    fprintf(stdout,"Can't open the file: %s\n", filesoundnames[0]);
    
    datadir=INSTALL_DATA_DIR;
    snprintf(filesoundnames[0],MAXTEXTLEN,"%s/sounds/%s",datadir,soundnames[0]);
    printf("checking for sound 2 (%d):%s\n",0,filesoundnames[0]);
    
    if((fp=fopen(filesoundnames[0],"rb"))==NULL){
      fprintf(stdout,"Can't open the file: %s\n", filesoundnames[0]);
      Ssoundenabled=FALSE;  
      return(1);
    }
  }
  printf("Sound(0) file OK\n");

  /* --checking file sound directory */


  /* Clear Error Code (so we can catch any new errors) */
  alutGetError();
  if (!alutInit (0,NULL)){
    reportError ();
  }

  for(i=0;i<NUM_SOURCES;i++){
    sourcetable[i].source=-1;
    sourcetable[i].buffer=-1;
    sourcetable[i].sid=-1;
  }

  /* Create the buffers */
  alGenBuffers(NUM_BUFFERS, buffers);
  if ((error = alGetError()) != AL_NO_ERROR){
    printf("alGenBuffers : %d", error);
    Ssoundenabled=FALSE;
    return(2);
  }


#if SOUNDDEBUG
  printf("InitSound(): buffers created:\n");
  for(i=0;i<NUM_BUFFERS;i++){
    printf("\t buffer: %d %d\n",i,buffers[i]);
  }  
  printf("\n");
#endif

  /* Generate the sources */
  alGenSources(NUM_SOURCES, sources);
  if ((error = alGetError()) != AL_NO_ERROR){
    printf("alGenSources : %d", error);
    reportError ();
    Ssoundenabled=FALSE;
    return(3);
  }

  for(i=0;i<NUM_SOURCES;i++){
    sourcetable[i].source=sources[i];
  }

  /*
     position of the source 
   */
  alSourcefv(sources[0], AL_POSITION, sourcePos);
  alSourcefv(sources[1], AL_POSITION, sourcePos);
  
  /*
    position of the listener 
   */
  alListenerfv(AL_POSITION,listenerPos); 
  alListenerfv(AL_VELOCITY,listenerVel); 
  alListenerfv(AL_ORIENTATION,listenerOri); 

  /* soundfiles */

  for(i=0;i<NUM_SOUNDS;i++){
    snprintf(filesoundnames[i],MAXTEXTLEN,"%s/sounds/%s",datadir,soundnames[i]);
    printf("\tsound(%d): %s\n",i,filesoundnames[i]);
  }
  
  /* loading the buffers */
  for(i=0;i<NUM_BUFFERS;i++){
    buffertable[i]=-1;
  }

  for(i=1,j=1;i<NUM_BUFFERS && i<NUM_SOUNDS;i++){
    data=LoadMemoryFromOggFile(filesoundnames[i],&format,&size,&frequency);
    if(data==NULL){
      fprintf(stderr,"Warning: file not found or corrupted: %s\n",filesoundnames[i]); 
      Ssoundenabled=FALSE;
      return(4);
    }
    if(data!=NULL){
      alBufferData(buffers[j],format,data,size,frequency);
#if SOUNDDEBUG
      printf("InitSound(): sound added: %s buffer(%d): %d data: %p \n",
	     filesoundnames[i],j,buffers[j],(void *)data);
#endif
      buffertable[j]=i;
      free(data);
      data=NULL;
      j++;
    }
#if SOUNDDEBUG
    else{
      printf("InitSound(): sound not added: %s data: %p \n",
	     filesoundnames[i],(void *)data);
    }
#endif
  }

  printf("InitSound(): soundenabled: %d \n",Ssoundenabled);
  return(0);
}


int ExitSound(void){
  int i;


  /* deataching buffer*/
  for(i=0;i<NUM_SOURCES;i++){
    alSourcei(sources[i], AL_BUFFER, 0);  
  }

  /* Deleting sources */
  for(i=0;i<NUM_SOURCES;i++){
    alDeleteSources(1,&sources[i]);  
  }

  /* Deleting buffers */
  for(i=0;i<NUM_BUFFERS;i++){
    alDeleteBuffers(1,&buffers[i]);
  }

  ExitStreamSound();

  /* exiting */
  


  alGetError();

  if (!alutExit()){
    reportError ();
  }
  return (0);
}


int PlaySound(int sid,int mode,float vol){

  int error;
  ALint sourceState;
  int i;
  int bufferid,sourceid,bufferfree,sourcefree;
  int cleandone=0;
  ALenum format; 
  ALsizei size; 
  ALfloat frequency; 
  ALvoid *data;

#if SOUNDDEBUG
  ALfloat sourceValue;
#endif


#if SOUNDDEBUG
  printf("PlaySound(): Trying sound id: %d\n",sid);
#endif

  if(Ssoundenabled==FALSE)return(1);

  if(vol<0||vol>1)return(2);

  if(sid!=MUSIC){
    if(GameParametres(GET,GSOUND,0)==FALSE){
      return(1);
    }
  }

  if(sid==MUSIC){
    if(music!=NULL){
      fprintf(stderr,"ERROR: music not NULL\n");
      exit(-1);
    }
    music=StreamSound(filesoundnames[0],SM_LOOP);
    return(0);
  }


  bufferid=-1;
  sourceid=-1;
  bufferfree=-1;
  sourcefree=-1;

  /* find the sound buffer */
  /* it is already loaded?*/
  for(i=0;i<NUM_BUFFERS;i++){
    if(buffertable[i]==sid){
      bufferid=i;
      break;
    }
  }

  if(bufferid==-1){
    /* is the buffer free ?*/
    do{
      for(i=0;i<NUM_BUFFERS;i++){
	if(buffertable[i]==-1){bufferfree=i;break;} /* free buffer  */
      }
      
      if(bufferfree==-1){ /* cleaning buffers */
	bufferfree=CleanBuffers();
      }
      
      if( bufferfree==-1)return(3);
      cleandone++;
    }
    while(cleandone<1);
    
    /* load the buffer */
    data=LoadMemoryFromOggFile(filesoundnames[sid],&format,&size,&frequency);
    if(data==NULL){
      fprintf(stderr,"Warning PlaySound(): file not found or corrupted: %s\n",filesoundnames[bufferfree]); 
      return(4);
    }
    
    if(data!=NULL){
      alBufferData(buffers[bufferfree],format,data,size,frequency);
      buffertable[bufferfree]=sid;
      bufferid=bufferfree;
#if SOUNDDEBUG
      printf("PlaySound():sound added: %s to buffer: %d(%d) in data: %p\n",
	     filesoundnames[buffertable[bufferfree]],buffers[bufferfree],bufferfree,(void *)data);
#endif
      free(data);
      data=NULL;
    }
  }/* if(bufferid==-1) */
#if SOUNDDEBUG
  else{
    printf("PlaySound():Sound id: %d already in buffer %d\n",sid,bufferid);      
  }
#endif



  /* has a source asigned ?*/
  for(i=0;i<NUM_SOURCES;i++){
    alGetSourcei(sources[i], AL_BUFFER, &sourceState); /*returns the buffer id
							 0 if is not a buffer associated*/
    if(sourceState==buffers[bufferid]){
      sourceid=i;
      break;
    }
  }

  if(sourceid==-1){/* there are not a assigned source */
    /* look for an unused source */
    for(i=0;i<NUM_SOURCES;i++){
      alGetSourcei(sources[i], AL_BUFFER, &sourceState); /*returns the buffer id
							 0 if is not a buffer associated*/
      if(sourceState==0){
	sourcefree=i;
#if SOUNDDEBUG
	printf("PlaySound():Source free found: %d(%d)\n",sources[i],i);
#endif
	break;
      }
    }
    if(sourcefree==-1){
      /* look for an unused source*/
      for(i=0;i<NUM_SOURCES;i++){
	alGetSourcei(sources[i], AL_SOURCE_STATE, &sourceState);
	if(sourceState!=AL_PLAYING){
	  sourcefree=i;
#if SOUNDDEBUG
	  printf("PlaySound(): Source  %d  is idle\n",i);      
#endif
	  break;
	}
      }
    }
    if(sourcefree==-1)return(5);
    
    sourceid=sourcefree;

    /*Attaching buffers to sources i
     */
#if SOUNDDEBUG
    printf("PlaySound():trying to attach: buffer %d(%d) to source %d(%d)  \n",
	   buffers[bufferid],bufferid,sources[sourceid],sourceid);      
#endif

    alSourcei(sources[sourceid], AL_BUFFER, buffers[bufferid]);
    if ((error = alGetError()) != AL_NO_ERROR){
      printf("alSourcei : %d ", error);
      reportError ();
      return(6);
    }
    if(sourcetable[sourceid].source==sources[sourceid]){
      sourcetable[sourceid].buffer=buffers[bufferid];
      sourcetable[sourceid].sid=sid;
    }
    else{
      printf("PlaySound(): Error sourcetable corrupted\n");
    }

#if SOUNDDEBUG
    printf("PlaySound():buffer %d attached to source %d  \n",bufferid,sourceid);      
#endif
  } /*if(sourceid==-1) */

#if SOUNDDEBUG
  printf("playing sound %d buffer: %d source: %d\n",
	 sid,bufferid,sourceid);      
#endif

  if(mode==SLOOP){
    alSourcei(sources[sourceid],AL_LOOPING,AL_TRUE);
  }
  else{
    alSourcei(sources[sourceid],AL_LOOPING,AL_FALSE);
  }

  alSourcef(sources[sourceid],AL_GAIN,vol*Ssoundvol*Smastervol);
  alSourcePlay(sources[sourceid]);

  /* sound propeties */

#if SOUNDDEBUG
  alGetSourcef(sources[sourceid],AL_GAIN,&sourceValue);
  printf("Gain: %f ",sourceValue);
  alGetSourcef(sources[sourceid],AL_MAX_GAIN,&sourceValue);
  printf("MAX Gain: %f ",sourceValue);
  alGetSourcef(sources[sourceid],AL_MIN_GAIN,&sourceValue);
  printf("MIN Gain: %f ",sourceValue);
  printf("\n");
#endif
  
#if SOUNDDEBUG
  printf("OK\n");
#endif
  return(0);
}


int Wait2FinishSound(void){
  int i; 
  ALint sourceState;
  if(Ssoundenabled==FALSE)return(1);

  for(i=0;i<NUM_SOURCES;i++){
    
    alGetSourcei(sources[i], AL_SOURCE_STATE, &sourceState);
    while(sourceState == AL_PLAYING){
      delay(1);
      alGetSourcei(sources[i], AL_SOURCE_STATE, &sourceState);
    }
  }
  return(0);
}


int CleanBuffers(void){
  /* free memory of the first unused buffer not used for any source */

  int i,j;
  int sw=0;
  int error;
  ALint sourceState;

  for(i=0;i<NUM_BUFFERS;i++){
    sw=0;
    for(j=0;j<NUM_SOURCES;j++){

      alGetSourcei(sources[j], AL_BUFFER, &sourceState); /*returns the buffer id
							 0 if is not a buffer associated*/
      if(sourceState==buffers[i]){
	sw=1;
	break;
      }
    }
    if(sw==0){/* buffer i no attached */
      printf("deleting buffer(%d) in %p\n",i,(void *)&buffers[i]);
      alDeleteBuffers(1,&buffers[i]);      
      if ((error = alGetError()) != AL_NO_ERROR){
	printf("alDeleteBuffers : %d ", error);
      }
      buffertable[i]=-1;

      alGenBuffers(1, &buffers[i]);
      if ((error = alGetError()) != AL_NO_ERROR){
	printf("alGenBuffers : %d", error);
	return 0;
      }
      return(i);
    }
  }
  return(-1); /* not found */
}

int StopSound(int sid){
  int i;
  int bufferid=-1;
  int sourceid=-1;
  int sw=0;
  ALint sourceState;

  if(Ssoundenabled==FALSE)return(1);
  /* found the buffer attached to the sound sid*/

  Sound(SSTOP,sid);
  return(0);

  sw=0;
  for(i=0;i<NUM_BUFFERS;i++){
    if(buffertable[i]==sid){
#if SOUNDDEBUG
      printf("Stop():sound %d in buffer %d\n",sid,i);
#endif
      bufferid=i;
      sw=1;
      break;
    }
  }
  if(sw==0)return(1);
  /* look for an associated source*/
  sw=0;
  for(i=0;i<NUM_SOURCES;i++){

    alGetSourcei(sources[i], AL_BUFFER, &sourceState); /*returns the buffer id
							 0 if is not a buffer associated*/
    if(sourceState==buffers[bufferid]){
      sw=1;
      sourceid=i;
      break;
    }
  }
  if(sw==0)return(2);

#if SOUNDDEBUG
  printf("Stop(): stoping sound %d in buffer %d in source %d\n",sid,bufferid,sourceid);
#endif

  alSourceStop(sources[sourceid]);

  return(0);
}


int Sound(int mode,int sid){
  int i;
  int bufferid=-1;
  int sourceid=-1;
  int sw=0;
  ALint sourceState;
  int status;

  if(Ssoundenabled==FALSE){
    return(1);
  }
  /* found the buffer attached to the sound sid*/
#if DEBUG
  if(debugsound)printf("Sound(): entering Sound() mode: %d sid:%d\n",mode,sid);
#endif
  switch(sid){
  case MUSIC:
    status=GameParametres(GET,GMUSIC,0);
    if(status==FALSE && (mode==SPLAY||mode==SPAUSE)){
#if DEBUG
      if(debugsound)printf("Sound(): no accepting mode\n");
#endif
      return(0);
    }

    status=GameParametres(GET,GSOUND,0);
    if(status==FALSE && (mode==SPLAY||mode==SPAUSE)){
#if DEBUG
      if(debugsound)printf("Sound(): no accepting mode\n");
#endif
      return(0);
    }

    switch(mode){
    case SPAUSE:
      music->state=SS_PAUSE;
#if DEBUG
      if(debugsound) printf("Sound() mode: %d\n",mode);
#endif
      break;
    case SPLAY:
      music->state=SS_PLAY;

#if DEBUG
      if(debugsound)printf("Sound() mode: %d\n",mode);
#endif
      break;
    case SSTOP:
      music->state=SS_PAUSE;

#if DEBUG
      if(debugsound)printf("Sound() mode: %d\n",mode);
#endif
      break;
    default:
    break;
    }

  return(0);
    break;
  default:
    break;
  }

  sw=0;
  for(i=1;i<NUM_BUFFERS;i++){
    if(buffertable[i]==sid){
#if SOUNDDEBUG
      printf("Sound():sound %d in buffer %d\n",sid,i);
#endif
      bufferid=i;
      sw=1;
      break;
    }
  }
  if(sw==0)return(1);
  /* look for an associated source*/
  sw=0;
  for(i=1;i<NUM_SOURCES;i++){

    alGetSourcei(sources[i], AL_BUFFER, &sourceState); /*returns the buffer id
							 0 if is not a buffer associated*/
    if(sourceState==buffers[bufferid]){
      sw=1;
      sourceid=i;
      break;
    }
  }
  if(sw==0)return(2);

#if SOUNDDEBUG
  printf("Sound():  sound %d in buffer %d in source %d\n",sid,bufferid,sourceid);
#endif

  switch(mode){
  case SPAUSE:
    alSourcePause(sources[sourceid]);
#if DEBUG
    if(debugsound) printf("Sound() mode: %d\n",mode);
#endif
    break;
  case SPLAY:
    alSourcePlay(sources[sourceid]);
#if DEBUG
    if(debugsound)printf("Sound() mode: %d\n",mode);
#endif
    break;
  case SSTOP:
    alSourceStop(sources[sourceid]);
#if DEBUG
    if(debugsound)printf("Sound() mode: %d\n",mode);
#endif
    break;
  default:
    break;
  }
  return(0);
}

float SetMasterVolume(float vol,int action){
  /* sets the effects volume */

  ALfloat nvol=0;
  float smastervol0=Smastervol;
  switch(action){
  case VOLSET:
    nvol=vol;
    break;
  case VOLGET:
    return(Smastervol);
    break;
  case VOLINC:
    nvol=Smastervol+vol;
    break;
  default:
    break;
  }
  if(nvol>1)nvol=1;
  if(nvol<0)nvol=0;

  Smastervol=nvol;
  if(Smastervol!=smastervol0){
    SetSoundVolume(Ssoundvol,VOLSET);
    SetMusicVolume(Smusicvol,VOLSET);
  }
  return(Smastervol);
}



float SetSoundVolume(float vol,int action){
  /* sets the effects volume */
  int i;
  ALfloat nvol=0;
  switch(action){
  case VOLSET:
    nvol=vol;
    break;
  case VOLGET:
    return(Ssoundvol);
    break;
  case VOLINC:
    nvol=Ssoundvol+vol;
    break;
  default:
    break;
  }

  if(nvol>1)nvol=1;
  if(nvol<0)nvol=0;

  if(0&&music!=NULL){
    music->value=Smusicvol;
    music->order=SO_SETVOL;
  }
  for(i=1;i<NUM_SOURCES;i++){
    alSourcef(sources[i],AL_GAIN,nvol*Smastervol);
  }
  Ssoundvol=nvol;
  return(Ssoundvol);
}


float SetMusicVolume(float vol,int action){
  /* sets the music volume */
static  ALfloat nvol=0;

  switch(action){
  case VOLSET:
    nvol=vol;
    break;
  case VOLGET:
    return(Smusicvol);
    break;
  case VOLINC:
    if(vol==0)return(Smusicvol);
    nvol=Smusicvol+vol;
    break;
  default:
    break;
  }
  if(nvol>1)nvol=1;
  if(nvol<0)nvol=0;
  
  if(music!=NULL){
    music->value=nvol*Smastervol;
    music->order=SO_SETVOL;
  }
  Smusicvol=nvol;
  return(Smusicvol);
}

ALvoid* LoadMemoryFromOggFile(char *fname,ALenum *format,ALsizei *size,ALfloat *frequency){
  char *data;
  OggVorbis_File sound;
  vorbis_info *information=NULL;
  int bytes_readed=0;
  int bytes;
  int current = -1;
  int buffer_size=0;

  data=malloc(4096*sizeof(char));
  if(data==NULL){ 
    fprintf(stderr,"ERROR in malloc oggLoadMemoryFromFile()\n"); 
    exit(-1); 
  }
  buffer_size=4096;

  if ( ov_fopen ( fname, &sound ) < 0 ){
    fprintf (stderr,"This is not an ogg file\n");
    exit(1);
  }
  information = ov_info ( &sound, -1 );
  if (( information->channels ) == 2 ){
    *format = AL_FORMAT_STEREO16;
  }else{
    *format = AL_FORMAT_MONO16;
  }

  *frequency=information->rate;

  do{
    if(bytes_readed + 4096 > buffer_size){
      data=realloc(data,bytes_readed+4096);
      if(data==NULL){ 
	fprintf(stderr,"ERROR in realloc oggLoadMemoryFromFile()\n"); 
	exit(-1); 
      }
    }
    bytes=ov_read ( &sound, &data[bytes_readed], 4096, 0, 2, 1, &current) ;
    if(bytes>0){
      bytes_readed+=bytes;

    }
  }
  while (bytes>0);
  *size=bytes_readed;
  return(data);
}

