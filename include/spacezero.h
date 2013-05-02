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
		version 0.84 April 2013
****/

#ifndef _SPACE_
#define _SPACE_ 

#include <gtk/gtk.h>
#include <stdio.h>
#include "objects.h"
#include "help.h"
#include "ai.h"
#include "functions.h"
#include "general.h" 
#include "save.h"
#include "shell.h" 
#include "planetnames.h"
#include "spacecomm.h" 
#include "sound.h" 
#include "graphics.h" 
#include "sectors.h"


gint MainLoop(gpointer data);
gint MenuLoop(gpointer data);
gint Quit(GtkWidget *widget,gpointer gdata);

void PrintGameOptions(void);

int UpdateObjs(void);
void UpdateShip(Object *);
void key_eval(struct Keys *key);
void Collision(struct HeadObjList *);

float PlanetAtraction(float *fx,float *fy,float x,float y,float m);
void CreateUniverse(int,int,struct HeadObjList *,char **);
void InitGameVars(void);
void CreateTestShips(struct HeadObjList *lheadobjs);

int PrintfObjInfo(FILE *fp,Object *obj);

Object *ChooseInitPlanet(struct HeadObjList lheadobjs);
void CreateShips(struct HeadObjList *lheadobjs);

int CheckGame(char *,int);

void GetGold(void);
void GetPoints(struct HeadObjList *hl,int proc,struct Player *p);
void GetPointsObj(struct HeadObjList *lhobjs,struct Player *p,Object *obj);

void Density(void);
void GetUniverse(void);
void NetComm(void);

void SetGameParametres(struct Parametres param);
void MakeTitle(struct Parametres param, char *title);
void CreateTeams(struct Player *players,struct Parametres param);
void CreatePlayers(struct Player **players,struct CCDATA **ccdatap);
void PrintTeams(struct Player *players);
void SaveRecord(char *file,struct Player *players,int record);

void AddPlanets2List(struct HeadObjList *listheadobjs,struct Player *players);


#endif
