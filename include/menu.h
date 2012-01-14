 /*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyright (C) 2006-2012  MRevenga
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

#ifndef _MENU_
#define _MENU_

#include "general.h"
#include "functions.h"
#include "help.h"
#include "shell.h"

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
#define ITEM_font 19
#define ITEM_geom 20
#define ITEM_start 21
#define ITEM_quit 22 
#define ITEM_server 23 
#define ITEM_client 24
#define ITEM_default 25 
#define ITEM_fire 26
#define ITEM_turnleft 27
#define ITEM_turnright 28
#define ITEM_accel 29
#define ITEM_automode 30
#define ITEM_manualmode 31
#define ITEM_map 32
#define ITEM_order 33



/* menu edit state   */
#define ITEM_ST_FALSE 0
#define ITEM_ST_SHOW 1
#define ITEM_ST_EDIT 2
#define ITEM_ST_UPDATE 3

#define MENUENTER 1
#define MENUESC 2

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
int  UpdateMenu(struct MenuHead *mhead,struct MenuHead *mactual,struct Keys *keys;);
void MenuUp(struct MenuHead *mhead);
void MenuDown(struct MenuHead *mhead);
int MenuEnter(struct MenuHead *mhead);
void MenuEsc(struct MenuHead *mhead);
struct MenuHead *SelectMenu(struct MenuHead *mhead);
void MenuItemActive(struct MenuHead *mhead,int active);
void Funct01(struct MenuItem *item,char *);

#endif
