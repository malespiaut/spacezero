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
**************************************************************/

#ifndef _MENU_
#define _MENU_

#include "general.h" 



/* menu item types */
#define MENUITEMTEXT 0      /* only show text */
#define MENUITEMBOOL 1      /* two values: TRUE, FALSE */
#define MENUITEMCHECKBOX 2  /* as ITEMBOOL */
#define MENUITEMTEXTENTRY 3 /* free text entry */
#define MENUITEMCOMBO 4     /* combo list */
#define MENUITEMACTION 5    /* */ 
#define MENUITEMGRABKEY 6   /* */

/* menu item ids */
#define ITEM_0 0
#define ITEM_h 1
#define ITEM_g 2
#define ITEM_n 3
#define ITEM_p 4
#define ITEM_t 5
#define ITEM_l 6
#define ITEM_s 7
#define ITEM_c 8
#define ITEM_ip 9
#define ITEM_port 10
#define ITEM_name 11
#define ITEM_sound 12
#define ITEM_music 13
#define ITEM_k 14
#define ITEM_cooperative 15
#define ITEM_compcooperative 16
#define ITEM_queen 17
#define ITEM_pirates 18
#define ITEM_enemyknown 19
#define ITEM_font 20
#define ITEM_geom 21
#define ITEM_start 22
#define ITEM_quit 23 
#define ITEM_server 24 
#define ITEM_client 25
#define ITEM_default 26 
#define ITEM_fire 27
#define ITEM_turnleft 28
#define ITEM_turnright 29
#define ITEM_accel 30
#define ITEM_automode 31
#define ITEM_manualmode 32
#define ITEM_map 33
#define ITEM_order 34



/* menu edit state   */
#define ITEM_ST_FALSE 0
#define ITEM_ST_SHOW 1
#define ITEM_ST_EDIT 2
#define ITEM_ST_UPDATE 3

#define MENUENTER 1
#define MENUESC 2

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




typedef struct{
  int state;
  unsigned int value;
}key;


struct Keys{
  int load,save;
  int left,right,up,down;  /* move map */
  int tab;                 /* change to next ship */
  int may,ctrl,alt,esc;
  int home;
  int Pagedown,Pageup;         /* next, previous planet */
  int f1,f2,f3,f4,f5,f6,f7,f8,f9,f10;   /* choose ship */
  int centermap;           /* center map */
  int enter;               /* enter command */
  int back;                /* delete last letter */ 
  int trace;               /* trace on/off */ 
  int mleft;               /* mouse left click*/
  int mright;              /* mouse right click*/
  int mdclick;             /* mouse double click*/
  int b;     /* buy ships */
  int s;     /* open charge  */
  int n;     /* change navigation mode, no  */
  int z;     /* zoom map */
  int l;     /* labels on/off */
  int p;     /* pause the game */
  int number[10]; /* shell */
  int g;  /* shell */
  int x;  /* shell */
  int t;  /* shell */
  int o;  /* shell */
  int r;  /* shell */
  int w;  /* shell */
  int e;  /* shell */
  int u;  /* shell */
  int d;  /* shell */
  int q;  /* ^Q quit */
  int y;  /* yes */
  int plus;
  int minus;
  /* user defined keys  */
  key fire;               /* fire */
  key turnleft,turnright;
  key accel;
  key automode,manualmode;     /* switch to manual-auto control */
  key map;
  key order;
};


struct MenuHead{
  char title[MAXTEXTLEN];
  int n;
  int nactive;
  int active;
  struct MenuItem *firstitem;
};

struct MenuItem{
  int id;
  int type;
  int active;
  char text[MAXTEXTLEN];
  char value[MAXTEXTLEN];
  struct MenuItem *next;
  struct MenuHead *nexthead;
};

void PrintMenuHead(struct MenuHead *mh);
void PrintAllMenu(struct MenuHead *mh);
struct MenuHead *MenuHeadNew(char *title);
int Add2MenuHead(struct MenuHead *mhead,struct MenuItem *item0,char *title);
char *GetOptionValue(int id);
char *GetTextEntry(struct MenuItem *item,char *);
struct MenuHead *CreateMenu(void);
int  UpdateMenu(struct MenuHead *mhead,struct MenuHead *mactual);
void MenuUp(struct MenuHead *mhead);
void MenuDown(struct MenuHead *mhead);
int MenuEnter(struct MenuHead *mhead);
void MenuEsc(struct MenuHead *mhead);
struct MenuHead *SelectMenu(struct MenuHead *mhead);
void MenuItemActive(struct MenuHead *mhead,int active);
void Funct01(struct MenuItem *item,char *);

/******* arguments **********/

int Arguments(int argc,char *argv[],char *optfile);
void PrintArguments(char *title);
int CheckArgs(void);
void SetDefaultParamValues(void);
int GetGeom(char *geom,int *w,int *h);
int SearchArg(char *target);

/****keys****/

void SetDefaultUserKeys(void);
void SaveUserKeys(char *file);
int LoadUserKeys(char *keyfile);
void SetDefaultKeyValues(int action);
struct Keys* GetKeys(void);




#endif
