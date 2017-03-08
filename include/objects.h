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

#ifndef _OBJECTS_
#define _OBJECTS_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gtk/gtk.h>
#include "general.h"
#include "data.h"
#include "players.h"

typedef struct _Object Object;

extern int g_objid;  /* id of the objects */
extern int g_projid; /* id of the projectiles */
extern int g_nobjsend;
extern int g_nshotsend;
extern int g_nobjmaxsend;
extern int g_nobjtype[];

extern Object *ship_c; /* ship controled by keyboard */
extern Object *cv;     /* coordinates center */

extern int *cell;

extern struct Habitat habitat;

extern struct HeadObjList listheadobjs;    /* list of all objects */
extern struct HeadObjList listheadplanets;        /* list of all planets */
extern struct HeadObjList *listheadcontainer;     /* lists of objects that contain objects: free space and planets*/
extern struct HeadObjList *listheadkplanets;      /* lists of planets known by players */
extern struct HeadObjList listheadplayer;         /* list of objects of each player */




#define VELMAX 60
#define VELMAX2 VELMAX*VELMAX
#define ACELMAX 600
#define VELANG .5
#define GASMAX 1000
#define STATEMAX 100


/* habitat modes */
#define H_SPACE 1
#define H_PLANET 2
#define H_SHIP 3

/* Obj modes */
#define NAV 1
#define LANDED 2
#define SOLD 3


/* object types 0-31 */
#define SHIP 1
#define PROJECTILE 2
#define PLANET 3
#define ASTEROID 4
#define ARTEFACT 5
#define TRACKPOINT 13
#define TRACE 14
#define ALLOBJS 31

/* objects subtypes 0-16*/
/* ship subtypes (SHIP)*/
#define SHIP0 0  /* no ship, any ship */
#define SHIP1 1
#define SHIP2 2
#define SHIP3 3
#define SHIP4 4
#define SHIP5 5
#define SHIP6 6
#define SHIP7 7
#define SHIP8 8
#define SHIP9 9
#define SHIP10 10
#define SHIP_S_MAX SHIP10

/* Asteroid subtypes */
#define ASTEROID1 1
#define ASTEROID2 2
#define ASTEROID3 3



/* predefined ships */ 
#define EXPLORER SHIP1
#define FIGHTER SHIP3
#define QUEEN SHIP4
#define SATELLITE SHIP5
#define TOWER SHIP6
#define PILOT SHIP7
#define FREIGHTER SHIP8
#define GOODS SHIP9
#define CLOAKDEVICE SHIP10

/* cost of the spaceships */

#define COSTEXPLORER 0.01
#define COSTFIGHTER 0.01
#define COSTQUEEN 0.04
#define COSTFREIGHTER 0.04
#define COSTGOODS 0.0
#define COSTSATELLITE 0.005
#define COSTTOWER 0.0
#define COSTPILOT 0.0


#define PRICESHIP0 0
#define PRICESHIP1 100
#define PRICESHIP2 200
#define PRICESHIP3 300
#define PRICESHIP4 400
#define PRICESHIP5 50
#define PRICESHIP6 200
#define PRICESHIP7 50
#define PRICESHIP8 5000
#define PRICESHIP9 0
#define PRICESHIP10 500

/* mass of ships */
#define MASSEXPLORER 50
#define MASSFIGHTER 100
#define MASSQUEEN 400
#define MASSSATELLITE 10
#define MASSTOWER 100
#define MASSPILOT 10
#define MASSFREIGHTER 400
#define MASSGOODS 10


/* projectile subtypes (PROJECTILE) 49-64*/
#define SHOT0 49
#define SHOT1 50
#define SHOT2 51
#define SHOT3 52  /* missile */
#define SHOT4 53  /* laser */
#define SHOT5 54  /* laserbeam*/ /* TODO new tower weapon */
#define EXPLOSION 55
#define MISSILE SHOT3
#define LASER SHOT4
#define LASERBEAM SHOT5


