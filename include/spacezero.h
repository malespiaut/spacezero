 /*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyleft (C) 2006-2011  M.Revenga
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

#ifndef _SPACE_
#define _SPACE_ 

#include <gtk/gtk.h>
#include <stdio.h>
#include "objects.h"


#define SIGNAL0 SIGRTMIN


#define MALLOC_CHECK_ 2
#define NULO 0



gint MainLoop(gpointer data);
gint MenuLoop(gpointer data);
gint Quit(GtkWidget *widget,gpointer gdata);

void PrintGameOptions(void);
void DrawInfo(GdkPixmap *pixmap,Object *);

int UpdateObjs(void);
void UpdateShip(Object *);
void key_eval(struct Keys *key);
void Collision(struct HeadObjList *);
gint TimerCreateObj(gpointer dat);

float PlanetAtraction(float *fx,float *fy,float x,float y,float m);
void CreateUniverse(int,int,struct HeadObjList *,char **);
void CreateTestShips(struct HeadObjList *lheadobjs);

int PrintfObjInfo(FILE *fp,Object *obj);

Object *ChooseInitPlanet(struct HeadObjList lheadobjs);
void CreateShips(struct HeadObjList *lheadobjs);

int CheckGame(char *);

void GetGold(void);
void GetPoints(struct HeadObjList hl,int proc,struct Player *p);
void Density(void);
void GetUniverse(void);
void NetComm(void);



#endif
