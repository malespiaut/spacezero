 /*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyright(C) 2006-2011  M.Revenga
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

/*************  SpaceZero  M.R.H. 2006-2011 ******************
		Author: M.Revenga
		E-mail: mrevenga at users.sourceforge.net
		version 0.80 May 2011
****/

#include <time.h>
#include "functions.h"
#include "general.h"


float Random(int a){
/*
  returns a random float between (0,1) from the table rtable 
  if a >= 0 set read index to a
  if a= -2 return read index
  if a= -3 recalc random values 
*/
  static float rtable[1000];
  static int n=0;
  static int sw=0;

  if(a>=0){
    n=a;
    if(n>=1000){
      n=n%1000;
    }
  }
  if(a==-2)return(n);
  if(a==-3)sw=0;
  if(sw==0){ /* create table */
    int i;
    for(i=0;i<1000;i++){
      rtable[i]=(float)rand()/RAND_MAX;
    }
    n=0;
    sw=1;
  }
  if(n>=1000)n=0;
  return(rtable[n++]); 
}

void AddGold(struct Player *players,int player,int n){
  players[player].gold+=n;
}
int GetPlayerProc(struct Player *players,int player){
  return(players[player].proc);
}


int Proc(int option,int value){
  static int proc=0;

  switch(option){

  case SET:
    proc=value;
    break;
  case GET:
    return(proc);
    break;
  default:
    fprintf(stderr,"ERROR in Proc(). Option not valid\n");
    exit(-1);
    break;
  }
  return(proc);
}

int GetProc(void){
  return(Proc(GET,0));
}

int SetProc(int value){
  Proc(SET,value);
  return(Proc(GET,0));
}


int NProc(int option,int value){
  static int nproc=1;

  switch(option){

  case SET:
    nproc=value;
    break;
  case GET:
    return(nproc);
    break;
  default:
    fprintf(stderr,"ERROR in NProc(). Option not valid\n");
    exit(-1);
    break;
  }
  return(nproc);
}


int GetNProc(void){
  return(NProc(GET,0));
}

int SetNProc(int value){
  NProc(SET,value);
  return(NProc(GET,0));
}

int GetControl(struct Player *players,int player){
  return(players[player].control);
}

void delay(int time){
  /* 
     time are 1/100 of seconds to wait 
   */

  struct timespec req;
  int sec;

  if(time<=0)return;

  sec=(int)((float)time/100);
  time-=100*sec;
  req.tv_sec=sec;
  req.tv_nsec=time*1000000;
  nanosleep(&req,NULL);
}


