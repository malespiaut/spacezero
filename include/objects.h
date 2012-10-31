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

#ifndef _OBJECTS_
#define _OBJECTS_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gtk/gtk.h>
#include "general.h"
#include "data.h"

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
#define SHIP0 0
#define SHIP1 1
#define SHIP2 2
#define SHIP3 3
#define SHIP4 4
#define SHIP5 5
#define SHIP6 6
#define SHIP7 7
#define SHIP_S_MAX SHIP7

/* predefined ships */ 
#define EXPLORER SHIP1
#define FIGHTER SHIP3
#define QUEEN SHIP4
#define SATELLITE SHIP5
#define TOWER SHIP6
#define PILOT SHIP7

/* cost of the spaceships */

#define COSTEXPLORER 0.01
#define COSTFIGHTER 0.01
#define COSTQUEEN 0.04
#define COSTSATELLITE 0.005
#define COSTTOWER 0.0
#define COSTPILOT 0.0


#define PRICESHIP0 0
#define PRICESHIP1 100
#define PRICESHIP2 200
#define PRICESHIP3 300
#define PRICESHIP4 400
#define PRICESHIP5 200
#define PRICESHIP6 200
#define PRICESHIP7 50



/* projectile subtypes (PROJECTILE) 49-64*/
#define SHOT0 49
#define SHOT1 50
#define SHOT2 51
#define SHOT3 52  /* missile */
#define SHOT4 53  /* laser */
#define EXPLOSION 54
#define MISSILE SHOT3
#define LASER SHOT4


/* ship items */
#define ITSURVIVAL 1  /* has a survival pod */ 
#define ITPILOT    2     /* transport pilots */


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
#define NUMWEAPONS 10
#define CANNONMAX CANNON9

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

/* engine types */
#define ENGINE0 0 /* no engine */
#define ENGINE1 1 /* only turn */
#define ENGINE2 2
#define ENGINE3 3
#define ENGINE4 4
#define ENGINE5 5
#define ENGINEMAX ENGINE5

#define PRICEENGINE0 0
#define PRICEENGINE1 100
#define PRICEENGINE2 200
#define PRICEENGINE3 300
#define PRICEENGINE4 400
#define PRICEENGINE5 500

/* Planet states */
#define POWN 1
#define PALLY 2
#define PINEXPLORE 3
#define PENEMY 4
#define PUNKNOWN 5

/* Terrain types */
#define TERRAIN 0
#define LANDZONE 1

/* Asteroid types */
#define ASTEROID1 1
#define ASTEROID2 2
#define ASTEROID3 3

/* player status */
#define PLAYERNULL 0       /* not used */
#define PLAYERIDLE 1       /* not used */
#define PLAYERDEAD 2       /* GAMEOVER */
#define PLAYERACTIVE 3     /* all states > PLAYERACTIVE are active states */
#define PLAYERMODIFIED 4   /* when the number of ships or habitat change */  

#define OBJNAMESMAXLEN 16  /*max size of objs names. */

struct _Segment{
  int x0,y0,x1,y1;
  int type;
  struct _Segment *next;
};

typedef struct _Segment Segment;

struct ObjTable{
  int id,parent,dest,in,planet;
};

struct Planet{
  Segment *segment;  /* Planet terrain */
  int x,y;           /* coordinates of the planet */
  float r;           /* radio */
  float gold;        /* (0,...] */
  float reggold;     /* index of gold regeneration */
  float A,B;         /* local, tmp variables */
};

struct Order{
  int priority;
  int id;
  int time;       /* duration of the order */
  int g_time;     /* order time */
  float a,b,c,d;  /* internal variables */
  float e,f,g,h;
  float i,j,k,l;
};


struct ListOrder{
  struct Order order;
  struct ListOrder *next;
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
  int nshots;     /* number of shots */
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
  struct _Object *obj[4]; /* use to point to 4 objects 
			     enemy ship, 
			     planets: enemy, inexplore and ally  */
  float d2[4];   /* distance2 to these objects */
  int td2[4];    /* time when such info is updated */
  int mlevel;    /* if there is a near high level SHIP */
  int tmlevel;
  int a,b;         /* low fuel stop switches */
}Data;


struct _Object{
  int id;           /* global identifier */
  int pid;          /* player identifier */
  char name[MAXTEXTLEN];   /* object name */
  short player;     /* id of the player */
  short type;       /* type: SHIP,PLANET,PROJECTILE,... */

  short subtype;    /* object subtype */
  short level;
  float experience; /* experience */
  float pexperience;/* partial experience */
  int kills;        /*number of enemies killed. */

  int durable;
  int visible;      /* not used */
  int selected;     /* if ship is selected for an order */
  int radar;        /* radar range */
  int mass;         /* mass */

  int items;        /* survival ship, ...*/
  int cargo;        /* capacity of the bodega TODO. In planets: no of ships */
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
  struct Order actorder;
  struct _Object *parent;     /* pointer to parent obj */
  struct _Object *dest;       /* pointer to nearest enemy object */
  struct _Object *in;         /* Object in which is contained */ 
  struct Planet *planet;
  struct ListOrder *lorder;

  Weapon *weapon;     /* weapon selected 0 1 2*/
  Weapon weapon0;     /* shots */
  Weapon weapon1;     /* missile */
  Weapon weapon2;     /* laser beam */
  Engine engine;      /* motor */
  Data *cdata;        /* data base */
};
typedef struct _Object Object;


struct ObjectAll{   /* SENDOBJALL */
  int id;           /* global identifier */
  int pid;          /* player identifier */
  char name[MAXTEXTLEN];   /* object name */
  short player;
  short type;       /* type: SHIP,PLANET,PROJECTILE,... */
  short subtype;    /* object subtype */

