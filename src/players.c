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


#include <stdlib.h>
#include "players.h"
#include "objects.h"
#include "spacecomm.h"
#include "ai.h"
#include "functions.h"
#include "help.h"


struct Player *players;

struct Player *InitPlayers(void){
  int np;
  static int sw=0;

  np=GameParametres(GET,GNPLAYERS,0)+2;
  if(sw==0){
    players=malloc(np*sizeof(struct Player)); /* +1 system +1 pirates*/ 
    if(players==NULL){ 
      fprintf(stderr,"ERROR in malloc (players)\n"); 
      exit(-1); 
    } 
    MemUsed(MADD,np*sizeof(struct Player));
    sw++;
  }

  else{
    players=realloc(players,np*sizeof(struct Player));
    if(players==NULL){ 
      fprintf(stderr,"ERROR in realloc Execload(players)\n");
      exit(-1);
    } 
  }
  return(players);
}

void CreatePlayers(char *clientname,struct Parametres param){
  int i,j;
  
#if DEBUG
  printf("MAINORD:\n nplayers: %d\n",GameParametres(GET,GNPLAYERS,0));
  printf("nproc: %d  proc: %d\n",GetNProc(),GetProc());
#endif
  
  /* players=malloc((GameParametres(GET,GNPLAYERS,0)+2)*sizeof(struct Player)); /\* +1 system +1 pirates*\/  */
  /* if(players==NULL){  */
  /*   fprintf(stderr,"ERROR in malloc (players)\n");  */
  /*   exit(-1);  */
  /* }  */
  /* MemUsed(MADD,(GameParametres(GET,GNPLAYERS,0)+2)*sizeof(struct Player)); */

  players=InitPlayers();

  CreateCCData();
  
  for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    snprintf(players[i].playername,MAXTEXTLEN,"comp%d",i);
    players[i].id=i;
    players[i].status=PLAYERMODIFIED;
    players[i].pid=GameParametres(GET,GNPLANETS,0)+1;
    players[i].proc=0;
    players[i].control=COMPUTER;
    players[i].team=i+1;
    players[i].profile=PLAYERPROFDEFAULT;
    players[i].strategy=PLAYERSTRATWEAK;
    /* strategy is random weight choosed at WarCCPlanets() */
    players[i].strategy=(int)(NUMPLAYERSTRAT*Random(-1));
    players[i].gmaxlevel=0;
    players[i].maxlevel=0;
    players[i].cv=0;
    players[i].color=i;
    players[i].nplanets=0;
    players[i].nships=0;
    players[i].nbuildships=0;
    players[i].gold=10000*RESOURCEFACTOR;
    players[i].lastaction=0;
    players[i].ndeaths=0;
    players[i].nkills=0;
    players[i].points=0;
    players[i].modified=SENDOBJUNMOD;
    players[i].ttl=2000;
    players[i].goldships=0;
    players[i].goldupdates=0;
    players[i].goldweapon=0;

    players[i].kplanets=NULL;
    players[i].ksectors.n=0;
    players[i].ksectors.n0=0;
    players[i].ksectors.list=NULL;
    for(j=0;j<NINDEXILIST;j++){
      players[i].ksectors.index[j]=NULL;
    }
    
    /* player control and assigment of processors */
    players[i].control=COMPUTER;
    players[i].proc=0;
    if(i==1){
      players[i].control=HUMAN;
    }
    if(GameParametres(GET,GNET,0)==TRUE){
      if(i==1){
	players[i].proc=1;
	snprintf(players[i].playername,MAXTEXTLEN,"%s",PLAYERNAME);
	if(strlen(clientname)>0){
	  snprintf(players[i].playername,MAXTEXTLEN,"%s",clientname);
	}
      }
      if(i==2){
	players[i].control=HUMAN;
	snprintf(players[i].playername,MAXTEXTLEN,"%s",PLAYERNAME);
	if(strlen(param.playername)>0){
	  snprintf(players[i].playername,MAXTEXTLEN,"%s",param.playername);
	}
      }
    }
    else{
      if(i==1){
	players[i].control=HUMAN;
	if(strlen(param.playername)>0){
	  snprintf(players[i].playername,MAXTEXTLEN,"%s",param.playername);
	}
      }
    }
  }
  
  snprintf(players[GameParametres(GET,GNPLAYERS,0)+1].playername,MAXTEXTLEN,"%s","pirates");
  
}


void CreateTeams(struct Parametres param){
  int i,nteam=1;


  players[0].team=1;   /* Universe objects */
  
  /* default mode:  All against all */
  
  for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    players[i].team=i+1; 
  }
  nteam=2; 
  
  /* human players */
  for(i=1;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    if(players[i].control==HUMAN){
      players[i].team=nteam;
      if(param.cooperative==FALSE){
	nteam++;
      }
    }
  }
  if(param.cooperative==TRUE){
    nteam++;
  }
  
  /* computer players */
  for(i=1;i<GameParametres(GET,GNPLAYERS,0)+1;i++){
    if(players[i].control==COMPUTER){
      players[i].team=nteam;
      if(param.compcooperative==FALSE){
	nteam++;
      }
    }
  }
  if(param.compcooperative==TRUE){
    nteam++;
  }
  
  /* pirate player*/
  i=GameParametres(GET,GNPLAYERS,0)+1;
  players[i].team=nteam;
}


struct Player *GetPlayers(void){
  /* printf("players in %p\n",players); */
  return players;
}

struct Player *GetPlayer(int i){

return &players[i];
}




int GetPlayerControl(int player){
  return(players[player].control);
}


void PrintTeams(void){
  int i;
  for(i=1;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    printf("PLAYER %d TEAM %d ",i,players[i].team);
    if(players[i].control==HUMAN)printf("HUMAN");
    if(players[i].control==COMPUTER)printf("COMPUTER");
    printf(" name: \"%s\" ",players[i].playername);
    printf("STRATEGY: %d %d",players[i].strategy,players[i].profile);
    printf("\n");
  }
}

void PrintPlayerResume(void){
  int i;
  for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    printf("Player: %d gold used: ship: %d updates: %d weapon: %f\n",
	   players[i].id,players[i].goldships,players[i].goldupdates,players[i].goldweapon);
  }    
  
}




void AddPlayerGold(int player,int n){
  players[player].gold+=n;
}
int GetPlayerProc(int player){
  return(players[player].proc);
}
