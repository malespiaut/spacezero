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

#ifndef _SAVE_
#define _SAVE_

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <math.h>


#include "general.h"
#include "help.h"
#include "objects.h"
#include "statistics.h"
#include "ai.h"
#include "objects.h"
#include "spacecomm.h" 
#include "functions.h"  



char *CreateOptionsFile(void);
char *CreateSaveFile(int server,int client);
char *CreateRecordFile(void);
char *CreateKeyboardFile(void);

int CreateDir(char *dir);
void SaveRecord(char *file,struct Player *players,int record);

int FprintfObj(FILE *fp,Object *obj);
int FscanfObj(FILE *fp,Object *obj,struct ObjTable *);

int FprintfPlanet(FILE *fp,Object *obj);
int FscanfPlanet(FILE *fp,struct Planet *planet);

int FprintfOrders(FILE *fp,Object *obj);
int FscanfOrders(FILE *fp,Object *obj);

void FprintfCCData(FILE *fp,struct CCDATA *ccdata);
void FscanfCCData(FILE *fp,struct CCDATA *ccdata);

void FprintfPlanetInfo(FILE *fp,struct PlanetInfo *pinfo);
void FprintfPlanetInfoList(FILE *fp,struct CCDATA *ccdata);
void FscanfPlanetInfoList(FILE *fp,struct CCDATA *ccdata);
void FscanfPlanetInfo(FILE *fp,struct PlanetInfo *pinfo);

void SaveParamOptions(char *file,struct Parametres *par);
void SaveUserKeys(char *file,struct Keys *keys);
int LoadUserKeys(char *keyfile,struct Keys *keys);

int LoadParamOptions(char *file,struct Parametres *par);
void PrintParamOptions(struct Parametres *par);

int ExecSave(struct HeadObjList ,char *);
int ExecLoad(char *);


#endif