/* objects life */

#define LIFEPILOT 5000
#define LIFEGOODS 2500 
#define LIFESATELLITE 2400
#define LIFEASTEROID 9000
#define LIFEEXPLOSION 150
#define LIFETRACE 500
#define LIFESHOT1 30
#define LIFESHOT2 50
#define LIFESHOT3 200 /* MISSILE */
#define LIFESHOT4 5   /* LASER */
#define LIFESHOT5 40   /* LASERBEAM */

/* ship items */
#define ITSURVIVAL 1  /* has a survival pod */ 
#define CLOAKMAXVALUE 25
#define CLOAKPRICE 500

/* weapons types */
#define CANNON0 0   /* no weapon */
#define CANNON1 1   /* shot */
#define CANNON2 2   /* shot */
#define CANNON3 3   /* shot */
#define CANNON4 4   /* tower,new precision cannon */
#define CANNON5 5   /* shot */
#define CANNON6 6   /* shot */
#define CANNON7 7   /* velocity, not used  */
#define CANNON8 8   /* missile */
#define CANNON9 9   /* laser */
#define CANNON10 10 /* laserbeam */
#define NUMWEAPONS 11
#define CANNONMAX CANNON10

#define PRICECANNON0 0
#define PRICECANNON1 100
#define PRICECANNON2 150
#define PRICECANNON3 200
#define PRICECANNON4 400
#define PRICECANNON5 800
#define PRICECANNON6 1400
#define PRICECANNON7 1800   /* velocity  */
#define PRICECANNON8 2000   /* missile */
#define PRICECANNON9 2200   /* laser */
#define PRICECANNON10 2000   /* laserbeam */

/* engine types */
#define ENGINE0 0 /* no engine */
#define ENGINE1 1 /* only turn */
#define ENGINE2 2
#define ENGINE3 3
#define ENGINE4 4
#define ENGINE5 5
#define ENGINE6 6
#define ENGINEMAX ENGINE6

#define PRICEENGINE0 0
#define PRICEENGINE1 100
#define PRICEENGINE2 200
#define PRICEENGINE3 300
#define PRICEENGINE4 400
#define PRICEENGINE5 500
#define PRICEENGINE6 600

/* Planet states */
#define PUNKNOWN   0
#define POWN       1
#define PALLY      1<<1
#define PINEXPLORE 1<<2
#define PENEMY     1<<3


/* Terrain types */
#define TERRAIN 0
#define LANDZONE 1

/* player status */
#define PLAYERNULL 0       /* not used */
#define PLAYERIDLE 1       /* not used */
#define PLAYERDEAD 2       /* GAMEOVER */
#define PLAYERACTIVE 3     /* all states > PLAYERACTIVE are active states */
#define PLAYERMODIFIED 4   /* when the number of ships or habitat change */  

#define OBJNAMESMAXLEN 16  /*max size of objs names. */



struct Planet{
  Segment *segment;  /* Planet terrain */
  int x,y;           /* coordinates of the planet */
  float r;           /* radio */
  float gold;        /* (0,...] */
  float reggold;     /* index of gold regeneration */
  float A,B;         /* local, tmp variables */
};



struct ObjList{ 
  Object *obj; 
  struct ObjList *next; 
}; 

struct HeadObjList{ 
  int n; 
  int update; 
  struct ObjList *list; 
}; 


typedef struct{
  int type;
  int durable;
  int life;
  int damage;
  int max_vel;
  int mass;
  int gascost;
  float unitcost;
}Projectile;

typedef struct{
  int type;
  int rate;       /* time the weapon is ready again */
  int nshots;     /* number of shots, guns */
  int cont1;      /* if !=0 cant shot again */ 
  int mass;       /* mass of the projectile */
  int n;          /* number of ammunition */
  int max_n;      /* capacity */
  Projectile projectile;
}Weapon;

