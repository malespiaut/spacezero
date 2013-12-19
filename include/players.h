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


#ifndef _PLAYERS_
#define _PLAYERS_  

#include "data.h"
#include "menu.h"



struct Player{
  char playername[MAXTEXTLEN]; /* name of the player */
  short id;          /* player id */
  int status;        /* player status */
  int pid;           /* last ship player id  */
  short proc;        /* machine that controls it */
  short control;     /* HUMAN or COMPUTER */
  short team;        /* each player belongs to a team */
  short profile;     /* */
  short strategy;    /* */
  short gmaxlevel;    /* max ship level reached */
  short maxlevel;    /* actual max ship level */
  short level;       /* sum of ships level */
  int color;         /*    */
  int cv;            /* id of the actual ship */
  int nplanets;      /* number of players planets */
  int nships;        /* number of players ships */
  int nbuildships;   /* number of ships created */
  float gold;        /* actual gold of the player */ 
  float balance;
  int lastaction;    /* buy or upgrade */
  int ndeaths;       /* number of casualties */
  int nkills;        /* number of enemies killed  */
  int points;        /*  */
  short modified;    /* used in communication */
  short ttl;         /* used in communication */
  int goldships;     /* gold used in buy ships */
  int goldupdates;   /* gold used in upgrade ships */
  float goldweapon;  /*  gold used in gas and ammunition */

  struct IntList *kplanets;       /* list of known planets */
  struct HeadIntIList ksectors;   /* list of known universe sectors */
};


struct PlayerAll{
  char playername[MAXTEXTLEN]; /* name of the player */
  short id;          /* player id */
  int pid;         /* last ship player id  */
  short proc;        /* machine that controls it */
  short control;     /* HUMAN or COMPUTER */
  short team;        /* each player belongs to a team */
  short profile;     /* */
  short strategy;    /* */
  short gmaxlevel;    /* max ship level reached */
  short maxlevel;    /* actual max ship level */
  int color;       /*    */
  int cv;          /* id of the actual ship */
  int nplanets;    /* number of players planets */
  int nships;      /* number of players ships */
  int nbuildships; /* number of ships created */
  float gold;      /* actual gold of the player */ 
  float balance;
  int lastaction;  /* buy or upgrade */
  int ndeaths;     /* number of casualties */
  int nkills;      /* number of enemies killed  */
  int points;      /*  */
  short modified;    /* used in communication */
  short ttl;
  int goldships;     /* gold used in buy ships */
  int goldupdates;   /* gold used in upgrade ships */
  float goldweapon;  /*  gold used in gas and ammunition */
};

struct PlayerMod{    /* Used in communication  */
  short id;
  short gmaxlevel;    /* max ship level reached */ 
  short maxlevel;    /* actual max ship level */
  short nplanets;    /* number of players planets */
  short nships;      /* number of players ships */
  short nbuildships; /* number of ships created */
  float gold;        /* actual gold of the player */ 
  short ndeaths;     /* number of casualties */
  short nkills;      /* number of enemies killed  */
  int points;        /*  */
};




struct Player *InitPlayers(void);
void CreatePlayers(char *clientname,struct Parametres param);

struct Player *GetPlayers(void);
struct Player *GetPlayer(int i);
int GetPlayerControl(int player);

void PrintTeams(void);
void PrintPlayerResume(void);
void AddPlayerGold(int player,int n);
int GetPlayerProc(int player);


#endif
