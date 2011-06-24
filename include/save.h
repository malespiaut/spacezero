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

#ifndef _SAVE_
#define _SAVE_

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <math.h>

#include "ai.h"
#include "objects.h"
#include "ai.h"
#include "general.h"
#include "data.h"
#include "spacecomm.h"
#include "functions.h"

int CreateDir(char *dir);
char *CreateOptionsFile(void);
char *CreateSaveFile(int server,int client);
char *CreateRecordFile(void);

void SaveParamOptions(char *file,struct Parametres *par);
void LoadParamOptions(char *file,struct Parametres *par);
void PrintParamOptions(struct Parametres *par);

int ExecSave(struct HeadObjList ,char *);
int FprintfPlanet(FILE *fp,Object *obj);
int FprintfObj(FILE *fp,Object *obj);
int ExecLoad(char *);
int FscanfObj(FILE *fp,Object *obj,struct Tabla *);
int FscanfPlanet(FILE *fp,struct Planet *planet);

int Check(void);
int FprintfOrders(FILE *fp,Object *obj);
int FscanfOrders(FILE *fp,Object *obj);
int CountOrders(Object *obj);


void FprintfCCData(FILE *fp,struct CCDATA *ccdata);
void FscanfCCData(FILE *fp,struct CCDATA *ccdata);

int CountPlanetInfoList(struct CCDATA *ccdata);
void FprintfPlanetInfo(FILE *fp,struct PlanetInfo *pinfo);
void FprintfPlanetInfoList(FILE *fp,struct CCDATA *ccdata);
void FscanfPlanetInfoList(FILE *fp,struct CCDATA *ccdata);
void FscanfPlanetInfo(FILE *fp,struct PlanetInfo *pinfo);




#endif