typedef struct{
  int type;
  int a_max;        /* max accel */
  int a;            /* increment of acceleration */
  int v_max;        /* max speed */
  int v2_max;       /* max speed 2 */
  float ang_a;      /* increment angular acceleration */
  float ang_a_max;  /* max angular acceleration */
  float ang_v_max;  /* max angular velocity */
  float gascost;    /* cost of gas */
  int mass;         /* mass of the engine */
}Engine;

typedef struct{
  Object *obj[4]; /* use to point to 4 objects 
			     enemy ship, 
			     planets: enemy, inexplore and ally  */
  float d2[4];   /* distance2 to these objects */
  int td2[4];    /* time when such info is updated */
  int mlevel;    /* if there is a near high level SHIP */
  int tmlevel;
  int a,b;         /* low fuel stop switches */
}Data;


typedef struct{
  int capacity;                  /* max. mass capacity */
  int mass;                      /* actual mass cargo */
  int n;                         /* number of objects */ 
  struct HeadObjList *hlist;     /* objects list */
}Cargo;

struct Order{
  int priority;
  int id;
  int time;       /* duration of the order */
  int g_time;     /* order time */
  float a,b,c,d;  /* internal variables */
  float e,f,g,h;
  float i,j,k,l;
};


struct OrderList{
  struct Order order;
  struct OrderList *next;
};


struct _Object{
  int id;           /* global identifier */
  int pid;          /* player identifier */
  int oriid,destid; /* origin and destination planet id (only FREIGHTER) */
  char name[MAXTEXTLEN];   /* object name */
  short player;     /* id of the player */
  short type;       /* type: SHIP,PLANET,PROJECTILE,... */

  short subtype;    /* object subtype */
  short level;
  float experience; /* experience */
  float pexperience;/* partial experience */
  int kills;        /*number of enemies killed. */
  int ntravels;     /*number of planets landed. */

  int durable;
  int visible;      /* not used */
  int selected;     /* if ship is selected for an order */
  int radar;        /* radar range */
  int cloak;        /* cloaking device 0 totally visible ... 10 invisible in outer space */
  int mass;         /* mass */

  int items;        /* survival ship, ...*/
  int radio;        /* ship radio */
  float cost;       /* cost of the object per unit time*/
  int damage;       /* damage of the ship in collision*/

  short ai;         /* 0: (manual).(1,10) with AI [0,10] */
  short modified;   /* SENDOBJMOD, SENDOBJMOD0, etc, must be updated by net */
  short ttl;        /* if !=0 don't send  */
  short habitat;    /* free space or planet (H_SPACE H_PLANET)*/
  short mode;       /* LANDED, NAV(EGATING), SOLD */

  float x,y;        /* position */
  float x0,y0;      /* old position */  
  float vx,vy;      /* velocity */
  float fx,fy;      /* new force */
  float fx0,fy0;    /* old force */

  float a;          /* ship angle */
  float ang_v;      /* angular velocity */
  float ang_a;      /* angular acceleration */
  float accel;      /* linear acceleration */
  float gas;        /* gas */
  float gas_max;    /* gas capacity */
  float life;       /* time life */
  float shield;     /* shield [0,1]*/
  float state;      /* estate of the ship %[0,100]*/

  float dest_r2;    /* distance**2 to the nearest object */
  int sw;
  short trace;
  int norder;       /* number of pending orders */
  Cargo cargo;        /* capacity of the bodega TODO. In planets: no of ships */

  struct Order actorder;
  Object *parent;     /* pointer to parent obj */
  Object *dest;       /* pointer to nearest enemy object */
  Object *in;         /* Object in which is contained */ 
  struct Planet *planet;
  struct OrderList *lorder;

  Weapon *weapon;     /* weapon selected 0 1 2*/
  Weapon weapon0;     /* shots */
  Weapon weapon1;     /* missile */
  Weapon weapon2;     /* laser beam */
  Engine engine;      /* motor */
  Data *cdata;        /* data base */

};



