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

#ifndef _AI_
#define _AI_

/* ai */

#include "objects.h"
#include "players.h"

#define CCDATAPRINT 0
#define CCDATANSHIPS 1
#define CCDATANEXPLORER 2
#define CCDATANFIGHTER 3
#define CCDATANPILOT 4
#define CCDATANTOWER 5
#define CCDATANQUEEN 6
#define CCDATASTRENGTH 7

#define MAXPRIORITY 1
#define MAINORD 2
#define FIRSTORD 3

#define MAXnf2a 40
#define MINnf2a 6


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
#define ORBIT 10
#define GOTO 11
#define EXPLORE 12
#define RETREAT 13
#define SELECT 14
#define REPEAT 15
#define BUY 16
#define SELL 17
#define DESTROY 18
#define UPGRADE 19
#define WRITE 20
#define PATROL 21
#define RUNAWAY 22

/* TODO ai players profiles */
#define PLAYERPROFDEFAULT 0   /* attack planets default profile*/
#define PLAYERPROFPACIFIST 1  /* never attack planets */
#define PLAYERPROFAGRESSIVE 2 /* */

/* ai players strategies */
#define NUMPLAYERSTRAT 4
#define PLAYERSTRATDEFAULT 0 /* attack near from near planet*/
#define PLAYERSTRATRANDOM 1  /* choose a random planet to attack */
#define PLAYERSTRATSTRONG 2  /* from strongest to nearest */
#define PLAYERSTRATWEAK 3  /* attack weakest from nearest */

#define PIRATESTRENGTH 85  /* factor for initial experience for pirates. (0..750)  */


struct PlanetInfo{
  struct PlanetInfo *next;
  Object *planet;
  int time;        /* time last update */
  int nexplorer;   /* number of explorers of the planet (team)*/
  int nfighter;    /* number of explorers of the planet (team)*/
  int ntower;      /* number of explorers of the planet (team)*/
  int ncargo;      /* number of explorers of the planet (team)*/
  int npilot;      /* number of pilots */
  float strength;  /* total strenght  */
  float strengtha; /* strength to attack */
  int nassigned;
};

struct CCDATA{
  int player;       /* player id */
  struct PlanetInfo *planetinfo;  /* list of info of planets */
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
  int npilot;       /* number of pilots of the player*/
  Object *pilot;    /* a pointer to the first landed pilot */

  int sw;           /* not used */
  int war;          /* war phase */
  float p2a_strength;    /* strength of planet 2 attack */

  Object *planethighresource; 
  Object *planetweak;
  Object *planethighlevel;

  Object *planet2meet;
  Object *planet2attack;
};



void ai(struct HeadObjList *lhobjs,Object *obj,int actual_player);
void aimissile(struct HeadObjList *lhobjs,Object *obj);

int HigherPoint(struct Planet *planet);
void ControlCenter(struct HeadObjList *lhobjs,struct Player *player);
int CreateCCData(void);
struct CCDATA *GetCCData(int i);
Object *ObjFromPlanet(struct HeadObjList *lhobjs,int id,int player);
Object *ObjMinExperience(struct HeadObjList *lhobjs,int player);
Segment *LandZone(struct Planet *planet);
Object *Coordinates(struct HeadObjList *lhobjs,int id,float *x,float *y);
int ExecGoto(struct HeadObjList *lhobjs,Object *obj,struct Order *ord);
void ExecLand(Object *obj,struct Order *order);
void ExecAttack(struct HeadObjList *lhobjs,Object *obj,struct Order *order,struct Order *morder,float d2);
int ExecStop(Object *obj,float v0,float iv2);
int ExecStop2(Object *obj1,Object *obj2,float v0,float iv2);
int ExecBrake(Object *obj,float v0);
int ExecTurn(Object *obj,float ia);
int ExecTurnAccel(Object *obj,float c,float prec);
int ExecTakeOff(Object *obj);
int ExecOrbit(Object *obj,Object *planet,struct Order *mainord);
int ExecAccel(Object *obj);

Weapon *ChooseWeapon(Object *obj);
int FireCannon(struct HeadObjList *lhobjs,Object *sh,Object *);
void Play(Object *obj,int sid,float vol);

/* int SetModifiedAll20(struct HeadObjList lh,int proc); */

int Risk(struct HeadObjList *lhobjs,Object *obj);
void DelAllOrder(Object *obj);
void DelFirstOrder(Object *obj);
struct Order *ReadOrder(struct Order *,Object *obj,int);
void PrintOrder(struct Order *ord);
int AddOrder(Object *obj,struct Order *order);
int CountOrders(Object *obj);
int ReadMaxPriority(Object *obj);
void TestOrder(Object *obj);

int CountPlanetInfoList(struct CCDATA *ccdata);

void CreatePirates(struct HeadObjList *lhobjs,int,float,float,float);
void CreateAsteroids(struct HeadObjList *lhobjs,int n, float x0,float y0);
void GetInformation(struct Player *p1,struct Player *p2,Object *obj);

int OtherProc(struct HeadObjList *lh,int,Object *obj);


int CCBuy(struct HeadObjList *lhobjs,struct CCDATA *ccdata,struct Player *player,int *pid);
void CalcCCInfo(struct HeadObjList *lhobjs,struct HeadObjList *lhkplanets,int player,  struct CCDATA *ccdata);
int GetCCPlanetInfo(struct CCDATA *ccdata,int pid,int info);
void PrintCCPlanetInfo(struct CCDATA *ccdata);
void DestroyCCPlanetInfo(struct CCDATA *ccdata);
void CalcCCPlanetStrength(int player,struct CCDATA *ccdata);
int NearestCCPlanets(struct CCDATA *ccdata,Object *obj,int status,struct NearObject *objs);
int WarCCPlanets(struct Player *player,struct CCDATA *ccdata);
int AddobjCCData(struct CCDATA *ccdata,Object *obj);
Object *CCUpgrade(struct HeadObjList *lhobjs,struct Player *player);
void SendPlanetInfo2Allies(int player,struct PlanetInfo *pinfo);
 
int DecideWar(struct Player *player,struct CCDATA *ccdata);
struct PlanetInfo *GetPlanetInfo(struct CCDATA *ccdata,Object *planet);
Object *GetNearPlanet(struct CCDATA *ccdata,Object *planet1,int mode);
int ResetPlanetCCInfo(struct CCDATA *ccdata,Object *planet);
struct PlanetInfo *AddNewPlanet2CCData(struct CCDATA *ccdata,Object *planet);
int AddPlanetInfo2CCData(struct CCDATA *ccdata,struct PlanetInfo *pinfo);
int IsInCCList(struct CCDATA *ccdata,Object *planet);
struct PlanetInfo *CalcEnemyPlanetInfo(struct HeadObjList *lhobjs,struct CCDATA *ccdata,Object *obj);
int UpdateEnemyPlanetInfo(struct HeadObjList *lhobjs,struct CCDATA *ccdata,int player);
int CountAssignedCCPlanetInfo(struct HeadObjList *lhobjs,struct CCDATA *ccdata,Object *planet);
struct PlanetInfo *War(struct HeadObjList *lhobjs,struct Player *player,struct CCDATA *ccdata);
int BuyorUpgrade(struct HeadObjList *lhobjs,struct Player *player,struct CCDATA *ccdata);



#endif