int GameParametres(int option,int param,int value){
  /*
    Set or get global game parametres.
    returns:
    reading a parameter:
    value asked.
    -1 if there an error
    setting a parameter:
    0 if there no error.
    -1 if there an error
   */
  static struct Game game;
  int ret=0;

  //  printf("option: %d param: %d value:%d\n",option,param,value);
  switch(option){

  case SET:
    switch(param){
    case GULX:             /* universe size */
      game.ulx=value;
      break;
    case GULY:             /* universe size */
      game.uly=value;
      break;
    case GWIDTH:           /* window size */
      game.width=value;
      break;
    case GHEIGHT:          /* window size */
      game.height=value;
      break;
    case GPANEL:          /* shell size */
      game.panel_height=value;
      break;
    case GNET:             /* TRUE : NET , FALSE : LOCAL */
      game.net=value;
      break;
    case GMODE:            /* LOCAL, SERVER, CLIENT */
      game.mode=value;
      break;
    case GCOOPERATIVE:         
      game.cooperative=value;
      break;
    case GCOMPCOOPERATIVE:         
      game.compcooperative=value;
      break;
    case GQUEEN:            
      game.queen=value;
      break;
    case GPIRATES:
      game.pirates=value;
      break;
    case GNGALAXIES:       /* number of galaxies */
      game.ngalaxies=value;
      break;
    case GNPLAYERS:        /* number of players */
      game.nplayers=value;
      break;
    case GNPLANETS:        /* number of planets */
      game.nplanets=value;
      break;
    case GKPLANETS:        /* TRUE FALSE planets known or unknown */ 
      game.kplanets=value;
      break;
    case GMUSIC:          /* TRUE FALSE game paused */
      game.music=value;
      break;
    case GSOUND:          /* TRUE FALSE game paused */
      game.sound=value;
      break;
    case GPAUSED:          /* TRUE FALSE game paused */
      game.paused=value;
      break;
    case GQUIT:            /* 0,1,2 really quit? */
      game.quit=value;
      break;
    case DEFAULT:
      game.ulx=ULX;
      game.uly=ULY;
      game.width=DEFAULTWIDTH;
      game.height=DEFAULTHEIGHT;
      game.panel_height=PANEL_HEIGHT;
      game.net=FALSE;
      game.mode=LOCAL;
      game.cooperative=FALSE;
      game.compcooperative=FALSE;
      game.queen=FALSE;
      game.pirates=TRUE;
      game.ngalaxies=NUMGALAXIES;
      game.nplayers=NUMPLAYERS;
      game.nplanets=NUMPLANETS;
      game.kplanets=PLANETSKNOWN;
      game.music=TRUE;
      game.sound=TRUE;
      game.paused=FALSE;
      game.quit=0; /*  possible values 0,1,2 */
      break;
    default:
      ret=-1;
      break;
    }
    break;

  case GET:
    switch(param){
    case GULX:             /* universe size */
      ret=game.ulx;
      break;
    case GULY:             /* universe size */
      ret=game.uly;
      break;
    case GWIDTH:           /* window size */
      ret=game.width;
      break;
    case GHEIGHT:          /* window size */
      ret=game.height;
      break;
    case GPANEL:          /* shell size */
      ret=game.panel_height;
      break;
    case GNET:             /* TRUE : NET , FALSE : LOCAL */
      ret=game.net;
      break;
    case GMODE:            /* LOCAL, SERVER, CLIENT */
      ret=game.mode;
      break;
    case GCOOPERATIVE:         
      ret=game.cooperative;
      break;
    case GCOMPCOOPERATIVE:         
      ret=game.compcooperative;
      break;
    case GQUEEN:         
      ret=game.queen;
      break;
    case GPIRATES:
      ret=game.pirates;
      break;
    case GNGALAXIES:       /* number of galaxies */
      ret=game.ngalaxies;
      break;
    case GNPLAYERS:        /* number of players */
      ret=game.nplayers;
      break;
    case GNPLANETS:        /* number of planets */
      ret=game.nplanets;
      break;
    case GKPLANETS:        /* TRUE FALSE planets known or unknown */ 
      ret=game.kplanets;
      break;
    case GMUSIC:          /* TRUE FALSE game paused */
      ret=game.music;
      break;
    case GSOUND:          /* TRUE FALSE game paused */
      ret=game.sound;
      break;
    case GPAUSED:          /* TRUE FALSE game paused */
      ret=game.paused;
      break;
    case GQUIT:            /* 0,1,2 really quit? */
      ret=game.quit;
      break;
    default:
      fprintf(stderr,"ERROR: GameParametres(): param %d unknown\n",param);
      ret=-1;
      break;
    }
    
    break;
  default:
    ret=-1;
    break;
  }
  return(ret);  
}


int GetTime(void){
  return(sTime(2,0));
}

void SetTime(int t){
  sTime(3,t);
}

void IncTime(void){
  sTime(1,0);
}

int sTime(int action,int t){
  static int time=0;
  switch(action){
  case 0: /* RESET */
    time=0;
    break;
  case 1: /* INCREMENT */
    time++;
    break;
  case 2:
    return (time);
    break;
  case 3: /* SET */
    time=t;  
    break;
  default:
    break;
  }
  return(time);
}

void DelCharFromCad(char *cad,char *filter){
  /*
    Remove from cad the characters that are not in filter.

   */

  int n=0;
  int m=0;

  //  printf("cad: \"%s\"\n",cad);
  //  printf("filter: \"%s\"\n",filter);

  //  printf("\"");
  while(cad[n]!='\0'){
    //    printf("%c",*c);
    if(n>=MAXTEXTLEN-1)break;
    if(strchr(filter,cad[n])==NULL){ /* not found */
      //  printf("not found :%c\n",cad[n]);
    }
    else{  /* found */
      // printf("found :%c\n",cad[n]);
      //      cad[m]=cad[n];
      strncpy(&cad[m],&cad[n],1);
      m++;

    }
    n++;
  }

  strncpy(&cad[m],"\0",1);
  //  memcpy(&c[2],'\0',sizeof(char));
  //  printf("m: %d\n",m);
  //printf("cad2: \"%s\"\n",cad);
}

float Sqrt(int n){
  static float rtable[20000];
  static int sw=0;
  float ret;

  if(sw==0){ /* create table */
    int i;
    for(i=0;i<20000;i++){
      rtable[i]=(float)sqrt(i);
    }
    sw=1;
  }

  if(n>=20000){ret=sqrt(n);}
  else{
    ret=rtable[n];
  }
  return(ret); 
}