struct ObjectAll{   /* SENDOBJALL */
  int id;           /* global identifier */
  int pid;          /* player identifier */
  int oriid,destid; /* origin and destination planet id (only FREIGHTER) */
  char name[MAXTEXTLEN];   /* object name */
  short player;
  short type;       /* type: SHIP,PLANET,PROJECTILE,... */
  short subtype;    /* object subtype */

  short level;
  float experience;
  int kills;        /*number of enemies killed */
  int ntravels;     /*number of planets landed. */ 

  int durable;
  int visible;      /* not used */
  int selected;     /* if ship is selected for an order */
  int radar;        /* radar range */
  int cloak;        /* cloaking device */
  int mass;         /* mass */

  unsigned int items;
  int radio;        /* ship radio */
  float cost;       /* cost of the object per unit time*/
  int damage;       /* damage of the ship in collision*/

  short ai;         /* -1: by keyboard. [0,10] */
  short modified;   /* SENDOBJMOD, SENDOBJMOD0, etc, must be updated by net */
  short ttl;        /* if !=0 don't send  */
  short habitat;    /* free space or planet (H_SPACE H_PLANET)*/
  short mode;       /* LANDED, NAV(EGATING)  */

  float x,y;        /* position */
  float x0,y0;      /* old position */  
  float vx,vy;      /* velocity */
  float fx,fy;      /* new force */
  float fx0,fy0;    /* old force */

  float a;          /* ship angle */
  float ang_v;      /* angular velocity */
  float ang_a;      /* angular acceleration */
  float accel;      /* linear acceleration */
  float gas;        /* gas */
  float gas_max;    /* gas capacity */
  float life;       /* time life */
  float shield;     /* shield [0,1]*/
  float state;      /* estate of the ship %[0,100]*/

  float dest_r2;    /* distance**2 to the nearest object */
  int sw;           /* id of his killer */
  short trace;
 
  int norder;       /* number of pending orders */
  struct Order actorder;  /* actual order */

  int parent;       /* pointer to parent obj */
  int dest;         /* pointer to nearest enemy object */
  int inid;         /* Object in which is contained */ 

  int weapon;        /*weapon selected 0 1 2*/
  Weapon weapon0;    /* actual weapon */
  Weapon weapon1;    /* actual weapon */
  Weapon weapon2;    /* actual weapon */
  Engine engine;     /* motor */
  Cargo cargo;        /* capacity of the dock TODO */
};

struct ObjectNew{   /* SENDOBJNEW */
  int id;           /* identificador */
  short player;
  short type;       /* type: SHIP,PLANET,PROJECTILE,... */

  short subtype;    /* subtipo de objeto */
  int durable;
  int radio;        /* ship radio */
  int damage;       /* damage of the ship */

  short ai;         /* -1: by keyboard. [0,10] */
  short modified;   /* SENDOBJMOD, SENDOBJMOD0, etc, must be updated by net */
  short habitat;    /* free space or planet (H_SPACE H_PLANET)*/
  short mode;       /* LANDED, NAV(EGATING)  */

  float x,y;        /* coordinates */
  float vx,vy;      /* velocity */
  float a;          /* ship angle */
  float gas;        /* gas */
  float life;       /* time life */

  int parent;       /* pointer to parent obj */
  int inid;         /* Object in which is contained */ 
  int planet;

  int engtype;
};


struct ObjectAAll{  /* SENDOBJAALL */
  int id;           /* identificador */
  short level;
  short habitat;    /* free space, planet or ship */
  short mode;       /* LANDED, NAVEGATING  */
  float x,y;        /* actual coordinates */
  float x0,y0;      /* old coordinates  */  
  float vx,vy;      /* actual velocity */  

  float a;          /* ship angle */
  float ang_v;      /* angular velocity */
  float ang_a;      /* angular acceleration */
  float accel;      /* acceleration */

  float gas;        /* fuel */
  float life;       /* life time */
  float state;      /* ship state %[0,100]*/

  int inid;         /* Object id in which is contained */ 
};


