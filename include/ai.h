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

#ifndef _AI_
#define _AI_

/* ai */

#include "objects.h"

#define CCDATAPRINT 0
#define CCDATANSHIPS 1
#define CCDATANEXPLORER 2
#define CCDATANFIGHTER 3
#define CCDATANTOWER 4
#define CCDATANQUEEN 5

#define MAXPRIORITY 1
#define MAINORD 2
#define FIRSTORD 3

/* order ids */

#define NOTHING 0
#define TURN 1
#define TURNACCEL 2
#define ACCEL 3
#define BRAKE 4
#define STOP 5
#define FIRE 6
#define ATTACK 7
#define LAND 8
#define TAKEOFF 9
#define GOTO 10
#define EXPLORE 11
#define RETREAT 12
#define SELECT 13
#define REPEAT 14
#define BUY 15
#define SELL 16
#define UPGRADE 17
#define WRITE 18
#define PATROL 19
#define RUNAWAY 20


struct PlanetInfo{
  struct PlanetInfo *next;
  Object *planet;
  int time;        /* time last update */
  int nexplorer;   /* number of explorers of the planet (team)*/
  int nfighter;    /* number of explorers of the planet (team)*/
  int ntower;      /* number of explorers of the planet (team)*/
  int ncargo;      /* number of explorers of the planet (team)*/
  float strength;  /* total strenght  */
  float strengtha; /* strength to attack */
  int nassigned;
};

struct CCDATA{
  int player;       /* player id */
  struct PlanetInfo *planetinfo;  /* list of info planets */
  int time;         /* last time info */
  int time2;        /* another timer */
  int nkplanets;    /* number of known planets */
  int nplanets;     /* number of own planets */
  int ninexplore;   /* number of inexplore planets */
  int nenemy;       /* number of enemy planets */
 
  int nexplorer;    /* number of explorers of the player*/
  int nfighter;     /* number of fighters of the player*/
  int ntower;       /* number of towers of the player*/
  int ncargo;       /* number of cargos of the player*/

  int sw;           /* not used */
  int war;          /* war phase */

  Object *planetlowdefense; 
  Object *planetweak;

  Object *planet2meet;
  Object *planet2attack;
};



void ai(struct HeadObjList *lhobjs,Object *obj,int actual_player);


int HigherPoint(struct Planet *planet);
void ControlCenter(struct HeadObjList *lhobjs,struct Player player);
Object *ObjFromPlanet(struct HeadObjList *lhobjs,int id,int player);
Object *ObjMinExperience(struct HeadObjList *lhobjs,int player);
Segment *LandZone(struct Planet *planet);
Object *Coordinates(struct HeadObjList *lhobjs,int id,float *x,float *y);
void ExecGoto(Object *obj,struct Order *ord);
void ExecLand(Object *obj,struct Order *order);
void ExecAttack(struct HeadObjList *lhobjs,Object *obj,struct Order *order,struct Order *morder,float d2);
void ExecStop(Object *obj,float v0);
int ExecBrake(Object *obj,float v0);
void ExecTurn(Object *obj,float ia);


Weapon *ChooseWeapon(Object *obj);
int FireCannon(struct HeadObjList *lhobjs,Object *sh,Object *);
void Play(Object *obj,int sid,float vol);

/* int SetModifiedAll20(struct HeadObjList lh,int proc); */

int Risk(struct HeadObjList *lhobjs,Object *obj,int,int *order_id);
void DelAllOrder(Object *obj);
void DelFirstOrder(Object *obj);
struct Order *ReadOrder(struct Order *,Object *obj,int);
void PrintOrder(struct Order *ord);
int AddOrder(Object *obj,struct Order *order);
int CountOrder(Object *obj);
int ReadMaxPriority(Object *obj);
void TestOrder(Object *obj);

void CreatePirates(struct HeadObjList *lhobjs,int,float,float);
void CreateAsteroids(struct HeadObjList *lhobjs,int n, float x0,float y0);
void GetInformation(struct Player *p1,struct Player *p2,Object *obj);

int AreEnemy(struct HeadObjList *lh,int,Object *obj);


int CCBuy(struct CCDATA *ccdata,struct Player player,int *pid);
void CalcCCInfo(struct HeadObjList *lhobjs,struct HeadObjList *lhkplanets,int player,  struct CCDATA *ccdata);
int GetCCPlanetInfo(struct CCDATA *ccdata,int pid,int info);
void PrintCCPlanetInfo(struct CCDATA *ccdata);
void DestroyCCPlanetInfo(struct CCDATA *ccdata);
void CalcCCPlanetStrength(int player,struct CCDATA *ccdata);
int NearestCCPlanets(struct CCDATA *ccdata,Object *obj,int status,struct NearObject *objs);
int WarCCPlanets(struct Player player,struct CCDATA *ccdata);
int AddobjCCData(struct CCDATA *ccdata,Object *obj);
Object *CCUpgrade(struct HeadObjList *lhobjs,struct Player *player);
 
int DecideIfWar(struct Player player,struct CCDATA *ccdata);
struct PlanetInfo *GetPlanetInfo(struct CCDATA *ccdata,Object *planet);
Object *GetNearPlanet(struct CCDATA *ccdata,Object *planet1,int mode);
int ResetPlanetCCInfo(struct CCDATA *ccdata,Object *planet);
int AddNewPlanet2CCData(struct CCDATA *ccdata,Object *planet);
int AddPlanetInfo2CCData(struct CCDATA *ccdata,struct PlanetInfo *pinfo);
int IsInCCList(struct CCDATA *ccdata,Object *planet);
int CalcEnemyPlanetInfo(struct HeadObjList *lhobjs,struct CCDATA *ccdata,Object *obj);
int AddCCEnemyPlanetInfo(struct CCDATA *ccdata,Object *planet); /* NULL function */
int CountAssignedCCPlanetInfo(struct HeadObjList *lhobjs,struct CCDATA *ccdata,Object *planet);
struct PlanetInfo *War(struct HeadObjList *lhobjs,struct Player player,struct CCDATA *ccdata);
int BuyorUpgrade(struct HeadObjList *lhobjs,struct Player player,struct CCDATA *ccdata);



#endif
