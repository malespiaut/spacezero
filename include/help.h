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
		version 0.84 april 2013
**************************************************************/


#ifndef _HELP_
#define _HELP_

#include "general.h" 
/* #include "save.h" */

enum Arguments{
  ARG_0,		/* 0*/
  ARG_h,		/* 1*/
  ARG_g,		/* 2*/
  ARG_n,		/* 3*/
  ARG_p,		/* 4*/
  ARG_t,		/* 5*/
  ARG_l,		/* 6*/
  ARG_s,		/* 7*/
  ARG_c,		/* 8*/
  ARG_ip,		/* 9*/
  ARG_port,		/* 10*/
  ARG_name,		/* 11*/
  ARG_sound,		/* 12*/
  ARG_music,		/* 13*/
  ARG_soundvol,		/* 14*/
  ARG_musicvol,		/* 15*/
  ARG_k,		/* 16*/
  ARG_cooperative,	/* 17*/
  ARG_compcooperative,	/* 18*/
  ARG_queen,		/* 19*/
  ARG_pirates,		/* 20*/
  ARG_nopirates,	/* 21*/
  ARG_enemyknown,	/* 22*/
  ARG_noenemyknown,	/* 23*/
  ARG_font,		/* 24*/
  ARG_geom,		/* 25*/
  ARG_nomenu,		/* 26*/
  ARG_fontlist,		/* 27*/
  ARG_lang,		/* 28*/
  ARG_NUM
};

struct Parametres{
  int ngalaxies;
  int nplanets;
  int nplayers;
  int nteams;
  int ul;
  int kplanets;
  int sound; /* on off */
  int music; /* on off */ 
  int soundvol; /* 0..100 */
  int musicvol; /* 0..100 */
  int cooperative;
  int compcooperative;
  int queen;
  int pirates;
  int enemyknown;
  int menu;
  int server;
  int client;
  char IP[MAXTEXTLEN];
  int port;
  int port2;
  char playername[MAXTEXTLEN];
  char font[MAXTEXTLEN];
  char geom[MAXTEXTLEN];
  int fontlist;
  char lang[MAXTEXTLEN];
};

struct Validargum{
    char cad[MAXTEXTLEN];
    int id;
  };

void PrintWarnings(char *version);
void PrintArguments(struct Parametres param,char *title);
int SearchArg(char *target,struct Validargum *v);
void Usage(char *ver,char *l_rev);
int Arguments(int argc,char *argv[],struct Parametres *,char *optfile);
void SetDefaultParamValues(struct Parametres *par);
void SetDefaultUserKeys(struct Keys *keys);
int CheckArgs(struct Parametres p);
int GetGeom(char *geom,int *w,int *h);

#endif