struct Objectdynamic{ /* SENDOBJMOD */
  int id;          /* identificador */
  short level;
  short habitat;   /* free space or planet */
  int inid;        /* id of the container */
  short mode;      /* LANDED, NAVEGATING  */
  float x,y;       /* actual coordinates */
  float x0,y0;     /* old coordinates */   
  float vx,vy;     /* actual velocity */ 
  float a;         /* ship angle */
  float ang_v;     /* angular velocity */
  float ang_a;     /* angular acceleration */
  float accel;     /* aceleration */
  float state;     /* ship state %[0,100]*/
};


struct Objectpos{   /* SENDOBJMOD0 */
  int id;           /* identificador */
  float x,y;        /* actual coordinates */
};

struct NearObject{
  Object *obj;
  float d2;
};


struct Habitat{
  int type;      /* H_SPACE, H_PLANET , H_SHIP */
  Object *obj;
};


/*****************************************************************/
/*  Obj Tree. Verlet lists. NOT USED  */
/*****************************************************************/
struct ObjTree{
  Object *obj;
  struct ObjTree *next;
  struct ObjTree *in;
};

struct VerletList{
  Object *obj;
  struct ObjList *nextobj;
  struct VerletList *next;
};
/*****************************************************************/
/*  --Obj Tree. Verlet lists. NOT USED  */
/*****************************************************************/



/* function Declarations */

int CopyObject(Object *obj,Object *nobj);
void Experience(Object *obj,float points);
Object *NewObj(int type,int stype,
	       int x,int y,float vx,float vy,
	       int weapontype,int engtype,int,Object *parent,Object *in);
void ShipProperties(Object *obj,int stype,Object *in);
void NewWeapon(Weapon *weapon,int type);
void NewEngine(Engine *eng,int type);
struct Planet *NewPlanet(void);
/* int GetSegment(Segment *segment,Object *obj); */
int GetSegment(Segment *segment,struct Planet *planet,float x,float y);
int GetLandedZone(Segment *segment,struct Planet *planet);

Object *RemoveDeadObjs(struct HeadObjList *lhobjs,Object *);
void RemoveObj(struct HeadObjList *lhobjs,Object *obj2remove);
int GameOver(struct HeadObjList *lhead,struct Player *players,int actual_player);

int CountObjs(struct HeadObjList *lh,int player,int type,int subtype);
int CountShipsInPlanet(struct HeadObjList *lh,int planetid,int player,int type,int subtype,int max);
int CountShips(struct HeadObjList *lh,int *c,int *s);
int CountPlayerShipObjs(struct HeadObjList *lh,int player,int *cont);
int CountPlayerPlanets(struct HeadObjList *lh,struct Player *player,int *cont);
int CountPlanets(struct HeadObjList *lh,int type);
int CountModObjs(struct HeadObjList *lh,int type);

void PrintObj(Object *obj);

Object *SelectObj(struct HeadObjList *lh,int id);
Object *SelectpObj(struct HeadObjList *lh,int id,int player);
Object *SelectObjInObj(struct HeadObjList *lh,int id,int player);
Object *SelectpObjInObj(struct HeadObjList *lh,int pid,int player);
Object *PrevCv(struct HeadObjList *lh,Object *,int);
Object *NextCv(struct HeadObjList *lh,Object *,int);
Object *FirstShip(struct HeadObjList *lh,Object *cv0,int pid);

Object *NextPlanetCv(struct HeadObjList *lh,Object *cv0,int pid);
Object *PrevPlanetCv(struct HeadObjList *lh,Object *cv0,int pid);

void Explosion(struct HeadObjList *lh,Object *cv,Object *obj,int type);

Object *ObjNearThan(struct HeadObjList *lh,int player,int x,int y,float dmin2);
float Distance2NearestShip(struct HeadObjList *lh,int player,int x,int y);
float Distance2NearestShipLessThan(struct HeadObjList *lh,int player,int x,int y,float dmin);
void NearestObjAll(struct HeadObjList *lhc,Object *obj,struct NearObject *objs);
Object *NearestObj(struct HeadObjList *lh,Object *obj,int type,int stype,int status,float *d2);