  short level;
  float experience;
  int kills;        /*number of enemies killed */

  int durable;
  int visible;      /* not used */
  int selected;     /* if ship is selected for an order */
  int radar;        /* radar range */
  int mass;         /* mass */

  int cargo;        /* capacity of the dock TODO*/
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
  struct Order actorder;

  int parent;       /* pointer to parent obj */
  int dest;         /* pointer to nearest enemy object */
  int inid;         /* Object in which is contained */ 

  int weapon;        /*weapon selected 0 1 2*/
  Weapon weapon0;    /* actual weapon */
  Weapon weapon1;    /* actual weapon */
  Weapon weapon2;    /* actual weapon */
  Engine engine;     /* motor */
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

struct ObjList{
  Object *obj;
  struct ObjList *next;
};

struct HeadObjList{
  int n;
  int update;
  struct ObjList *next;
};

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

struct Habitat{
  int type;      /* H_SPACE, H_PLANET , H_SHIP */
  Object *obj;
};

struct Global{
  int actual_player;
  int g_objid,g_projid;
  int ship_c,cv;
  int habitat_type;
  int habitat_id;
  int fobj[4];
};

struct Player{
  char playername[MAXTEXTLEN]; /* name of the player */
  short id;          /* player id */
  int status;        /* player status */
  int pid;           /* last ship player id  */
  short proc;        /* machine that controls it */
  short control;     /* HUMAN or COMPUTER */
  short team;        /* each player belongs to a team */
  short profile;     /* */
  short strategy;    /* */
  short maxlevel;    /* max ship level reached */
  short level;       /* sum of ships level */
  int color;         /*    */
  int cv;            /* id of the actual ship */
  int nplanets;      /* number of players planets */
  int nships;        /* number of players ships */
  int nbuildships;   /* number of ships created */
  float gold;        /* actual gold of the player */ 
  float balance;
  int lastaction;    /* buy or upgrade */
  int ndeaths;       /* number of casualties */
  int nkills;        /* number of enemies killed  */
  int points;        /*  */
  short modified;    /* used in communication */
  short ttl;         /* used in communication */

  struct IntList *kplanets;       /* list of known planets */
  struct HeadIntIList ksectors;   /* list of known universe sectors */
};

struct PlayerAll{
  char playername[MAXTEXTLEN]; /* name of the player */
  short id;          /* player id */
  int pid;         /* last ship player id  */
  short proc;        /* machine that controls it */
  short control;     /* HUMAN or COMPUTER */
  short team;        /* each player belongs to a team */
  short profile;     /* */
  short strategy;    /* */
  short maxlevel;    /* max ship level reached */
  int color;       /*    */
  int cv;          /* id of the actual ship */
  int nplanets;    /* number of players planets */
  int nships;      /* number of players ships */
  int nbuildships; /* number of ships created */
  float gold;      /* actual gold of the player */ 
  float balance;
  int lastaction;  /* buy or upgrade */
  int ndeaths;     /* number of casualties */
  int nkills;      /* number of enemies killed  */
  int points;      /*  */
  short modified;    /* used in communication */
  short ttl;
};

struct PlayerMod{    /* Used in communication  */
  short id;
  short nplanets;    /* number of players planets */
  short nships;      /* number of players ships */
  short nbuildships; /* number of ships created */
  float gold;        /* actual gold of the player */ 
  short ndeaths;     /* number of casualties */
  short nkills;      /* number of enemies killed  */
  int points;        /*  */
};


/* function Declarations */

int CopyObject(Object *obj,Object *nobj);
void Experience(Object *obj,float points);
Object *NewObj(struct HeadObjList *lhobjs,int type,int stype,
	       int x,int y,float vx,float vy,
	       int weapontype,int engtype,int,Object *parent,Object *in);
void ShipProperties(Object *obj,int stype,Object *in);
void NewWeapon(Weapon *weapon,int type);
void NewEngine(Engine *eng,int type);
struct Planet *NewPlanet(void);
int GetSegment(Segment *segment,Object *obj);
int GetLandedZone(Segment *segment,struct Planet *planet);

Object *RemoveDeadObjs(struct HeadObjList *lhobjs,Object *,struct Player *p);
void RemoveObj(struct HeadObjList *lhobjs,Object *obj2remove);
int GameOver(struct HeadObjList *lhead,struct Player *players,int actual_player);

int CountObjs(struct HeadObjList *lh,int player,int type,int subtype);
int CountShipsInPlanet(struct HeadObjList *lh,int planetid,int type,int subtype,int max);
int CountShips(struct HeadObjList *lh,int *c,int *s);
int CountPlayerShipObjs(struct HeadObjList *lh,int player,int *cont);
int CountPlayerPlanets(struct HeadObjList *lh,struct Player player,int *cont);
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
Object *NearestObj(struct HeadObjList *lh,Object *obj,int type,int status,float *d2);
int NearestObjs(struct HeadObjList *lh,Object *obj,int type,int status,int n,struct NearObject *objs);

void DestroyAllObj(struct HeadObjList *lh);
void DestroyObj(Object *obj);
void DestroyPlanet(struct Planet *planet);


int Add2TextMessageList(struct TextMessageList *lh,char *cad,int source,int dest,int mid,int time,int value);

int Add2ObjList(struct HeadObjList *lhobjs,Object *obj);
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
int BuyShip(struct Player player,Object *obj,int type);


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
int EjectPilotsObj(struct HeadObjList *lh,Object *obj);
void CheckPilots(struct HeadObjList *hol,Object *);

float Distance2(Object *obj1,Object *obj2);


/*************************/

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