void DestroyAllObj(struct HeadObjList *lh);
void DestroyObj(Object *obj);
void DestroyPlanet(struct Planet *planet);


int Add2TextMessageList(struct TextMessageList *lh,char *cad,int source,int dest,int mid,int time,int value);

int Add2ObjList(struct HeadObjList *lhobjs,Object *obj);
int Add2ObjList_B(struct HeadObjList *lhobjs,Object *obj);
int Add2ObjList_E(struct HeadObjList *lhobjs,Object *obj);
int DestroyObjList(struct HeadObjList *lh);
int CountObjList(struct HeadObjList *hlist);
int PrintObjList(struct HeadObjList *hl);
int IsInObjList(struct HeadObjList *lhobjs,Object *obj);
void KillAllObjs(struct HeadObjList *);
void DestroyAllPlayerObjs(struct HeadObjList *lh,int player);

int CreatePlayerList(struct HeadObjList list1,struct HeadObjList *list2,int);
int CreateContainerLists(struct HeadObjList *lh,struct HeadObjList *hcontainer);
int CreatekplanetsLists(struct HeadObjList *lh,struct HeadObjList *hkplanets);
int CreatePlanetList(struct HeadObjList,struct HeadObjList *);
void CreateNearObjsList(struct HeadObjList *lh,struct HeadObjList *lhn,int player);
int CreatePilot(Object *obj);

char Type(Object *obj);
char *TypeCad(Object *obj);

int UpdateSectors(struct HeadObjList lh);
int GetPrice(Object *obj,int sid,int eng,int weapon);
int BuyShip(struct Player *player,Object *obj,int type);


Object *MarkObjs(struct HeadObjList *lh,Space reg,Object *cv,int ctrl);
void UnmarkObjs(struct HeadObjList *lh);
int PrintSelected(struct HeadObjList *lh);
int CountSelected(struct HeadObjList *lh,int player);
int CountNSelected(struct HeadObjList *lh,int player);
int NearMaxLevelObj(Object *obj,struct HeadObjList *lh);
int IsPlanetEmpty(Object *planet,Object *obj);
Object *FirstSelected(struct HeadObjList *lh,int player);
Object *SelectOneShip(struct HeadObjList *lh,Space region,Object *cv,int ctrl);

int IsInRegion(Object *obj,Space region);


int UpdateCell(struct HeadObjList *lh,int *cell);
int ValueCell(int *cell,Object *obj,int x,int y);

int CreatePilot( Object *obj);



float Distance2(Object *obj1,Object *obj2);
float Distance(Object *cv,float x0,float y0);

int CheckObjsId(void);


int CargoAdd(Object *obj1,Object *obj2);
int CargoRem(Object *obj1,Object *obj2);
int CargoDestroy(Object *obj);
void CargoCheck(struct HeadObjList *hol,Object *);
int CargoEjectObjs(Object *obj,int type,int subtype);
Object *CargoGet(Object *obj1,int type,int subtype);
int CargoIsObj(Object *obj,Object *obj2);
int CargoPrint(Object *obj);
int CargoGetMass(Object *obj);
int CargoBuild(Object *obj);


/*************************/
/*****************************************************************/
/*  Obj Tree. Verlet lists. NOT USED  */
/*****************************************************************/
struct ObjTree *Add2ObjTree(struct ObjTree *,Object *);
struct ObjTree *DelObjTree(struct ObjTree *head,Object *obj);
void DestroyTree(struct ObjTree *head);
void PrintObjTree (struct ObjTree *);
int IsInObjTree(struct ObjTree *,int );
struct ObjTree *Look4ObjTree(struct ObjTree *,Object *);

struct VerletList *CreateVerletList(struct HeadObjList hol);
void PrintVerletList(struct VerletList *hvl);
void DestroyVerletList(struct VerletList *hvl);

#endif
