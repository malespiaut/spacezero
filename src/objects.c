 /*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyright(C) 2006-2012  MRevenga
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

#include <sys/time.h>
#include "general.h"
#include "functions.h"
#include "spacecomm.h"
#include "objects.h"
#include "ai.h"
#include "data.h"
#include "sound.h"
#include "sectors.h"


#define DL (2*RADAR_RANGE)
//#define DL 3000

struct ObjTree *treeobjs=NULL;
extern struct Player *players;
extern struct HeadObjList listheadobjs;
extern struct HeadObjList *listheadkplanets; /* lists of planets known by players */
extern struct HeadObjList listheadplayer;     /* list of objects of each player */
extern struct TextMessageList listheadtext;
extern struct CharListHead gameloglist;          /* list of all game messages */
extern struct Window windowgamelog;
extern int actual_player,actual_player0;
extern int record;
extern int gameover;
extern int *cell;
int g_objid=1;  /* id of the objects */
int g_projid=-2; /* id of the projectiles */



Object *NewObj(struct HeadObjList *lhead,int type,int stype,
	       int x,int y,float vx,float vy,
	       int weapontype,int engtype,int player,Object *parent,Object *in){
  
  /*
    Create new object
    return:
    a pointer to the new object
    NULL if the are some error or if isnot possible to create it.
   */
  Object *obj;
  int i;  

  /*   return NULL; */

  if(player<0||player>GameParametres(GET,GNPLAYERS,0)+1){
    fprintf(stderr, "ERROR in NewObj(): invalid player id %d\n", player);
    fprintf(stderr, "\ttype :%d stype: %d\n", type,stype);
    exit(-1);

    return(NULL); 
  }


  obj=malloc(sizeof(Object));
  MemUsed(MADD,+sizeof(Object));
  if(obj==NULL){
    fprintf(stderr,"ERROR in malloc NewObj()\n");
    exit(-1);
  }

  if(type<TRACKPOINT){
    players[player].status=PLAYERMODIFIED;
    if(type==SHIP)listheadplayer.update=1;
  }
  if(type<=0){
    fprintf(stderr,"ERROR in NewObj() (type<0) type:%d stype:%d\n",type,stype);
    exit(-1);
  }

  strcpy(obj->name,"x");
  obj->durable=FALSE;
  obj->visible=TRUE;
  obj->selected=FALSE;
  obj->radar=RADAR_RANGE;
  obj->experience=0;
  obj->pexperience=0;
  obj->modified=SENDOBJNEW;
  obj->ttl=0;
  obj->level=0;
  obj->kills=0;
  obj->habitat=H_SPACE;
  obj->mode=NAV;
  obj->damage=1;
  obj->x=x;obj->y=y;      /* actual position */
  obj->x0=x;obj->y0=y;    /* speed in the previous time */
  obj->vx=vx,obj->vy=vy;  /* actual speed */
  obj->fx0=obj->fy0=0;    /* force in previous time */
  obj->fx=obj->fy=0;      /* actual force */
  obj->a=0;               /* ship angle */
  obj->ang_v=0;           /* angular speed */
  obj->ang_a=0;           /* angular aceleration */
  obj->accel=0;           /* aceleracion */
  obj->gas=0;             /* combustible */
  obj->gas_max=0;         /* max. combustible */
  obj->life=1;            /* life time */
  obj->shield=0;          /* shield  (0,1) */
  obj->state=1;           /* ship state (0,1)*/

  switch(type){
  case PLANET:
    obj->id=g_objid;                         /* identifier */
    //    obj->id=GetNProc()*g_objid+GetProc();    /* identifier */
    break;
  case PROJECTILE:
    obj->id=GetNProc()*g_projid-GetProc();   /* identifier */
    break;
  default:
    obj->id=GetNProc()*g_objid+GetProc();    /* identifier */
    break;
  }

  
/*   if(obj->id>30000){ */
/*     fprintf(stderr,"Warning in NewObject(), id too high\n"); */
/*   } */
/*   if(obj->id<-30000){ */
/*     fprintf(stderr,"Warning in NewObject(), id too low\n"); */
/*   } */

  obj->dest_r2=-1;     /* distance2 of the nearest object */
  obj->parent=parent;     /* id of the creator obj */
  obj->dest=NULL;     /* pointer to nearest object */
  obj->in=in;       /* pointer to container object */
  obj->mass=1;       /* mass */
  obj->items=0;
  obj->cargo=0;       /* cargo capacity */
  obj->radio=1;      /* radio */
  obj->type=type;       /* object type  */
  obj->subtype=stype;    /* object subsubtype */
  obj->player=player;

  //  printf("obj:%d player: %d\n",obj->id,obj->player);
  obj->ai=0;         /* 0: by keyboard. [1,10] */
  obj->sw=0;
  obj->lorder=NULL;
  obj->actorder.id=-1;
  obj->norder=0;
  obj->trace=FALSE;
  obj->planet=NULL;
  
  NewWeapon(&obj->weapon0,weapontype);
  NewWeapon(&obj->weapon1,CANNON0);
  NewWeapon(&obj->weapon2,CANNON0);

  if(obj->type==SHIP && obj->subtype==TOWER){
    obj->weapon0.projectile.gascost*=8;
    obj->weapon1.projectile.gascost*=8;
    obj->weapon2.projectile.gascost*=8;
  }

  obj->weapon=&obj->weapon0;

  NewEngine(&obj->engine,engtype);
  obj->cdata=NULL;

  switch(obj->type){
  case PLANET:
    obj->pid=obj->id;
    g_objid++;
    break;
  case TRACE:
    obj->id=-1;
    obj->modified=SENDOBJNOTSEND;
    break;
  case PROJECTILE:
    obj->pid=obj->id;
    g_projid--;
    if(obj->subtype==EXPLOSION){
      obj->modified=SENDOBJNOTSEND;/* don't send explosion */
    }
    break;
  default:
    obj->pid=players[obj->player].pid;
    players[obj->player].pid++;
    g_objid++;
    break;
  }


  switch(obj->type){
  case PROJECTILE:
    obj->radar=0;
    if(obj->parent==NULL){ 
      fprintf(stderr,"ERROR (NewObj) parent NULL id: %d\n",obj->id); 
      fprintf(stderr,"\tplayer: %d\n",obj->player);
      fprintf(stderr,"\ttype: %d\n",obj->type);
      fprintf(stderr,"\tstype: %d\n",obj->subtype);
      return(NULL); 
    } 
    
    obj->planet=NULL;/* obj->parent->planet; */
    
    if(obj->parent!=NULL){
      obj->habitat=obj->parent->habitat;
      obj->in=in;
      obj->durable=obj->parent->weapon->projectile.durable;
      obj->life=obj->parent->weapon->projectile.life;
      obj->damage=obj->parent->weapon->projectile.damage;
      obj->mass=obj->parent->weapon->projectile.mass;
    }
    else{
/*       obj->habitat=obj->parent->habitat; */
/*       obj->in=obj->parent->in; */
/*       obj->durable=obj->parent->weapon->projectile.durable; */
/*       obj->life=obj->parent->weapon->projectile.life; */
/*       obj->damage=obj->parent->weapon->projectile.damage; */
/*       obj->mass=obj->parent->weapon->projectile.mass; */
    }


    switch(stype){
    case SHOT1: /*  standard */
      obj->state=1;
      obj->radio=1;
      break;
    case MISSILE:  /* SHOT3 */
      obj->radar=.5*RADAR_RANGE;
      obj->state=1;
      obj->gas_max=100;
      obj->gas=obj->gas_max;
      obj->radio=6;
      break;
    case LASER: /*  shot4 */
      obj->state=1;
      obj->gas_max=0;
      obj->radio=50;
      break;
    case EXPLOSION:
      obj->durable=TRUE;
      obj->life=150;
      obj->damage=obj->parent->damage/16;
      obj->parent=NULL;
      obj->mass=5;
      break;
    default:
      fprintf(stderr,"ERROR (NewOb)\n");
      exit(-1);
      break;
    }
    
    break;
  case SHIP:
    /*     g_print("creando...%d...",GetTime()); */
    ShipProperties(obj,obj->subtype,in);
    break;

  case ASTEROID:
    obj->durable=TRUE;
    obj->life=9000+4000.0*rand()/RAND_MAX;
    obj->state=10;
    obj->a=0;
    obj->ang_v=(2.0*Random(-1)-1.0)/5.0;
  
    switch(obj->subtype){
    case ASTEROID1:
      obj->mass=100;
      obj->damage=100;
      obj->radio=36;
      break;
    case ASTEROID2:
      obj->mass=30;
      obj->damage=50;
      obj->radio=18;
      obj->life/=2;
      break;
    case ASTEROID3:
      obj->mass=10;
      obj->damage=25;
      obj->radio=10;
      obj->life/=4;
      break;
    default:
      fprintf(stderr,"ERROR in NewObj(): asteroid subtype %d no implemented\n",
	      obj->subtype);
      exit(-1);
      break;
    }
    break;

  case PLANET:
    obj->mass=(int)((MINPLANETMASS+(MAXPLANETMASS-MINPLANETMASS)*
		     Random(-1))/1000)*1000;
    obj->radio=pow(obj->mass,.333);
    obj->shield=1.0;
    obj->state=100;
    obj->damage=10000;
    obj->planet=NewPlanet();
    obj->planet->x=obj->x;
    obj->planet->y=obj->y;/*-obj->radio*2; */
    obj->planet->r=obj->radio;
    obj->planet->gold=(int)((MINPLANETGOLD+((MAXPLANETGOLD-MINPLANETGOLD)*
					    Random(-1))*(float)obj->mass/MAXPLANETMASS)*RESOURCEFACTOR);
    obj->planet->reggold=0.035*((float)obj->mass/MAXPLANETMASS)+0.02+0.02*(Random(-1));
    obj->planet->A=0;
    obj->planet->B=0;
    break;
  case TRACKPOINT:
    obj->durable=FALSE;
    obj->visible=FALSE;
    obj->damage=0;
    obj->mass=0;
    obj->shield=0;
    break;
  case TRACE:
    obj->durable=TRUE;
    obj->life=500;
    obj->gas=0;
    if(obj->parent!=NULL){
      obj->habitat=obj->parent->habitat;
      obj->in=in;
    }
    break;
  default:
    fprintf(stderr,"ERROR (NewOb)\n");
    exit(-1);
    break;
  }

  obj->cost*=COSTFACTOR;

  /* data base */
  if(obj->type==SHIP){ // HERE TODO SHIP COMPUTER, only ships
    obj->cdata=malloc(sizeof(Data));
    MemUsed(MADD,+sizeof(Data));
    if(obj->cdata==NULL){
      fprintf(stderr,"ERROR in malloc NewObj()\n");
      exit(-1);
    }
    for(i=0;i<4;i++){
      obj->cdata->obj[i]=NULL;
      obj->cdata->d2[i]=-1;
      obj->cdata->td2[i]=0;
    }
    obj->cdata->mlevel=0;
    obj->cdata->tmlevel=0;
    obj->cdata->a=0;
    obj->cdata->b=0;
  }
  return(obj);
} /* --Newobj() */

int Add2ObjList(struct HeadObjList *lhead,Object *obj){
  /* 
     add obj at the beginning of the list 
  */

  struct ObjList *ls;

  ls=lhead->next;
  
  lhead->next=malloc(sizeof(struct ObjList));
  MemUsed(MADD,+sizeof(struct ObjList));
  if(lhead->next==NULL){
    fprintf(stderr,"ERROR in malloc Add2ObjList()\n");
    exit(-1);
  }
  
  lhead->next->obj=obj;
  lhead->n++;
  lhead->next->next=ls;
  return(0);

}

void NewWeapon(Weapon *weapon,int type){
  /*
    Create a weapon of type type
    returns:
    a pointer to the new weapon.
*/
  weapon->type=type;
  
  switch (type){
  case CANNON0: /* no weapon */
    weapon->projectile.type=SHOT0;
    weapon->rate=10000;
    weapon->nshots=0;
    weapon->cont1=weapon->rate;
    weapon->mass=0;
    weapon->n=0;
    weapon->max_n=0;
    break;
  case CANNON1:
    weapon->projectile.type=SHOT1;
    weapon->rate=20;
    weapon->nshots=1;
    weapon->cont1=weapon->rate;
    weapon->mass=0;
    weapon->n=0;
    weapon->max_n=50;
    break;
  case CANNON2:
    weapon->projectile.type=SHOT1;
    weapon->rate=15;
    weapon->nshots=1;
    weapon->cont1=weapon->rate;
    weapon->mass=0;
    weapon->n=0;
    weapon->max_n=50;
    break;
 case CANNON3:
    weapon->projectile.type=SHOT1;
    weapon->rate=12;
    weapon->nshots=1;
    weapon->cont1=weapon->rate;
    weapon->mass=0;
    weapon->n=0;
    weapon->max_n=50;
    break;
  case CANNON4:
    weapon->projectile.type=SHOT1;
    weapon->rate=12;
    weapon->nshots=2;
    weapon->cont1=weapon->rate;
    weapon->mass=0;
    weapon->n=0;
    weapon->max_n=50;
    break;
  case CANNON5:
    weapon->projectile.type=SHOT1;
    weapon->rate=8;
    weapon->nshots=3;
    weapon->cont1=weapon->rate;
    weapon->mass=0;
    weapon->n=0;
    weapon->max_n=150;
    break;
  case CANNON6:
    weapon->projectile.type=SHOT1;
    weapon->rate=8;
    weapon->nshots=5;
    weapon->cont1=weapon->rate;
    weapon->mass=0;
    weapon->n=0;
    weapon->max_n=50;
    break;
  case CANNON7:
    weapon->projectile.type=SHOT1;
    weapon->rate=8;
    weapon->nshots=6;
    weapon->cont1=weapon->rate;
    weapon->mass=0;
    weapon->n=0;
    weapon->max_n=50;
    break;
  case CANNON8:
    weapon->projectile.type=MISSILE;/* SHOT3; */
    weapon->rate=80;
    weapon->nshots=1;
    weapon->cont1=weapon->rate;
    weapon->mass=0;
    weapon->n=0;
    weapon->max_n=4;
    break;
  case CANNON9: /*  laser */
    weapon->projectile.type=LASER;/* SHOT4 */
    weapon->rate=10;
    weapon->nshots=1;
    weapon->cont1=weapon->rate;
    weapon->mass=0;
    weapon->n=0;
    weapon->max_n=25;

    break;
  default: 
    printf("error (NewWeapon) type: %d\n",type);
    exit(-1);
    break;
  }

  switch(weapon->projectile.type){
  case SHOT0: /* no weapon */
    weapon->projectile.durable=TRUE;
    weapon->projectile.life=0;
    weapon->projectile.damage=0;
    weapon->projectile.max_vel=0;
    weapon->projectile.mass=0;
    weapon->projectile.gascost=0;
    weapon->projectile.unitcost=0;
    break;
  case SHOT1:
    weapon->projectile.durable=TRUE;
    weapon->projectile.life=30;
    weapon->projectile.damage=15*DAMAGEFACTOR;
    weapon->projectile.max_vel=VELMAX*.75;
    weapon->projectile.mass=1;
    weapon->projectile.gascost=6;
    weapon->projectile.unitcost=.25;
    break;
  case SHOT2:
    weapon->projectile.durable=TRUE;
    weapon->projectile.life=50;
    weapon->projectile.damage=25*DAMAGEFACTOR;
    weapon->projectile.max_vel=VELMAX*.85;
    weapon->projectile.mass=2;
    weapon->projectile.gascost=10;
    weapon->projectile.unitcost=2;
    break;
  case MISSILE:/* SHOT3:  missile  */
    weapon->projectile.durable=TRUE;
    weapon->projectile.life=200;
    weapon->projectile.damage=75*DAMAGEFACTOR;
    weapon->projectile.max_vel=VELMAX*.85;
    weapon->projectile.mass=15;
    weapon->projectile.gascost=10;
    weapon->projectile.unitcost=50;
    break;
  case LASER:/* SHOT4:  laser */
    weapon->projectile.durable=TRUE;
    weapon->projectile.life=5;
    weapon->projectile.damage=50*DAMAGEFACTOR;
    weapon->projectile.max_vel=2*VELMAX;
    weapon->projectile.mass=0;
    weapon->projectile.gascost=30;
    weapon->projectile.unitcost=2;
    break;
  default:
    break;
 
  }
}

void NewEngine(Engine *eng,int type){
  /*
    Create in eng a new engine of type type.
   */

  /*
    typedef struct{
    int type;
    int a_max;         max accel 
    int a;             increment of acceleration 
    int v_max;         max speed 
    int v2_max;        max speed 2 
    float ang_a;       increment angular acceleration 
    float ang_a_max;   max angular acceleration 
    float ang_v_max;   max angular velocity 
    float gascost;     cost of gas 
    int mass;          mass of the engine 
    }Engine;
  */


  /* default for all engines */

  eng->type=type;
  switch (type){
  case ENGINE0: /*  no engine */
    eng->a=0;
    eng->ang_a=0;
    eng->a_max=0;
    eng->v_max=0;
    eng->ang_a=0;
    eng->ang_a_max=0;
    eng->ang_v_max=0;
    eng->gascost=0;
    eng->mass=0;
    break;
  case ENGINE1: 
    eng->a=0;
    eng->a_max=0;
    eng->v_max=20;/* VELMAX; 15; */
    eng->ang_a=0.01;
    eng->ang_a_max=.2;
    eng->ang_v_max=0.3;
    eng->gascost=.05;
    eng->mass=10;
    break;
  case ENGINE2:
    eng->a=25;
    eng->a_max=200;
    eng->v_max=40;/* 40VELMAX; 15; */
    eng->ang_a=0.001; //.0004/*  missile */
    eng->ang_a_max=.2;//.2
    eng->ang_v_max=.3;//.3
    eng->gascost=.07;
    eng->mass=20;
    break;
  case ENGINE3:
    eng->a=35;
    eng->a_max=400;
    eng->v_max=22;/* VELMAX; 20; */
    eng->ang_a=0.02;
    eng->ang_a_max=.2;
    eng->ang_v_max=0.3;
    eng->gascost=.07;
    eng->mass=30;
    break;
  case ENGINE4:
    eng->a=45;
    eng->a_max=600;
    eng->v_max=24;/* VELMAX; 25; */
    eng->ang_a=0.03; 
    eng->ang_a_max=.2;
    eng->ang_v_max=0.3;
    eng->gascost=.11;
    eng->mass=40;
    break;
  case ENGINE5:
    eng->a=55;
    eng->a_max=1200;
    eng->v_max=25;/* VELMAX; 30; */
    eng->ang_a=0.04;
    eng->ang_a_max=.2;
    eng->ang_v_max=0.3;
    eng->gascost=.16;
    eng->mass=60;
    break;
    
  default:
    printf("error (NewEngine) type: %d\n",type);
    exit(-1);
    break;
  }

  eng->v2_max=eng->v_max*eng->v_max;
}


int CountObjs(struct HeadObjList *lh,int player,int type,int subtype){
  /*
    Count the number of objects of type type.
    return this value.
  */

  struct ObjList *ls;
  int n=0;

  if(player!=-1){

    if(type==-1){
      ls=lh->next;
      while(ls!=NULL){
	if(ls->obj->player!=player){ls=ls->next;continue;}
	n++;
	ls=ls->next;
      }
      return(n);
    }
    
    if(subtype==-1){
      ls=lh->next;
      while(ls!=NULL){
	if(ls->obj->player!=player){ls=ls->next;continue;}
	if(ls->obj->type==type)n++;
	ls=ls->next;
      }
      return(n);
    }
    
    ls=lh->next;
    while(ls!=NULL){
      if(ls->obj->player!=player){ls=ls->next;continue;}
      if(ls->obj->type==type && ls->obj->subtype==subtype)n++;
      ls=ls->next;
    }
    return(n);
  }

  /* player==-1 count for all players */
  
  if(type==-1){
    ls=lh->next;
    while(ls!=NULL){
      
      n++;
      ls=ls->next;
    }
    return(n);
  }
  
  if(subtype==-1){
    ls=lh->next;
    while(ls!=NULL){
      if(ls->obj->type==type)n++;
      ls=ls->next;
    }
    return(n);
  }
  
  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->type==type && ls->obj->subtype==subtype)n++;
    ls=ls->next;
  }
  return(n);

}

int CountModObjs(struct HeadObjList *lh,int type){
  /*
    Count the number of modified objects of type type.
    return this value.
    note: not used
  */

  struct ObjList *ls;
  int n=0;
  
  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->type==type && ls->obj->modified!=SENDOBJUNMOD){
      n++;
    }
    ls=ls->next;
  }
  return n;
}


Object *RemoveDeadObjs(struct HeadObjList *lhobjs , Object *cv0,struct Player *p){
  /* 
     version 0.3
     Remove all dead objects from the list lhobjs.
     in net only are remove objects with modified=SENDOBJDEAD
     in single game remove obj with state<=0
     return:
     cv0 if is not removed
     NULL if cv0 is removed
  */

  struct ObjList *ls,*freels;
  Object *ret;
  char text[MAXTEXTLEN];
  int swdead=0;
  int swx=0;
  int gnet;

  ret=cv0;
  gnet=GameParametres(GET,GNET,0);

  ls=lhobjs->next;

  while(ls!=NULL){
    freels=NULL;
    swdead=0;
    swx=0;
    if(gnet==TRUE){
      if(ls->obj->modified==SENDOBJDEAD){swdead=1;}
    }
    else{
      if(ls->obj->state<=0){swdead=1;}
    }

    if(swdead){

      if(ls->obj->type<TRACKPOINT){
	int player=ls->obj->player;
	players[player].status=PLAYERMODIFIED;
	if(ls->obj->type==SHIP && player==actual_player)listheadplayer.update=1;
      }

      freels=ls;

      switch(ls->obj->type){
      case ASTEROID:
	swx=1;

	if(ls->obj->habitat==H_PLANET && ls->obj->sw==0){  /* asteroid crashed */
	  ls->obj->in->planet->gold+=100*pow(2,5-ls->obj->subtype);
	}

	break;
      case SHIP:
	swx=1;
	if(ls->obj->mode==SOLD)swx=0;
	if(ls->obj->habitat==H_PLANET && swx){

	  if(ls->obj->sw!=0){ /* ship killed */
	    ls->obj->in->planet->gold+=0.025*GetPrice(ls->obj,0,0,0);
	  }
	  else{ /* ship crashed */
	    float price,factor;
	    factor=0.01*ls->obj->state*0.25;
	    if(factor<.1)factor=.1;  /* at least an 10 percent*/
	    price=factor*GetPrice(ls->obj,0,0,0);
	    ls->obj->in->planet->gold+=price;
	  }

	}

	break;
      case PROJECTILE:
	if(ls->obj->subtype==MISSILE){
	  swx=1;
	}
	break;
      default:
	break;
      }
    
      if(swx){
	Explosion(lhobjs,cv0,ls->obj,0); /* create new objects, doesnot change ls*/
#if SOUND
	Play(ls->obj,EXPLOSION0,1);
#endif
      }
    }
    
    if(freels!=NULL){
      if(freels->obj->type < TRACKPOINT) {
	p[freels->obj->player].status=PLAYERMODIFIED;
      }
      if(freels->obj->type==SHIP){
	if(freels->obj->type==SHIP)p[freels->obj->player].ndeaths++;
	if( freels->obj->player == actual_player && freels->obj->mode!=SOLD){
	  snprintf(text,MAXTEXTLEN,"(%c %d) SHIP DESTROYED",Type(freels->obj),freels->obj->pid);
	  if(!Add2TextMessageList(&listheadtext,text,freels->obj->id,freels->obj->player,0,100,0)){
	    Add2CharListWindow(&gameloglist,text,0,&windowgamelog);
	  }
	}
      }
      
      if(freels->obj->subtype==QUEEN){
	int gqueen;
	if(freels->obj->player == actual_player){
	  printf("QUEEN DESTROYED\n");
	}
	gqueen=GameParametres(GET,GQUEEN,0);
	
	if(gqueen==TRUE){
	  DestroyAllPlayerObjs(lhobjs,freels->obj->player);
	  if(freels->obj->player==actual_player0)gameover=TRUE;
	}
      }
    }

    ls=ls->next;

    if(freels!=NULL){
      if(freels->obj==cv0)ret=NULL;
      RemoveObj(lhobjs,freels->obj);
    }
  }
  return(ret);
}


void RemoveObj(struct HeadObjList *lhobjs,Object *obj2remove){
  /* 
     Remove the object obj2remove from system.
  */

  struct ObjList *ls,*ls0,*ls1,*freels;
  Object *obj;

  if(obj2remove==NULL){
    fprintf(stderr,"ERROR en removeobj(), obj is NULL\n");
    return;
  }

  freels=NULL;
  ls=lhobjs->next;
  ls0=lhobjs->next;

  while(ls->obj!=obj2remove && ls!=NULL){
    ls0=ls;
    ls=ls->next;
  }
  if(ls->obj==obj2remove){
    if(ls != ls0){
      freels=ls;
      ls0->next=ls0->next->next;
      ls=ls->next;
    }
    else{ /* its the first element */ 
      freels=ls;
      lhobjs->next=lhobjs->next->next;
      ls=ls->next;
      ls0=ls->next;
    }
    
    /* cleaning the memory and references */
    
    obj=freels->obj;
    ls1=lhobjs->next;
    while(ls1!=NULL){
      if(ls1->obj->parent==obj){
	ls1->obj->parent=NULL;
      }
      if(ls1->obj->dest==obj){
	ls1->obj->dest=NULL;
      }
      ls1=ls1->next;
    }
    
    DelAllOrder(freels->obj);
    free(freels->obj);
    freels->obj=NULL;
    MemUsed(MADD,-sizeof(Object));
    free(freels);

    MemUsed(MADD,-sizeof(struct ObjList));
    freels=NULL;
    lhobjs->n--;
  } /*  if(ls->obj==obj2remove) */
  else{
    fprintf(stderr,"ERROR en removeobj()\n");
    exit(-1);
  }
  return;
}

int CountPlayerShipObjs(struct HeadObjList *lh,int player,int *cont){
  /*
    Count the number of ship of the player player
    return this values in the vector cont.
    return the total number of ships.
  */

  struct ObjList *ls;
  int n=0;

  cont[0]=cont[1]=cont[2]=0;
  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->player!=player){ls=ls->next;continue;}
    if(ls->obj->type!=SHIP){ls=ls->next;continue;}
    switch(ls->obj->subtype){
    case TOWER:
      cont[0]++;
      break;
    case EXPLORER:
      cont[1]++;
      break;
    case FIGHTER:
      cont[2]++;
      break;
    default:
      n++;
      break;
    }
    ls=ls->next;
  }
  
  return(n+cont[0]+cont[1]+cont[2]);
}


struct Planet *NewPlanet(void){
  /*
    Create the normailized surface of a planet
    returns a pointer to the planet surface
   */
  int x,y;
  int ix,iy;
  int x0,l;
  struct Planet *planet;
  Segment *s;
  /* int width=GameParametres(GET,GWIDTH,0); */
  int width=LXFACTOR;
  int factor=1;
  int sw=0;
  
  planet=malloc(sizeof(struct Planet));
  MemUsed(MADD,+sizeof(struct Planet));

  if(planet==NULL){
    fprintf(stderr,"ERROR in malloc NewPlanet()\n");
    exit(-1);
  }

  planet->segment=malloc(sizeof(Segment));
  MemUsed(MADD,+sizeof(Segment));
  if(planet->segment==NULL){
    fprintf(stderr,"ERROR in malloc NewPlanet()2\n");
    exit(-1);
  }

  /* first segment */
  planet->segment->next=NULL;
  s=planet->segment;
  
  x=0;y=0;
  y=50*((float)rand()/RAND_MAX);
  s->x0=0;
  s->y0=50*((float)rand()/RAND_MAX);
  s->x1=20*((float)rand()/RAND_MAX);
  s->y1=s->y0+40*((float)rand()/RAND_MAX)-20;
  if(s->y1<0)s->y1=0;
  s->type=TERRAIN;

  x=s->x1;
  y=s->y1;
  
  /* size and position of landing zone */
  l=FACTORLANDZONESIZE*((float)rand()/RAND_MAX)+LANDZONEMINSIZE;
  if(l>width-x)l=width-x;

  x0=(width-LANDZONEMINX-l)*((float)rand()/RAND_MAX)+LANDZONEMINX;

  if(x0<x)x0=x;


  while(x < width){
    
    ix =20*((float)rand()/RAND_MAX);
    iy =40*((float)rand()/RAND_MAX)-20;

    if(x>width-100){
      if(y-planet->segment->y0 > 100)factor=2;

      if(y>planet->segment->y0){
	iy =-factor*20*((float)rand()/RAND_MAX);
      }
      else{
	iy =20*((float)rand()/RAND_MAX);
      }
    }

    if(y+iy<5)continue;
    if(y+iy>MAXPLANETHEIGHT)continue;

    
    s->next=malloc(sizeof(Segment));
    MemUsed(MADD,+sizeof(Segment));
    if(s->next==NULL){
      fprintf(stderr,"ERROR in malloc NewPlanet()\n");
      exit(-1);
    }

    s=s->next;
    s->next=NULL;
    if(x<x0){      
      s->x0=x;
      s->y0=y;
      x+=ix;
      y+=iy;
      if(x>width)x=width;
      
      s->x1=x;
      s->y1=y;
      s->type=TERRAIN;
    }
    else{
      s->x0=x;
      s->y0=y;
      x+=l;
      y+=0;
      if(x>width)x=width-2;
      
      s->x1=x;
      s->y1=y;
      s->type=LANDZONE;
      sw++;
      x0=width;
    }
  }

  /* last segment */
  s->next=malloc(sizeof(Segment));
  MemUsed(MADD,+sizeof(Segment));
  if(s->next==NULL){
    fprintf(stderr,"ERROR in malloc NewPlanet()2\n");
    exit(-1);
  }
  s->next->x0=s->x1;
  s->next->y0=s->y1;
  s->next->x1=width;
  s->next->y1=planet->segment->y0;
  s->next->type=TERRAIN;
  s=s->next;
  s->next=NULL;

  if(sw==0){
    fprintf(stderr,"ERROR in NewPlanet(): planet too small\n");
    fprintf(stderr,"\tincrease window geometry\n");
    exit(-1);
  }
  return planet;
}


int GetLandedZone(Segment *segment,struct Planet *planet){

  Segment *s;

  if(planet==NULL){
    return(0);
  }

  s=planet->segment;
  while(s!=NULL){  
    switch(s->type){
    case LANDZONE:
      segment->x0=s->x0;
      segment->x1=s->x1;
      segment->y0=s->y0;
      segment->y1=s->y1;
      segment->type=s->type;
      return(0);
      break;
    default:
      break;
    }
    s=s->next;
  }    
  return(1);
}


int GetSegment(Segment *segment,Object *obj){
  /*
    returns the segment below the ship
*/
  Segment *s;

  if(obj==NULL)return(0);
  if(obj->habitat!=H_PLANET)return(0);

  s=obj->in->planet->segment;

  while(s!=NULL){  
  
    if(obj->x > s->x0 && obj->x < s->x1){
      segment->x0=s->x0;
      segment->x1=s->x1;
      segment->y0=s->y0;
      segment->y1=s->y1;
      segment->type=s->type;
      return(0);
    }
    s=s->next;
  }    
  return(1);
}



void Explosion(struct HeadObjList *lh,Object *cv,Object *obj,int type){
  /*
    Create explosion objects.
   */  
  int i;
  float v,vx,vy;
  float a;
  Object *nobj;
  int nexplosion=16;
  int swexplosion=0;

  struct timeval time;
  static int n=0;
  static struct timeval time0;


  /* max 500 explosion dots by second */
  gettimeofday(&time,NULL);
  if(time.tv_sec-time0.tv_sec>1 ){
    n=0;
  }
  if(n==0){
    time0=time;
  }
  if(n>500)return;


  if(obj==NULL)return;
  if(cv==NULL)return;

  /* only there are an explosion if you can see it. */

  if(cv->habitat==obj->habitat){
    if(cv->habitat==H_PLANET){
      if(cv->in==obj->in)swexplosion++;
    }
    else{
      /* check if are close */
      if( (cv->x-obj->x)*(cv->x-obj->x)+(cv->y-obj->y)*(cv->y-obj->y)<4000000)
	swexplosion++;
    }
  }
  if(!swexplosion)return;
  

  switch (type){
  case 0: /* ship destroyed */
    nexplosion=(64*obj->mass)/100;
    n+=nexplosion;
    for(i=0;i<nexplosion;i++){   //16
      a=2.*PI*(Random(-1));
      v=1.0*VELMAX*(Random(-1)); 
      vx=v*cos(a) + obj->vx;
      vy=v*sin(a) + obj->vy;
      nobj=NewObj(lh,PROJECTILE,EXPLOSION,obj->x,obj->y,vx,vy,
		  CANNON0,ENGINE0,obj->player,obj,obj->in);
      if(nobj!=NULL){
	nobj->life*=(.25+Random(-1));
	Add2ObjList(lh,nobj);
	nobj->parent=NULL;
      }
    }
    break;
  case 1: /* ship hitted */
    n+=4;
    for(i=0;i<4;i++){
      a=2.*PI*(Random(-1));
      v=0.5*VELMAX*(Random(-1)); 
      vx=v*cos(a) + obj->vx;
      vy=v*sin(a) + obj->vy;
      nobj=NewObj(lh,PROJECTILE,EXPLOSION,obj->x,obj->y,vx,vy,
		  CANNON0,ENGINE0,obj->player,obj,obj->in);
      if(nobj!=NULL){
	Add2ObjList(lh,nobj);
	nobj->parent=NULL;
      }
    }
    break;
  default:
    fprintf(stderr,"Error Explosion(): type %d not implemented\n",type);
    exit(-1);
    break;
  }
}


int CountPlayerPlanets(struct HeadObjList *lh,struct Player player,int *cont){
  /*
    version 0.1
    Count the known planets of the player player.
    return the status of the known planets in the vector cont
    returns the total number of known planets.
   */
  struct ObjList *ls;
  
  cont[0]=cont[1]=cont[2]=0;
  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->type==PLANET){
      if(IsInIntList((player.kplanets),ls->obj->id)){

	if(ls->obj->player==0)cont[0]++;	 /* INEXPLORE */
	else{
	  if(ls->obj->player==player.id)cont[1]++; /* ALLY */
	  else{
	    cont[2]++;                             /* ENEMY */
	  }
	}
      }
    }
    ls=ls->next;
  }
  return (cont[0]+cont[1]+cont[2]);
}


int CountPlanets(struct HeadObjList *lh,int type){
  /*
    Count the planets of the list
   */
  struct ObjList *ls;
  int n=0;
  
  printf("CountPlanets():\n");

  ls=lh->next;
  while(ls!=NULL){
    printf("%p\n",ls);
    n++;
    ls=ls->next;
  }
  return n;
}

int CountShipsInPlanet(struct HeadObjList *lh,int planetid,int player,int type,int stype,int max){
  /*
    Count the number of ships in planet planetid of type and subtype
    of the player player.
    if type or subtype are equal to -1 count all.

    Returns the number of ships.
  */

  struct ObjList *ls;
  int n=0;
  
  ls=lh->next;
  while(ls!=NULL){
    if(type!=-1){
      if(ls->obj->type!=type){ls=ls->next;continue;}
      if(stype!=-1){
	if(ls->obj->subtype!=stype){ls=ls->next;continue;}
      }
    }
    if(ls->obj->player!=player){ls=ls->next;continue;}    

    if(ls->obj->habitat==H_PLANET){
      if(ls->obj->in->id==planetid)n++;
      if(max>0 && n>=max)return(max);
    }
    else{
      if(planetid==0)n++;
      if(max>0 && n>=max)return(max);
    }

    ls=ls->next;
  }
  return(n);
}


int CountShips(struct HeadObjList *lh,int *planet,int *ships){
  /*
    Count the number of ships in planet
    Returns the number of ships.
  */

  struct ObjList *ls;
  int n=0;
  int i;

  for(i=0;i<MAXNUMPLANETS+1;i++){
    planet[i]=0;
  }
  for(i=0;i<SHIP_S_MAX+1;i++){
    ships[i]=0;
  }
  ls=lh->next;
  while(ls!=NULL){

    if(ls->obj->type!=SHIP){ls=ls->next;continue;}

    ships[ls->obj->subtype]++;
    
    if(ls->obj->habitat==H_PLANET){
      if(ls->obj->in!=NULL){
	planet[ls->obj->in->id - 1]++;
      }
      else{
	fprintf(stderr,"ERROR in CountShips(): in NULL id: %d\n",ls->obj->id);
      }
    }
    else{
      planet[MAXNUMPLANETS]++;
    }
    n++;
    ls=ls->next;
  }
  return(n);
}


int CopyObject(Object *nobj,Object *obj){

  memcpy(nobj,obj,sizeof(Object));
  return (sizeof(Object));
} 

Object *SelectObj(struct HeadObjList *lh,int id){
  /* 
     returns:
     a pointer to the Object with the id id,
     NULL if not exist.
  */
  struct ObjList *ls;

  if(id==0)return(NULL);

  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->id==id)
      return(ls->obj);
    ls=ls->next;
  }
  return(NULL);
}


Object *SelectpObj(struct HeadObjList *lh,int pid,int player){
  /* 
     returns:
     a pointer to the Object with the id id,
     NULL if not exist.
  */
  struct ObjList *ls;

  if(pid==0)return(NULL);

  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->type!=PLANET && ls->obj->player != player){ls=ls->next;continue;}
    if(ls->obj->pid==pid){
      return(ls->obj);
    }
    ls=ls->next;
  }
  return(NULL);
}


Object *SelectObjInObj(struct HeadObjList *lh,int id,int player){
  /* 
     returns:
     a pointer to the first Object which is inside of id
     NULL if not exist.
  */
  struct ObjList *ls;

  if(id==0)return(NULL);


  ls=lh->next;
  while(ls!=NULL){
    //    if(ls->obj->state<0)printf("st0: %d\n",ls->obj->id);
    if(ls->obj->in == NULL){ls=ls->next;continue;}
    if(ls->obj->in->id != id){ls=ls->next;continue;}
    if(ls->obj->id == id){ls=ls->next;continue;}
    if(ls->obj->player != player){ls=ls->next;continue;}
    if(ls->obj->type!=SHIP){ls=ls->next;continue;}

    if(ls->obj->state >= 0){
      return(ls->obj);
    }
    ls=ls->next;
  }
  return(NULL);
}

Object *SelectpObjInObj(struct HeadObjList *lh,int pid,int player){
  /* 
     returns:
     a pointer to the first Object which is inside of id
     NULL if not exist.
  */
  struct ObjList *ls;

  if(pid==0)return(NULL);
  if(pid<=GameParametres(GET,GNPLANETS,0))return(SelectObjInObj(lh,pid,player));

  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->in == NULL){ls=ls->next;continue;}
    if(ls->obj->in->pid != pid){ls=ls->next;continue;}
    if(ls->obj->pid == pid){ls=ls->next;continue;}
    if(ls->obj->player != player){ls=ls->next;continue;}
    if(ls->obj->type!=SHIP){ls=ls->next;continue;}

    if(ls->obj->state >= 0){
      return(ls->obj);
    }
    ls=ls->next;
  }
  return(NULL);
}


Object *SelectOneShip(struct HeadObjList *lh,Space reg,Object *cv,int ctrl){ 
  /*
    select the nearest obj to mouse pointer
    return the obj selected
  */
  
  struct ObjList *ls;
  Rectangle rect;
  float x,y;
  int sw1=0; /* first selected */
  Object *ret;
  float d2,d2min=10000;

  if(lh==NULL)return(NULL);
  //  if(cv==NULL)return(NULL);

  ret=NULL;

  rect.x=reg.rect.x;
  rect.y=reg.rect.y;

  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->player!=actual_player){ls=ls->next;continue;}
    if(ls->obj->type!=SHIP){ls=ls->next;continue;}
    if(ls->obj->habitat!=H_SPACE && ls->obj->habitat!=H_PLANET){ls=ls->next;continue;}


    if(ctrl==FALSE)ls->obj->selected=FALSE;

    if(reg.habitat>0){  /* select a region inside a planet */
      if(ls->obj->habitat==H_SPACE){ls=ls->next;continue;}
      if(reg.habitat!=ls->obj->in->id){ls=ls->next;continue;}
      d2=(rect.x-ls->obj->x)*(rect.x-ls->obj->x)+(rect.y-ls->obj->y)*(rect.y-ls->obj->y);
    }
    else{ /* select a region in map view */
      if(ls->obj->habitat==H_PLANET){
	x=ls->obj->in->x;
	y=ls->obj->in->y;
      }
      else{
	x=ls->obj->x;
	y=ls->obj->y;
      }
      d2=(rect.x-x)*(rect.x-x)+(rect.y-y)*(rect.y-y);
    }

    if(d2<d2min||sw1==0){
      ret=ls->obj;
      d2min=d2;
      sw1++;
      }
    ls=ls->next;
  }
  return(ret);
}


int IsInRegion(Object *obj,Space region){
  Point a,b;
  Rectangle rect;
  rect.x=region.rect.x;
  rect.y=region.rect.y;
  rect.width=region.rect.width;
  rect.height=region.rect.height;
  
  if(region.rect.width<0){
    rect.x+=region.rect.width;
    rect.width*=-1;
  }
  
  if(region.rect.height<0){
    rect.y+=region.rect.height;
    rect.height*=-1;
  }
  if(region.habitat>0){ /* select a region inside a planet */
    a.x=rect.x;
    a.y=rect.y-rect.height;
    b.x=rect.x+rect.width;
    b.y=rect.y;
  }
  else{ /* select a region in map view */
    a.x=rect.x;
    a.y=rect.y;
    b.x=rect.x+rect.width;
    b.y=rect.y+rect.height;
  }
  if(obj->x < a.x || obj->x > b.x || obj->y < a.y || obj->y > b.y ){
    /*reset */

    return(TRUE);
  }
  return(FALSE);
  
}


float Distance2NearestShip(struct HeadObjList *lh,int player,int x,int y){
  /*

    Return the distance2 of the nearest SHIP to the point x,y.  x,y
    are space coordinates, no planet. The nearest obj can be in free
    space or inside a planet. In this last case the coordinates are
    the planet ones.
    if player = -1 match all players
  */

  float d2;
  struct ObjList *ls;
  float rx,ry,r2;
  float x1,y1;
  Object *obj=NULL;
  int sw=0;

/*   if(!PLANETSKNOWN) */
/*     if(IsInIntList((players[player].kplanets),ls->obj->id)==0)break; */

  d2=-1;
  
  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->type!=SHIP){ls=ls->next;continue;}
    if(ls->obj->player!=player && player !=-1){ls=ls->next;continue;}

    obj=ls->obj;

    switch(obj->habitat){
    case H_PLANET:
      x1=obj->in->x;
      y1=obj->in->y;
      break;
    case H_SPACE:
      x1=obj->x;
      y1=obj->y;
      break;
    default:
      ls=ls->next;continue;
      break;
    }

    rx=x - x1;
    ry=y - y1;
    r2=rx*rx+ry*ry;

    if(sw==0|| r2<d2){
      d2=r2;
      sw++;
    }
    ls=ls->next;
  }
  return(d2);
}


float Distance2NearestShipLessThan(struct HeadObjList *lh,int player,int x,int y,float dmin2){
  /*

    returns 1 if the distance2 to nearest SHIP to the point x,y is less than dmin2.  
    returns 0 if is greater.
    x,y are space coordinates, no planet. The nearest obj can be in free
    space or inside a planet. In this last case the coordinates are
    the planet ones.
    if player = -1 match all players

  */

  float d2;
  struct ObjList *ls;
  float rx,ry,r2;
  float x1,y1;
  Object *obj=NULL;
  int sw=0;

/*   if(!PLANETSKNOWN) */
/*     if(IsInIntList((players[player].kplanets),ls->obj->id)==0)break; */

  d2=-1;

  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->type!=SHIP){ls=ls->next;continue;}
    if(ls->obj->player!=player && player !=-1){ls=ls->next;continue;}

    obj=ls->obj;

    switch(obj->habitat){
    case H_PLANET:
      x1=obj->in->x;
      y1=obj->in->y;
      break;
    case H_SPACE:
      x1=obj->x;
      y1=obj->y;
      break;
    default:
      ls=ls->next;continue;
      break;
    }

    rx=x - x1;
    ry=y - y1;
    r2=rx*rx+ry*ry;

    if(sw==0|| r2<d2){
      d2=r2;
      if(d2<dmin2)return(1);
      sw++;
    }
    ls=ls->next;
  }
  return(0);
}

Object *ObjNearThan(struct HeadObjList *lh,int player,int x,int y,float d2){
  /*
    returns:
    the nearest obj planet id of first  planet closer than dmin2.
    0 if there are no planets at that distance.
  */

  struct ObjList *ls;
  float rx,ry;
  float dmin2;
  Object *obj=NULL;
  Object *retobj=NULL;

  dmin2=d2;

  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->player!=player && ls->obj->type!=PLANET){ls=ls->next;continue;}
    if(ls->obj->habitat!=H_SPACE){ls=ls->next;continue;}
    if(ls->obj->type!=PLANET && ls->obj->type!=SHIP){ls=ls->next;continue;}

    if(ls->obj->type==PLANET){
      if(!PLANETSKNOWN){
	if(IsInIntList((players[player].kplanets),ls->obj->id)==0){
	  ls=ls->next;continue;
	} 
      }
    }
    obj=ls->obj;
 
    rx=x - obj->x;
    ry=y - obj->y;

    if(rx*rx+ry*ry<dmin2){
      dmin2=rx*rx+ry*ry;
      retobj=obj;
    }
    ls=ls->next;
  }
  return(retobj);
}



Object *NearestObj(struct HeadObjList *lh,Object *obj,int type,int pstate,float *d2){
  /*
    Return a pointer to the nearest object of obj in state pstate.
    pstate can be own, ally, enemy or inexplore. 
    in d2 returns the distance^2.  
  */

  struct ObjList *ls;
  float rx,ry,r2;
  float x0,y0,x1,y1;
  float r2min;
  int player;
  Object *robj=NULL;
  Object *obj2=NULL;
  int pheight2=0;
  int swp=0;
  int sw=0;


/*   if(!PLANETSKNOWN) */
/*     if(IsInIntList((players[player].kplanets),ls->obj->id)==0)break; */

  *d2=-1;
  if(obj==NULL)return(NULL);
  
  player=obj->player;
  r2min=-1;
  obj->dest_r2=-1;

  if(obj->habitat==H_SPACE){
    pheight2=GameParametres(GET,GHEIGHT,0);
    pheight2*=pheight2;
  }

  ls=lh->next;
  while(ls!=NULL){
    obj2=ls->obj;
    if(obj2->type!=type){ls=ls->next;continue;}

    switch(obj2->type){
    case SHIP:
      if(obj2->ttl<MINTTL){ls=ls->next;continue;}
      if(obj2->subtype==PILOT){
	ls=ls->next;continue; /* ignoring pilots */
	if(obj2->mode==LANDED){ls=ls->next;continue;}
	if(obj2->habitat==H_SHIP){ls=ls->next;continue;}
      }
      break;
    case PLANET:
      if(IsInIntList((players[obj->player].kplanets),obj2->id)==0){
	ls=ls->next;continue;
      }
      break;
    default:
      break;
    }

    sw=0;
    if(pstate & PENEMY){
      if((players[obj2->player].team!=players[player].team) &&  obj2->player!=0) {sw=1;}
    }
    if(pstate & PINEXPLORE){
      if(obj2->player==0){sw=1;}
    }
    if(pstate & PALLY){
      if(players[obj2->player].team==players[player].team)
	{sw=1;}
    }
    if(!sw){ls=ls->next;continue;}
    

    if(obj2==obj){ls=ls->next;continue;}

    if(obj2->habitat==H_PLANET){
      if(obj->habitat!=H_PLANET){ls=ls->next;continue;}
      
      if(obj->habitat==H_PLANET){
	if(obj->in!=obj2->in){ls=ls->next;continue;}
      }
    }      

    x0=obj->x;
    y0=obj->y;

    swp=0;
    if(obj->habitat==H_PLANET && obj2->habitat!=H_PLANET){
      x0=obj->in->x;
      y0=obj->in->y;
      swp=1;
    }
    x1=obj2->x;
    y1=obj2->y;

    rx=x0 - x1;
    ry=y0 - y1;
    r2=rx*rx+ry*ry;
    if(swp)r2+=pheight2;

    if(robj==NULL || r2<r2min){
      r2min=r2;
      *d2=r2;
      obj->dest_r2=r2;
      robj=obj2;
    }
    ls=ls->next;
  }
  obj->dest=robj;
  return(robj);
}

void NearestObjAll(struct HeadObjList *lhc,Object *obj,struct NearObject *objs){
  /*
    version 03
    look for closer object to obj in the list lhc.

    In vector objs save the next information, (in order):
    the nearest enemy ship or ASTEROID
    the nearest known enemy planet
    the nearest known inexplore planet
    the nearest known ally planet
  */

  struct ObjList *ls;
  Object *obj2;
  int i,j;
  float rx,ry,r2;
  float x0,y0;
  int player;
  float radar2;
  int nlist;
  int pheight2=0;
  int swp=0;

  for(i=0;i<4;i++){
    objs[i].obj=NULL;
    objs[i].d2=-1;
   }
 
  if(obj==NULL)return;
  if(obj->habitat!=H_SPACE && obj->habitat!=H_PLANET)return;

  player=obj->player;

  radar2=obj->radar*obj->radar;

  if(obj->habitat==H_PLANET){
    pheight2=GameParametres(GET,GHEIGHT,0);
    pheight2*=pheight2;
  }

  /* among free space and planets */
  nlist=3;
  if(obj->habitat==H_SPACE){
    nlist=2;
  }
  for(j=0;j<nlist;j++){
    switch(j){
    case 0:
      ls=listheadkplanets[obj->player].next;
      break;
    case 1:
      ls=lhc[0].next;
      break;
    default:
      ls=lhc[(obj->in->id)].next;
      break;
    }

    while(ls!=NULL){
      obj2=ls->obj;
      //      if(obj2->player==0){ls=ls->next;continue;}
      
      switch(obj2->type){
      case SHIP:
      case ASTEROID:
	if(obj2->player==obj->player){ 
	  ls=ls->next;continue; 
	} 
	if(obj2->ttl<MINTTL){ls=ls->next;continue;}

	if(obj2->state<=0){
	  ls=ls->next;continue; 
	}

	if(obj->habitat==H_PLANET && obj2->habitat==H_PLANET){
	  if(obj->in != obj2->in){
	    fprintf(stderr,"ERROR 2 in NearestObjAll()\n");
	    ls=ls->next;continue;
	  }
	}
	/* ignore pilots */
	if(obj2->type==SHIP && obj2->subtype==PILOT){ls=ls->next;continue;}
	
	break;
      case PLANET:
	if(j!=0){ls=ls->next;continue;}
	break;
      default:
	ls=ls->next;continue;
      }
      swp=0;
      if(obj->habitat==H_PLANET && obj2->habitat!=H_PLANET){
	x0=obj->in->x;
	y0=obj->in->y;
	swp=1;
      }
      else{
	x0=obj->x;
	y0=obj->y;
      }    
      
      rx=x0 - obj2->x;
      ry=y0 - obj2->y;
      r2=rx*rx+ry*ry;
      if(swp)r2+=pheight2;

      switch(obj2->type){
      case SHIP:
	if(r2>radar2){ls=ls->next;continue;}
      case ASTEROID:
	if(r2>25*radar2){ls=ls->next;continue;}

	if( players[obj2->player].team!=players[player].team ){ /* Enemy Ship */
	  if(objs[0].obj==NULL){ /* first element */
	    objs[0].obj=obj2;
	    objs[0].d2=r2;
	  }
	  else{
	    if(r2<objs[0].d2){
	      objs[0].obj=obj2;
	      objs[0].d2=r2;
	    }
	  }
	}
	break;

      case PLANET:
	if(obj2->player!=0 && (players[obj2->player].team != players[player].team )){ /* Enemy Planet */
	  if(objs[1].obj==NULL){ 
	    objs[1].obj=obj2;
	    objs[1].d2=r2;
	  }
	  else{
	    if(r2<objs[1].d2){
	      objs[1].obj=obj2;
	      objs[1].d2=r2;
	    }
	  }
	  break;
	}
	
	if((obj2->player==0) ){ /* Inexplore Planet */
	  if(objs[2].obj==NULL){ 
	    objs[2].obj=obj2;
	    objs[2].d2=r2;
	  }
	  else{
	    if(r2<objs[2].d2){
	      objs[2].obj=obj2;
	      objs[2].d2=r2;
	    }
	  }
	  break;
	}
	
	if((players[obj2->player].team==players[player].team) ){ /* Ally Planet */
	  if(objs[3].obj==NULL){ 
	    objs[3].obj=obj2;
	    objs[3].d2=r2;
	  }
	  else{
	    if(r2<objs[3].d2){
	      objs[3].obj=obj2;
	      objs[3].d2=r2;
	    }
	  }
	  break;
	}
	break;
      default:
	fprintf(stderr,"ERROR in NearestObjAll\n");
	exit(-1);
	break;
      }
      ls=ls->next;
    }
  }
  return;
}


void DestroyAllObj(struct HeadObjList *lh){
  struct ObjList *ls,*ls0;
  
  ls=lh->next;

  while(ls!=NULL){
    /*     printf("%d ",ls->obj->id); */
    DestroyObj(ls->obj);
    ls->obj=NULL;
    ls0=ls;
    ls=ls->next;
    free(ls0);
    ls0=NULL;
    MemUsed(MADD,-sizeof(struct ObjList));
    lh->n--;
  }
}


void DestroyAllPlayerObjs(struct HeadObjList *lh,int player){
  struct ObjList *ls;
  
  ls=lh->next;

  while(ls!=NULL){
    /*     printf("%d ",ls->obj->id); */
    if(ls->obj->player!=player){ls=ls->next;continue;}
    if(ls->obj->type==SHIP){
      ls->obj->state=0;
    }
    ls=ls->next;
  }
}


void DestroyObj(Object *obj){

  if(obj==NULL)return;
  DelAllOrder(obj);
  if(obj->type==PLANET){
    DestroyPlanet(obj->planet);
  }
  if(obj->cdata!=NULL){ // HERE COMPUTER, only ships
    if(obj->type!=SHIP){ /* checking  */
      fprintf(stderr,"ERROR DestroyObj(): obj is not a SHIP\n");
      exit(-1);
    }
    free(obj->cdata);
    obj->cdata=NULL;
    MemUsed(MADD,-sizeof(Data));

  }
  free(obj);
  obj=NULL;
  MemUsed(MADD,-sizeof(Object));
}

void DestroyPlanet(struct Planet *planet){
  Segment *s,*s0;

  s=planet->segment;
  while(s!=NULL){
    s0=s;
    s=s->next;
    free(s0);
    s0=NULL;
    MemUsed(MADD,-sizeof(Segment));

  } 
  free(planet);
  planet=NULL;
  MemUsed(MADD,-sizeof(struct Planet));

}

int CopyPlanet2Buf(struct Planet *planet,char *buf){
  struct Planet *planet2;
  Segment *s,*s2;
  int offset=0;

  planet2=(struct Planet *)buf;
  
  planet2->x=planet->x;
  planet2->y=planet->y;
  planet2->r=planet->r;
  planet2->gold=planet->gold;
  planet2->segment=planet->segment;
  offset=sizeof(struct Planet);
  
  s=planet->segment;
  
  s2=(Segment *)(buf+offset);

  
  while(s!=NULL){

    s2->x0=s->x0;
    s2->y0=s->y0;
    s2->x1=s->x1;
    s2->y1=s->y1;
    s2->type=s->type;
    s2->next=s->next;

    offset+=sizeof(Segment);
    s2++;
    s=s->next;
  }
  return(offset);
}


int CopyBuf2Planet(char *buf,struct Planet *planet){
  struct Planet *planet2;
  Segment *s,*s2;
  int offset=0;

  planet=malloc(sizeof(struct Planet));
  MemUsed(MADD,+sizeof(struct Planet));
  if(planet==NULL){
    fprintf(stderr,"ERROR in malloc CopyBuf2Planet()\n");
    exit(-1);
  }

  planet2=(struct Planet *)buf;
  
  planet->x=planet2->x;
  planet->y=planet2->y;
  planet->r=planet2->r;
  planet->gold=planet2->gold;
  planet->segment=NULL;

  offset=sizeof(struct Planet);

  s=malloc(sizeof(Segment));
  MemUsed(MADD,+sizeof(Segment));
  if(s==NULL){
    fprintf(stderr,"ERROR in malloc CopyBuf2Planet()2\n");
    exit(-1);
  }
  s->next=NULL;
  planet->segment=s;

  s2=(Segment *)(buf+offset);

  s->x0=s2->x0;
  s->y0=s2->y0;
  s->x1=s2->x1;
  s->y1=s2->y1;
  s->type=s2->type;
  s->next=NULL;


  while(s2->next!=NULL){
    s->next=malloc(sizeof(Segment));
    MemUsed(MADD,+sizeof(Segment));
    if(s->next==NULL){
      fprintf(stderr,"ERROR in malloc CopyBuf2Planet()2\n");
      exit(-1);
    }
    s->next->next=NULL;
    s=s->next;
    s2++;
    offset+=sizeof(Segment);

    s->x0=s2->x0;
    s->y0=s2->y0;
    s->x1=s2->x1;
    s->y1=s2->y1;
    s->type=s2->type;
  }
  return(0);
}


int UpdateSectors(struct HeadObjList lh){
  /*
    add the actual sector to his own player list

  */

  struct ObjList *ls;
  int n=0;
  int i,j,k;  
  int i2,k2,is;
  int time;
  int proc=0;
  float maxx,maxy;

  
  proc=GetProc();
  time=GetTime();

  maxx=0.55*GameParametres(GET,GULX,0);
  maxy=0.55*GameParametres(GET,GULY,0);

  ls=lh.next;

  while(ls!=NULL){
    if((ls->obj->id + time)%20){ls=ls->next;continue;}
    if(proc==players[ls->obj->player].proc){
      if(ls->obj->type==SHIP && ls->obj->habitat==H_SPACE){
	
	if(ls->obj->x>maxx || ls->obj->x<-maxx || ls->obj->y>maxy || ls->obj->y<-maxy){
	  ls=ls->next;continue;
	}
	
	switch(ls->obj->subtype){
	case EXPLORER:
	  k=ls->obj->radar/SECTORSIZE;
	  k2=k*k;
	  for(i=-k;i<k+1;i++){
	    i2=i*i;
	    is=i*SECTORSIZE;
	    for(j=-k;j<k+1;j++){
	      if(i2+j*j<=k2){
		Add2IntIList(&(players[ls->obj->player].ksectors),
			    Quadrant(ls->obj->x+is,ls->obj->y+j*SECTORSIZE));
	      }
	    }
	  }
	  break;
	default:
	  Add2IntIList(&(players[ls->obj->player].ksectors),
		       Quadrant(ls->obj->x,ls->obj->y));
	  break;
	}

	n++;
      }
    }
    ls=ls->next;
  }
  return n;
}

int Add2TextMessageList(struct TextMessageList *listhead,char *cad,
			int source,int dest,int mid,int time,int priority){
  /* 
     version 02. April 2 2011
     add the integer id to the list by priority given by priority
     if is not already added.
     where:
     cad: the message.
     source: the object id
     dest: the destination player. -1 : for all players
     mid: is a message indentifier.
     time: the duration of the message in centiseconds.
     priority: priority.
     returns:
     0 if the message is added to the list
     1 if not, because is already added.
   */
  struct TextMessageList *list;
  struct TextMessageList *lh;
  int n=0;
  /* Add text at the end of the list */

  if(dest!=actual_player && dest!=-1){
    return(1);
  }

  lh=listhead;
  while(lh->next!=NULL){

    if(lh->next->info.source==source && lh->next->info.id==mid && lh->next->info.value==priority){ /* already added */
      //      if(priority)printf("message (%s)rejected\n",cad);
      return(1);
      /* if(strncmp(cad,lh->info.text,24)==0){ */
      /* } */
    }

    if(lh->next->info.value<priority){ /* added before end */
      /* printf("BREAK\n"); */
      break;
    }
    n++;
    lh=lh->next;
  }

  /*   printf("m addded:%s\n",cad); */
  list=malloc(sizeof(struct TextMessageList));
  MemUsed(MADD,+sizeof(struct TextMessageList));
  if(list==NULL){
    fprintf(stderr,"ERROR in malloc Add2TextMessageList()\n");
    exit(-1);
  }
  list->info.source=source;
  list->info.dest=dest;
  list->info.id=mid;
  list->info.value=priority;
  list->info.time=GetTime();
  list->info.print=0;
  list->info.duration=time;
  strncpy(list->info.text,cad,MAXTEXTLEN);

  if(lh->next==NULL){ /* is the lastest */
    list->next=NULL;
    lh->next=list;
  }
  else{
    list->next=lh->next->next;
    lh->next=list;
  }
  /* printf("message (%s) [%d %d %d %d]added %d %d\n",cad, */
  /* 	 list->info.source,list->info.dest,list->info.id,list->info.value,n,list->info.duration); */

  listhead->info.n++;
  /*   gdk_beep(); */
  return (0);
}


int GetPrice(Object *obj,int stype,int eng,int weapon){
  /*
    return:
    -1 if there are some error
    the price of the object obj
    if obj is NULL the price given for the other parametres.
   */
  static int ship_price[]={PRICESHIP0,PRICESHIP1,PRICESHIP2,PRICESHIP3,PRICESHIP4,PRICESHIP5,PRICESHIP6,PRICESHIP7};
  static int engine_price[]={PRICEENGINE0,PRICEENGINE1,PRICEENGINE2,PRICEENGINE3,PRICEENGINE4,PRICEENGINE5};
  static int weapon_price[NUMWEAPONS]={PRICECANNON0,PRICECANNON1,PRICECANNON2,PRICECANNON3,PRICECANNON4,PRICECANNON5,PRICECANNON6,PRICECANNON7,PRICECANNON8,PRICECANNON9};
  int price=0;
  int level=0;

  
  if(obj!=NULL){
    if(obj->type!=SHIP){
      fprintf(stdout,"ERROR in GetPrice() obj type:%d\n",obj->type);
      return(-1);
    }

    stype=obj->subtype;
    eng=obj->engine.type;
    weapon=obj->weapon0.type;
    level=obj->level;

    if(obj->type==SHIP && obj->subtype==PILOT){ 
      /* TODO show this only in human gamer */
      /*      fprintf(stdout,"(%d)pilots can not be upgraded\n",obj->player); */
      eng=ENGINE0;
      weapon=CANNON0;
      // return(-1);
    } 
  }

  if(stype<SHIP0 || stype>SHIP_S_MAX){
    fprintf(stderr,"ERROR in GetPrice() stype id:%d\n",stype);
    return(-1);
  }
  if(eng<ENGINE0 || eng>ENGINEMAX){
    fprintf(stderr,"ERROR in GetPrice() engine id:%d\n",eng);
    return(-1);
  }
  if(weapon<CANNON0 || weapon>CANNONMAX){
    fprintf(stderr,"ERROR in GetPrice() weapon id:%d\n",weapon);
    return(-1);
  }

  price+=ship_price[stype];
  price+=engine_price[eng];
  price+=weapon_price[weapon];
  price*=(level+1);

  return(price);
}

int BuyShip(struct Player player,Object *obj,int type){
  /*
    buy and create an object of type type
    returns:
    an error code:
    0 ok 
    1 obj is null
    2 obj is not landed
    3 player in a enemy planet
    4 Code error, must not happen
    5 Error in GetPrice()
    6 Not enough gold.
  */

  Object *obj_b;
  Segment *s;
  float r;
  int price=0;

  if(obj==NULL)return(SZ_OBJNULL);
  if(obj->mode!=LANDED)return(SZ_OBJNOTLANDED);
  if(obj->player!=obj->in->player && type==TOWER){
    printf("Warning: Buyship() player: %d planet: %d. No owned.\n",player.id,obj->in->player);
    return(SZ_NOTOWNPLANET);
  }

  if(obj->type==SHIP && obj->subtype==PILOT && type!=FIGHTER){ /* pilot buy a ship */
    printf("A pilot only can buy FIGHTERS\n");
    return(SZ_NOTALLOWED);
  }


  /*   if(obj->player==game.nplayers+1)return; */

  /*   printf("BuyShip(): type=%d\n",type); */

  switch(type){
  case EXPLORER:
    price=GetPrice(NULL,EXPLORER,ENGINE3,CANNON3);
    break;
  case FIGHTER:
    price=GetPrice(NULL,FIGHTER,ENGINE4,CANNON4);
    break;
  case TOWER:
    price=GetPrice(NULL,TOWER,ENGINE1,CANNON4);
    break;
  default:
    fprintf(stderr,"WARNING in Buyship() ship type %d not implemented\n",type);
    return(SZ_NOTIMPLEMENTED);
    break;

  }
  if(price<0)return(SZ_UNKNOWNERROR);
  if(players[obj->player].gold < price)return(SZ_NOTENOUGHGOLD);
  

  if(obj->type==SHIP && obj->subtype==PILOT){ /* pilot buy a ship */
    float shield=obj->shield;
    ShipProperties(obj,type,obj->in);
    obj->cost*=pow(2,obj->level);
    obj->subtype=type;
    obj->shield=shield;
    obj->a=PI/2;
    obj->ai=1;
    obj->durable=FALSE;
    obj->mode=LANDED;
    obj->habitat=H_PLANET;
    //    obj->selected=FALSE;
    if(obj->in->type!=PLANET){
      fprintf(stderr,"ERROR BuyShip() pilot buying\n");
    }

    if(GameParametres(GET,GNET,0)==TRUE){
      SetModified(obj,SENDOBJALL);
    }
    players[obj->player].gold -=price;
    return(SZ_OK);
  }

  s=LandZone(obj->in->planet);
  if(s==NULL){
    fprintf(stderr,"ERROR BuyShip(): Segment==NULL\n");
    exit(-1);
  }

  switch(type){
  case EXPLORER:
    obj_b=NewObj(&listheadobjs,SHIP,EXPLORER,
		 obj->x,s->y0,
		 0,0,
		 CANNON3,ENGINE3,obj->player,NULL,obj->in);
    break;
  case FIGHTER:
    obj_b=NewObj(&listheadobjs,SHIP,FIGHTER, 
		 obj->x,s->y0,
		 0,0,
		 CANNON4,ENGINE4,obj->player,NULL,obj->in);
    break;
  case TOWER:
    obj_b=NewObj(&listheadobjs,SHIP,TOWER,
		 obj->x,s->y0,
		 0,0,
		 CANNON4,ENGINE1,obj->player,NULL,obj->in);
    break;
  default:
    obj_b=NULL;
    fprintf(stderr,"ERROR in Buyship() ship type %d not implemented\n",type);
    return(SZ_NOTIMPLEMENTED);
    break;

  }
  if(obj_b!=NULL){
    players[obj->player].gold -=price;
    /* obj_b->y+=obj_b->radio+1; */
    r=s->x1-s->x0-2*obj_b->radio;
    
    obj_b->x=s->x0+obj_b->radio+r*(Random(-1));
    //    obj_b->x=s->x0+obj_b->radio+r*(obj_b->pid%20)/20.;
    obj_b->y+=obj_b->radio+1;
    obj_b->player=obj->player;
    obj_b->a=PI/2;
    obj_b->habitat=obj->habitat;
    obj_b->planet=NULL;
    obj_b->gas=obj_b->gas_max*(.5+.5*(Random(-1)));
    obj_b->mode=LANDED;

    players[obj_b->player].nbuildships++;

    Add2ObjList(&listheadobjs,obj_b);
    if(GameParametres(GET,GNET,0)==TRUE){
      SetModified(obj,SENDOBJNEW);
    }
  }
  else{
    fprintf(stderr,"ERROR in BuyShip()\n");
  }
  return(SZ_OK);
}



Object *NextCv(struct HeadObjList *lh,Object *cv0,int pid){
  /*
    returns:
    if cv0 is NULL a pointer to the first ship
    if cv0 != NULL a pointer to the next ship in the list
    
  */
  
  struct ObjList *ls;
  Object *obj1=NULL;
  int sw1=0;
  int swcv=0;
  
  ls=lh->next;
  if(ls==NULL){
    printf("There are no ship selected!!\n");
    return(NULL);
  }
  
  while(ls!=NULL){   /* find the actual cv */
    if(ls->obj->player!=pid){ls=ls->next;continue;}
    if(ls->obj->type!=SHIP){ls=ls->next;continue;}
    if(ls->obj->subtype==PILOT && ls->obj->habitat==H_SHIP){ls=ls->next;continue;}

    if(!sw1){
      if(cv0==NULL){
	return(ls->obj); /* first found */
      }
      obj1=ls->obj;
      sw1=1;
    }
    if(!swcv && cv0!=NULL){
      if(ls->obj==cv0)
	swcv=1;
    }
    if(swcv && ls->obj!=cv0){
      /* 	printf("obj(1):%p %d\n",ls->obj,ls->obj->id); */
      //      if(cv0!=NULL)cv0->selected=FALSE;
      return(ls->obj); /* first after cv */
    }
    
    ls=ls->next;
  }
  /*   printf("obj(2):%p\n",obj1); */
  //  if(obj1!=NULL)cv0->selected=FALSE;
  return(obj1);
}

Object *PrevCv(struct HeadObjList *lh,Object *cv0,int pid){
  /*
    returns:
    if cv0 is NULL a pointer to the first ship
    if cv0 != NULL a pointer to the previous ship in the list
    
  */
  
  struct ObjList *ls;
  Object *obj,*pobj;
  
  obj=pobj=NULL;

  ls=lh->next;
  if(ls==NULL){
    printf("There are no ship selected!!\n");
    return(NULL);
  }

  while(ls!=NULL){
    obj=ls->obj;
    if(obj->player==pid && obj->type==SHIP){
      if(obj->subtype==PILOT && obj->habitat==H_SHIP){ls=ls->next;continue;}
      if(cv0==NULL)return(obj); /* first found */
      else{
	if(obj!=cv0)pobj=obj;
	if(obj==cv0){
	  if(pobj!=NULL)return(pobj);
	}
      }
    }
    ls=ls->next;
  }
  if(pobj==NULL && cv0!=NULL)return(cv0);
  return(pobj);
}


Object *FirstShip(struct HeadObjList *lh,Object *cv0,int pid){
  /*
    returns:
    a pointer to the first ship in free space.
    if not, the actual selected cv0.
  */
  
  struct ObjList *ls;
  
  ls=lh->next;
  if(ls==NULL){
    printf("There are no ship selected!!\n");
    return(NULL);
  }
  
  while(ls!=NULL){ 
    if(ls->obj->player!=pid){ls=ls->next;continue;}
    if(ls->obj->type!=SHIP){ls=ls->next;continue;}
    if(ls->obj->subtype==PILOT && ls->obj->habitat==H_SHIP){ls=ls->next;continue;}
    
    if(ls->obj->habitat==H_SPACE){
      return(ls->obj); /* first found */
    }
    
    
    ls=ls->next;
  }
  /*   printf("obj(2):%p\n",obj1); */
  //  if(obj1!=NULL)cv0->selected=FALSE;
  return(cv0);
}

Object *PrevPlanetCv(struct HeadObjList *lh,Object *cv0,int playerid){
  /*
    version 01
    returns:
    if cv0 is NULL a pointer to the first planet
    if cv0 != NULL a pointer to the previous planet in the list
  */
  
  struct ObjList *ls;
  Object *pobj=NULL;
  int pplanet=-1;
  int planet0=-1; 
  int planet;
  
  ls=lh->next;
  if(ls==NULL){
    printf("There are no ship selected!!\n");
    return(NULL);
  }
  if(cv0!=NULL){
    planet0=cv0->habitat==H_PLANET ? cv0->in->id : 0;
  }

  while(ls!=NULL){

    if(ls->obj->player!=playerid){ls=ls->next;continue;}
    if(ls->obj->type!=SHIP){ls=ls->next;continue;}
    if(ls->obj->subtype==PILOT && ls->obj->habitat==H_SHIP){ls=ls->next;continue;}
    if(cv0==NULL)return(ls->obj);

    planet=ls->obj->habitat==H_PLANET ? ls->obj->in->id : 0;
    if(planet==planet0){
      if(pobj!=NULL)return(pobj);
      else{
	ls=ls->next;continue;
      }
    }
    else{
      if(pplanet!=planet){
	pobj=ls->obj;
	pplanet=planet;
      }
    }
    ls=ls->next;
  }
  if(pobj==NULL && cv0!=NULL)return(cv0);
  return(pobj);
}


Object *NextPlanetCv(struct HeadObjList *lh,Object *cv0,int playerid){
  /*
    returns:
    if cv0 is NULL a pointer to the first ship 
    if cv0 != NULL a pointer to the first ship in next planet in the list
  */
  
  struct ObjList *ls;
  Object *obj1;
  int sw1=0;
  int swcv=0;
  int planet0=0;

  
  ls=lh->next;
  if(ls==NULL){
    printf("There are no ship selected!!\n");
    return(NULL);
  }
  
  obj1=NULL;
  if(cv0!=NULL){
    if(cv0->habitat==H_PLANET){
      planet0=cv0->in->id;
    }
  }
  
  while(ls!=NULL){   /* find the actual cv */
    if(ls->obj->player==playerid && ls->obj->type==SHIP){
      if(ls->obj->subtype==PILOT && ls->obj->habitat==H_SHIP){ls=ls->next;continue;}
      if(!sw1){
	if((cv0==NULL || planet0==0) && ls->obj->habitat==H_PLANET){
	  return(ls->obj); /* first found */
	}
	obj1=ls->obj; 
	sw1=1;
      }
      if(!swcv && cv0!=NULL){
	if(ls->obj==cv0)
	  swcv=1;
      }
      if(swcv && ls->obj!=cv0 && ls->obj->habitat==H_PLANET){
	/* 	printf("obj(1):%p %d\n",ls->obj,ls->obj->id); */
	if(planet0==0){
	  return(ls->obj); /* first after cv */
	}
	else{
	  if(planet0!=ls->obj->in->id){
	    return(ls->obj);
	  }
	}
      }
    }
    ls=ls->next;
  }
  /*   printf("obj(2):%p\n",obj1); */
  return(obj1);
}


int CountObjList(struct HeadObjList *hlist){
  /*
    returns:
    the number of item of the list hlist.
   */
  struct ObjList *ls;
  int n=0;  

  if(hlist==NULL)return(0);
  ls=hlist->next;
  while(ls!=NULL){
    n++;
    ls=ls->next;
  }
  return n;
}

int DestroyObjList(struct HeadObjList *hl){
  /*
    free memory for the list hl.
   */
  struct ObjList *ls0;
  int n=0;
  long memused=0;
  
  if(hl==NULL)return(0);

  while(hl->next!=NULL){
    ls0=hl->next;
    hl->next=hl->next->next;
    ls0->obj=NULL;
    ls0->next=NULL;
    free(ls0);
    ls0=NULL;
    memused-=sizeof(struct ObjList);
    hl->n--;
    n++;
  }
  MemUsed(MADD,memused);

  if(hl->next!=NULL){
    fprintf(stderr,"ERROR 1 in DestroyObjList()\n");
    exit(-1);
  }
  if(hl->n!=0){
    fprintf(stderr,"ERROR 2 in DestroyObjList()  n:%d  del:%d\n",hl->n,n);
    hl->n=0;
    hl->next=NULL;
    /*     exit(-1); */
  }
  return(n);
}

int PrintObjList(struct HeadObjList *hl){
  /*
    print some info of the list hl
    DEBUG only
   */  
  if(hl==NULL)return(0);
  printf("List: ");
  while(hl->next!=NULL){
    printf("%d ",hl->next->obj->id);
    hl->next=hl->next->next;    
  }
  printf("\n");
  return(0);
}


int IsInObjList(struct HeadObjList *lhobjs,Object *obj){

  /*
    NOTUSED
    Look for obj in the list lhobjs
    returns:
    0 if obj is not in the list
    1 if is in the list
   */

  struct ObjList *ls;
  if(lhobjs==NULL)return(-1);
  if(obj==NULL)return(0);

  ls=lhobjs->next;
  while(ls!=NULL){
    if(ls->obj==obj)return(1);
    ls=ls->next;
  }
  return(0);
}

void KillAllObjs(struct HeadObjList *lheadobjs){
  /*
    Kill all objects setting their state and life to zero. 
   */
  struct ObjList *ls;

  if(lheadobjs==NULL)return;  
  ls=lheadobjs->next;

  while(ls!=NULL){
    ls->obj->life=0;
    ls->obj->state=0;
    ls->obj->sw=0;
    ls=ls->next;
  }
}


int CreatePlayerList(struct HeadObjList hlist1,struct HeadObjList *hlist2,int player){
  /* 
     version 04 
     Create a list in hlist2 only with ships an planets 
     that belongs to player player from the list hlist1.
     return:
     the number of elements of the list.
*/

  struct ObjList *ls0,*ls1,*ls2;
  Object *objin;
  int id1,id2;
  int n=0;
  long memused=0;


  if(hlist2->next!=NULL){
    fprintf(stderr,"WARNING CPL not NULL %p %d\n",hlist2->next,hlist2->n);
  }

  hlist2->n=0;
  hlist2->next=NULL;
  ls0=ls1=ls2=NULL;

  for(ls1=hlist1.next;ls1!=NULL;ls1=ls1->next){

    switch(ls1->obj->type){
    case SHIP:
      if(ls1->obj->player!=player)continue;
      break;
    case PLANET:
      if(players[ls1->obj->player].team!=players[player].team)continue;
      break;
    default:
      continue;
      break;
    }

    if(ls1->obj->subtype==SATELLITE)continue;
    if(ls1->obj->state<=0)continue;

    
    id1=ls1->obj->id; /* id of the planet in which is contained, 0 for free space*/
    if(ls1->obj->type!=PLANET)id1=0;

    /* HERENEW */

    if(ls1->obj->in!=NULL){ /* not a planet */
      objin=ls1->obj->in;
      while(objin!=NULL){
	id1=objin->id;
	if(objin->type!=PLANET)id1=0;
	objin=objin->in;
      }
    }

    ls0=malloc(sizeof(struct ObjList));
    memused+=sizeof(struct ObjList);
    //    MemUsed(MADD,+sizeof(struct ObjList));
    if(ls0==NULL){
      fprintf(stderr,"ERROR in malloc CreateList()\n");
      exit(-1);
    }
    ls0->obj=ls1->obj;
    ls0->next=NULL;
    

    if(hlist2->next==NULL){ /* first item */
      hlist2->next=ls0;
      hlist2->n++;
      n++;
      /*      printf("%d ",ls0->obj->id); */
      continue;
    }

    id2=hlist2->next->obj->id;
    if(hlist2->next->obj->type!=PLANET)id2=0;
  
    if(hlist2->next->obj->in!=NULL){
      objin=hlist2->next->obj->in;
      while(objin!=NULL){
	id2=objin->id;
	if(objin->type!=PLANET)id2=0;
	objin=objin->in;
      }
    }



    if(id1 < id2 ){
      ls0->next=hlist2->next;
      hlist2->next=ls0;
      hlist2->n++;
      n++;
      /*      printf("%d ",ls0->obj->id); */
      continue;
    }

    for(ls2=hlist2->next;ls2->next!=NULL;ls2=ls2->next){

      id2=ls2->next->obj->id;
      if(ls2->next->obj->type!=PLANET)id2=0;
     
      if(ls2->next->obj->in!=NULL){
	objin=ls2->next->obj->in;
	while(objin!=NULL){
	  id2=objin->id;
	  if(objin->type!=PLANET)id2=0;
	  objin=objin->in;
	}
      }


      if(id1 < id2 )break;
    }

    ls0->next=ls2->next;
    ls2->next=ls0;

    hlist2->n++;
    n++;
  }
  MemUsed(MADD,memused);
  return(n) ;
}


int CreateContainerLists(struct HeadObjList *lh,struct HeadObjList *hcontainer){
  /* 
     Create a list with all the objects that can collide.
     Create a container list for every planet
     Add to hcontainer all the objects in each planet plus free space objects
     returns:
     0
   */
  struct ObjList *ls;
  Object *obj;
  int nplanets;
  int i,n;
  int proc;
  int value0;
  int gulx,guly;


  for(i=0;i<GameParametres(GET,GNPLANETS,0)+1;i++){
    if(hcontainer[i].next!=NULL){ 
      fprintf(stderr,"WARNING: CCL() not NULL\n"); 
    } 
    hcontainer[i].n=0;
  }

  nplanets=GameParametres(GET,GNPLANETS,0);
  gulx=GameParametres(GET,GULX,0);
  guly=GameParametres(GET,GULY,0);

  proc=GetProc();

  ls=lh->next;
  while(ls!=NULL){
    
    if(ls->obj->state<=0 && proc==players[ls->obj->player].proc){
      ls=ls->next;continue;
    }
    /* objects than dont collide */
    switch(ls->obj->type){
    case PROJECTILE:
      if(ls->obj->subtype==EXPLOSION && ls->obj->habitat==H_SPACE){  
	  ls=ls->next;continue;     
      }    
      break;
    case TRACE:
    case TRACKPOINT:
      ls=ls->next;
      continue;
      break;
    case SHIP:
      if(ls->obj->subtype==PILOT && ls->obj->mode==LANDED){
	ls=ls->next;
	continue;
      }
      break;
    default:
      break;
    }
    /* --objects than dont collide */

    switch(ls->obj->habitat){
    case H_SPACE:
      n=0;
      break;
    case H_PLANET:
      n=ls->obj->in->id;
      break;
    case H_SHIP:
      ls=ls->next;continue;
      break;
    default:
      n=0;
      break;
    }

    /* check */

    if(n<0||n>nplanets){
      //      if(ls->obj->subtype==PILOT){ls=ls->next;continue;}
      fprintf(stderr,"ERROR 1 in CreateContainerlists()\n");
      fprintf(stderr,"\t DEBUG info:   n: %d objid:%d type: %d stype: %d\n",n,ls->obj->id,ls->obj->type,ls->obj->subtype);
      n=0;
    }

    obj=ls->obj;
    
    /**** CELLON****/
    if(n==0){
      switch(obj->type){
      case PLANET:
	value0=1;
	//	printf("planet\n");
	break;
      case ASTEROID:
	value0=2;
	break;
      case SHIP:
	value0=1<<(players[obj->player].team+3);
	if(obj->subtype==PILOT)value0=4;
	break;
      case PROJECTILE:
	value0=1<<(players[obj->player].team+3);
	break;
      default:
	ls=ls->next;
	continue;
	break;
      }
      if(ValueCell(cell,obj,gulx,guly)==value0){
	//	printf("type. %d \n",obj->type);
	ls=ls->next;continue;
      }
    }
    /*****CELLON*****/

    Add2ObjList(&hcontainer[n],ls->obj);
    ls=ls->next;
  }
  return(0);
}



int CreatekplanetsLists(struct HeadObjList *lh,struct HeadObjList *hkplanets){
  /* 
     Create a list with all the known planets.
     returns the number of objects of the list.
   */
  struct ObjList *ls;
  int gnplayers;
  int i;
  int proc;

  proc=GetProc();
  gnplayers=GameParametres(GET,GNPLAYERS,0);

  for(i=0;i<gnplayers+1;i++){
    if(hkplanets[i].next!=NULL){ 
      fprintf(stderr,"WARNING: CkpL() not NULL\n"); 
    } 
    hkplanets[i].n=0;
  }

  ls=lh->next;
  while(ls!=NULL){

    if(ls->obj->type!=PLANET){ls=ls->next;continue;}

    for(i=0;i<gnplayers+2;i++){
      if(proc==players[i].proc){
	if(IsInIntList(players[i].kplanets,ls->obj->id)){
	  Add2ObjList(&hkplanets[i],ls->obj);
	}
      }
    }
    ls=ls->next;
  }

  return(0);
}

int CreatePlanetList(struct HeadObjList lheadobjs,struct HeadObjList *lheadplanets){
  /*
    version 0.1
    Create a list of planets from the object list lheadobjs 
    to obj. list lheadplanets.
    returns:
    the number of planets added to the list.
   */


  struct ObjList *ls,*ls0,*lsp;
  long memused=0;

  if(lheadplanets->next!=NULL){ 
    //    fprintf(stderr,"WARNING: CPL() not NULL\n"); 
    DestroyObjList(lheadplanets);
  } 
  lheadplanets->n=0;
  
  ls=lheadobjs.next;
  ls0=NULL;
  lsp=NULL;

  while(ls!=NULL){
    if(ls->obj->type==PLANET){

      ls0=malloc(sizeof(struct ObjList));
      memused+=sizeof(struct ObjList);

      if(ls0==NULL){
	fprintf(stderr,"ERROR in malloc() CreatePlanetList()\n");
	exit(-1);
      }

      ls0->obj=ls->obj;
      ls0->next=NULL;

      if(lheadplanets->n==0){
	lheadplanets->next=ls0;
	lsp=ls0;
      }
      else{
	lsp->next=ls0;
	lsp=ls0;
      }
      lheadplanets->n++;
    }
    ls=ls->next;
  }
  MemUsed(MADD,memused);
  return(lheadplanets->n);
}


void CreateNearObjsList(struct HeadObjList *lh,struct HeadObjList *lhn,int player){
  /*
    Create a list of enemy ships in radar range of the player player
  */

  struct ObjList *ls1,*ls2;
  Object *obj1,*obj2;

  float rx,ry,r2;
  int sw=0;
  int gnet,proc;

  gnet=GameParametres(GET,GNET,0);
  proc=GetProc();

  if(lhn->next!=NULL){ 
    fprintf(stderr,"WARNING: CNOL() not NULL\n"); 
  } 
  lhn->n=0;
  ls1=lh->next;
  while(ls1!=NULL){ 
    sw=0;
    /* obj1 is an enemy */
    
    obj1=ls1->obj;
    
    if(players[obj1->player].team==players[player].team){
      ls1=ls1->next;continue;
    }

    if(obj1->type!=SHIP){
      ls1=ls1->next;continue;
    }

    if(obj1->habitat==H_PLANET){
      ls1=ls1->next;continue;
    }

    if(gnet==TRUE){
      if(proc!=players[obj1->player].proc){
	if(obj1->ttl<MINTTL){ls1=ls1->next;continue;}
      }
    }
    
    ls2=lh->next;
    while(ls2!=NULL && sw==0){ 
      /* obj2 is an ally */
      obj2=ls2->obj;
      if(obj2->player!=player){
	ls2=ls2->next;continue;
      }
      
      if(obj2->type!=SHIP){
	ls2=ls2->next;continue;
      }
      
      if(obj2->habitat==H_PLANET){
	rx=obj2->in->x - obj1->x;
	ry=obj2->in->y - obj1->y;
      }
      else{
	rx=obj2->x - obj1->x;
	ry=obj2->y - obj1->y;
      }
      r2=rx*rx+ry*ry;
      
      if(r2<obj2->radar*obj2->radar){
	Add2ObjList(lhn,obj1);
	sw=1;
      }
      ls2=ls2->next;
    }
    ls1=ls1->next;
  }
}



void Experience(Object *obj,float pts){
  /*
    Increase ships level. 
    
   */

  float mulshots;

  if(obj->state<=0){
    return;
  }
  if(obj->type!=SHIP)return;
  if(obj->subtype==PILOT)return;
  //  if(GetProc()!=players[obj->player].proc)return;

  players[obj->player].points+=pts;
  if(players[obj->player].points>=record){
    record=players[obj->player].points;
  }

  obj->experience+=pts;

  if(obj->type==SHIP){
    while(obj->experience>=100*pow(2,obj->level)){
      obj->experience-=100*pow(2,obj->level);
      obj->level++;
      //HERE      obj->ttl=0;

      if(GameParametres(GET,GNET,0)==TRUE){
	/* if(GetProc()==players[obj->player].proc){ */
	  SetModified(obj,SENDOBJALL);
	/* } */
      }
      if(obj->level > players[obj->player].maxlevel){
	players[obj->player].maxlevel=obj->level;
      }
      
      obj->state=100;
      obj->gas=obj->gas_max;
      obj->shield+=(.9-obj->shield)/3.;
      obj->engine.gascost-=.01;
      obj->engine.v_max++;
      obj->cost*=2.0;

      mulshots=1+1./(obj->level);

      if(obj->weapon0.type!=CANNON0){
	obj->weapon0.max_n+=50;
	if(obj->weapon0.rate>9){
	  obj->weapon0.rate--;
	}
	if(obj->weapon0.nshots<4){
	  if(!(obj->level%2)){
	    obj->weapon0.nshots++;
	  }
	}
	obj->weapon0.projectile.damage*=1.+.2*DAMAGEFACTOR;
      }
      
      if(obj->weapon1.type!=CANNON0){
	if(obj->weapon1.rate>9){
	  obj->weapon1.rate--;
	}
	if(obj->weapon1.max_n<10){ /* max 11 missiles */
	  obj->weapon1.max_n+=2;
	}
	obj->weapon1.projectile.damage*=1.+.2*DAMAGEFACTOR;
      }

      if(obj->weapon2.type!=CANNON0){
	obj->weapon2.max_n*=mulshots;
	if(obj->weapon2.rate>9){
	  obj->weapon2.rate--;
	}
	obj->weapon2.projectile.damage*=1.+.2*DAMAGEFACTOR;
      }

      if(obj->shield>.9)obj->shield=.9;
      if(obj->engine.gascost<.01)obj->engine.gascost=.01;
      if(obj->engine.v_max>VELMAX)obj->engine.v_max=VELMAX;

      obj->engine.v2_max=obj->engine.v_max*obj->engine.v_max;

      if(obj->subtype==FIGHTER){
	switch(obj->level){
	case 1:
	  if(obj->weapon1.type==CANNON0){
	    NewWeapon(&obj->weapon1,CANNON8);
	  }
	  break;
	case 2:
	  if(obj->weapon2.type==CANNON0){
	    NewWeapon(&obj->weapon2,CANNON9);
	  }
	  break;
	default:
	  break;
	}
      }
    }
  }
}

void PrintObj(Object *obj){
  printf("obj id:%d pid: %d type: %d stype: %d\n",
	 obj->id,obj->pid,obj->type,obj->subtype);
}

char Type(Object *obj){
  /*
    returns:
    a char 
   */

  char mode=' ';
  if (obj==NULL)return(mode);

  if(obj->type==SHIP){
    switch (obj->subtype){
    case FIGHTER:
      mode='F';
      break;
    case EXPLORER:
      mode='E';
      break;
    case TOWER:
      mode='T';
      break;
    case QUEEN:
      mode='Q';
      break;
    case PILOT:
      mode='A';
      break;
    default:
      mode='O';
      break;
    }
  }
  return(mode);
}

char *TypeCad(Object *obj){
  /*
    returns:
    a cad with the type of ship
   */

  static char cad[16]="";
  if (obj==NULL)return(cad);

  if(obj->type==SHIP){
    switch (obj->subtype){
    case FIGHTER:
      strcpy(cad,"FIGHTER");
      break;
    case EXPLORER:
      strcpy(cad,"EXPLORER");
      break;
    case TOWER:
      strcpy(cad,"TOWER");
      break;
    case QUEEN:
      strcpy(cad,"QUEEN");
      break;
    case PILOT:
      strcpy(cad,"PILOT");
      break;
    default:
      strcpy(cad,"");
      break;
    }
  }
  return(cad);
}


Object *MarkObjs(struct HeadObjList *lh,Space reg,Object *cv,int ctrl){
  /*
    Set the selected mark on in all ship inside the region reg.
    return a pointer to the first selected obj
    if cv is selected returns cv.

  */

  struct ObjList *ls;
  int n=0;
  Rectangle rect;
  int x,y;
  Point a,b;
  int sw1=0; /* first selected */
  Object *ret;

  if(lh==NULL)return(NULL);

  ret=NULL;
  rect.x=reg.rect.x;
  rect.y=reg.rect.y;
  rect.width=reg.rect.width;
  rect.height=reg.rect.height;

  if(reg.rect.width<0){
    rect.x+=reg.rect.width;
    rect.width*=-1;
  }

  if(reg.rect.height<0){
    rect.y+=reg.rect.height;
    rect.height*=-1;
  }

  if(reg.habitat>0){ /* select a region inside a planet */
    
    a.x=rect.x;
    a.y=rect.y-rect.height;
    b.x=rect.x+rect.width;
    b.y=rect.y;
    
  }
  else{ /* select a region in map view */
    
    a.x=rect.x;
    a.y=rect.y;
    b.x=rect.x+rect.width;
    b.y=rect.y+rect.height;
  }

  ls=lh->next;
  while(ls!=NULL){
    if(ctrl==FALSE)ls->obj->selected=FALSE;
    if(ls->obj->player!=actual_player){ls=ls->next;continue;}
    if(ls->obj->type!=SHIP){ls=ls->next;continue;}

    if(reg.habitat>0){  /* select a region inside a planet */
      if(ls->obj->habitat==H_SPACE){ls=ls->next;continue;}
      if(reg.habitat!=ls->obj->in->id){ls=ls->next;continue;}

      if(ls->obj->x > a.x && ls->obj->x < b.x ){
	if(ls->obj->y+ls->obj->radio > a.y && ls->obj->y+ls->obj->radio < b.y ){
	  if(sw1==0){
	    if(ls->obj->habitat!=H_SHIP){
	      ret=ls->obj;
	      sw1++;
	    }
	  }
	  ls->obj->selected=TRUE;
	  n++;
	}
      }
    }
    else{ /* select a region in free space */
      if(ls->obj->habitat==H_PLANET){
	x=ls->obj->in->planet->x;
	y=ls->obj->in->planet->y;
      }
      else{
	x=ls->obj->x;
	y=ls->obj->y;
      }

      if(x > a.x && x < b.x ){
	if(y > a.y && y < b.y ){
	  if(sw1==0){
	    if(ls->obj->habitat!=H_SHIP){
	      ret=ls->obj;
	      sw1++;
	    }
	  }
	  ls->obj->selected=TRUE;
	  n++;
	}
      }
    }
    ls=ls->next;
  }
  if(cv!=NULL){
    if(cv->selected==TRUE)return(cv);
  }
  return(ret);
}


void UnmarkObjs(struct HeadObjList *lh){
  /*
    unmark all objects
  */

  struct ObjList *ls;
  
  if(lh==NULL)return;
  ls=lh->next;
  while(ls!=NULL){
    ls->obj->selected=FALSE;
    ls=ls->next;
  }
  return;
}


int PrintSelected(struct HeadObjList *lh){
  /*
    printf the selected ships
    return the number of selected ships
  */

  struct ObjList *ls;
  int n=0;
  
  if(lh==NULL)return(0);
  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->selected==TRUE){
      printf("\t%c %d\n",Type(ls->obj),ls->obj->pid);
      n++;
    }
    ls=ls->next;
  }
  return n;
}

int NearMaxLevelObj(Object *obj,struct HeadObjList *lh){
  /* 
     Not USED.
     return the max level of the objects in the list 
     that belongs to same player that obj.
     pilots are not counted
*/

  struct ObjList *ls;
  int maxlevel =0;


  if(lh==NULL)return(0);
  ls=lh->next;
  while(ls!=NULL){ 
    if(obj==ls->obj){ls=ls->next;continue;}
    if(ls->obj->player!=obj->player){ls=ls->next;continue;}
    if(ls->obj->type==SHIP && ls->obj->subtype==PILOT){ls=ls->next;continue;}
    if(ls->obj->level>maxlevel)maxlevel=ls->obj->level;
    ls=ls->next;
  } /* while(ls1!=NULL) */
  while(ls!=NULL);
  return(maxlevel); 
}

int IsPlanetEmpty(Object *planet,Object *obj){
  /* 
     don't count obj
     don't count pilots
     returns:
     0 if the planet is totally empty.
     1 if there are only allies landed.
     2 if there are enemy ships landed.

  */

  struct ObjList *ls;
  int gnet;
  int ret=0;

  /* PRODUCTION remove messages */

  if(obj==NULL)return(-1);
  if(planet==NULL)return(-1);
  if(planet->player==0)return(0); /* planet never conquered */ 

  gnet=GameParametres(GET,GNET,0);
  ls=listheadobjs.next;
  while(ls!=NULL){
    if(ls->obj->in==planet){
      if(ls->obj->type!=SHIP){ls=ls->next;continue;}
      if(ls->obj->subtype==PILOT){ls=ls->next;continue;}
      if(ls->obj->mode!=LANDED){ls=ls->next;continue;}
      if(ls->obj==obj){ls=ls->next;continue;}
      if(gnet && ls->obj->ttl<MINTTL){ls=ls->next;continue;}
      if(players[ls->obj->player].team!=players[obj->player].team){
	/* printf("IPE: player: %d type: %d stype: %d pid: %d mode: %d planet: %d st: %f ttl: %d\n", */
	/*        ls->obj->player,ls->obj->type,ls->obj->subtype,ls->obj->pid,ls->obj->mode,ls->obj->in->id,ls->obj->state,ls->obj->ttl); */
	return(2);
      }
      else{
	if(ls->obj->player!=obj->player)ret=1;
      }
    }
    ls=ls->next;
  }
  //  printf("PLANET EMPTY\n");
  return(ret);
}

int UpdateCell(struct HeadObjList *lh,int *cell){
  /* 
     version 02 080411
*/
  struct ObjList *ls;
  Object *obj;
  int n=0;
  int dx,dy;
  int nx0,ny0,nx,ny;
  int nydx;
  int dx2,dy2;
  int i,j;
  int value;
  int gnet;

  dx=GameParametres(GET,GULX,0)/DL;
  dy=GameParametres(GET,GULY,0)/DL;
  dx2=dx/2;
  dy2=dy/2;
  gnet=GameParametres(GET,GNET,0);
  ls=lh->next;
  while(ls!=NULL){ 
    if(ls->obj->habitat==H_PLANET){
      ls=ls->next;continue;
    }

    obj=ls->obj;
    switch(obj->type){
    case SHIP:
      if(gnet==TRUE && obj->ttl<MINTTL){ls=ls->next;continue;}
      value=1<<(players[obj->player].team+3);
      if(obj->subtype==PILOT)value=4;
      break;
    case PROJECTILE:
      if(obj->subtype==EXPLOSION){ 
	ls=ls->next;continue; 
      }
      value=1<<(players[obj->player].team+3);
      break;
    case PLANET:
      value=1;
      break;
    case ASTEROID:
      value=2;
      break;
    default:
      ls=ls->next;
      continue;
      break;
    }

    nx0=(obj->x/DL+dx2);
    ny0=(obj->y/DL+dy2);

    for(j=-1;j<=1;j++){
      ny=ny0+j;
      if(ny<0)ny=0;
      else{
	if(ny>=dy)ny=dy-1;
      }
      nydx=ny*dx;
      
      for(i=-1;i<=1;i++){
	nx=nx0+i;
	if(nx<0)nx=0;
	else{
	  if(nx>=dx)nx=dx-1;
	}
	
	if((cell[nx+nydx]&value)==0){
	  cell[nx+nydx]+=value;
	}
      }
    }
    n++;
    ls=ls->next;
  }
  return(n);
}


int ValueCell(int *cell,Object *obj,int gulx,int guly){
  /*
    HERECOM
  */
  int x,y;
  int dx,dy;
  int nx,ny;
  int index;


  if(cell==NULL){
    fprintf(stderr,"ERROR: ValueCell(): cell NULL\n");
    exit(-1); //PRODUCTION only warning, must not be happen
    return(0);
  }
  if(obj==NULL){
    fprintf(stderr,"ERROR: ValueCell(): obj NULL\n");
    exit(-1);
    return(0);
  }

  dx=gulx/DL;
  dy=guly/DL;

  
  if(obj->habitat==H_PLANET){
    x=obj->in->planet->x;
    y=obj->in->planet->y;
  }
  else{
    x=obj->x;
    y=obj->y;
  }
  
  nx=((int)x+(gulx)/2)/DL;
  ny=((int)y+(guly)/2)/DL;
  if(nx<0)nx=0;
  if(ny<0)ny=0;
  if(nx>=dx)nx=dx-1;
  if(ny>=dy)ny=dy-1;
  
  index=nx+ny*dx;

  if(index<0||index>=dx*dy){
    fprintf(stderr,"Error in ValueCell()\n");
    fprintf(stderr,"\tx:%f y:%f  %d %d\n",obj->x,obj->y,nx,ny);
    exit(-1);
    return(0);
  }
  return(cell[index]);
}


  
void ShipProperties(Object *obj,int stype,Object *in){
  /*
    TODO add only physical properties.

   */
  switch(stype){
  case EXPLORER: /* EXPLORER */
    obj->radar=2*RADAR_RANGE;
    obj->gas_max=1000;
    obj->gas=obj->gas_max;
    obj->shield=0;
    obj->state=90;
    obj->mass=50;
    obj->cargo=10; /* TODO not implemented */
    obj->radio=10;
    obj->ai=1;
    obj->damage=25;
    obj->cost=COSTEXPLORER*COSTFACTOR;
    break;
  case SHIP0: /* not used */
    /* HERE there are SHIP0 */
  case SHIP2: /* not used */
  case FIGHTER: /*  FIGHTER */
    obj->gas_max=1000;
    obj->gas=obj->gas_max;
    obj->shield=0;
    obj->state=90;
    obj->mass=100;

    if(stype==FIGHTER && obj->level>=MINLEVELPILOT) {
      obj->items=obj->items|ITSURVIVAL;
    }
    obj->cargo=10; /* TODO not implemented */
    obj->radio=10;
    obj->ai=1;
    obj->damage=25;
    obj->cost=COSTFIGHTER*COSTFACTOR;

    /*       NewWeapon(&obj->weapon1,CANNON8); */
    /*       NewWeapon(&obj->weapon2,CANNON9); */
    break;
  case QUEEN: /*  cargo queen ship */
    obj->gas_max=2000;
    obj->gas=obj->gas_max;
    obj->shield=0.9;
    obj->state=90;
    obj->mass=400;
    obj->cargo=20;
    obj->radio=20;
    obj->ai=1;
    obj->damage=25;
    obj->cost=COSTQUEEN*COSTFACTOR;
    break;
    
  case SATELLITE: /* SATELLITE: */
    obj->durable=TRUE;
    obj->life=2400;
    obj->gas_max=500;
    obj->gas=obj->gas_max;
    obj->shield=0;
    obj->state=99;
    obj->mass=20;
    obj->damage=10;
    obj->radio=5;
    obj->ai=1;
    obj->habitat=obj->parent->habitat;
    obj->in=in;
    obj->planet=NULL;
    obj->cost=COSTSATELLITE*COSTFACTOR;
    break;
  case TOWER: /* TOWER: */
    /*       obj->radar=8*RADAR_RANGE; */
    obj->gas_max=1000;
    obj->gas=obj->gas_max;
    obj->shield=0.5;
    obj->state=99;
    obj->mass=100;
    obj->radio=10;
    obj->ai=1;
    obj->in=in;
    obj->damage=25;
    obj->cost=COSTTOWER*COSTFACTOR;
      break;
  case PILOT: /* PILOT */
    obj->gas_max=0;
    obj->gas=obj->gas_max;
    obj->shield=0;
    obj->state=100;
    obj->mass=10;
    obj->cargo=0; /* TODO not implemented */
    obj->radio=10;
    obj->ai=0;
    obj->habitat=obj->parent->habitat;
    obj->in=in;
    obj->damage=5;
    obj->cost=COSTPILOT*COSTFACTOR;
    break;
  default:
    fprintf(stderr,"ERROR ShipProperties(): unknown subtype %d\n",stype);
    exit(-1);
    break;
  }
}


int CreatePilot( Object *obj){

  /*
    when the ship obj is destroyed and it has a survival pod it is
    ejected.
    returns:
    0 if the ship has not a survival pod
    1 if its created.
   */
  if((obj->items & ITSURVIVAL)==0)return(0);

  /* check */
  if(obj->type==SHIP && obj->subtype!=FIGHTER){
    fprintf(stderr,"ERROR in CreatePilot() type: %d stype: %d\n",obj->type,obj->subtype);
    fprintf(stderr,"\t pilot not created\n");
    return(0);
  }
  
  obj->gas=500;
  if(obj->habitat==H_PLANET){
    obj->y+=20;
    obj->vy+=10;
    if(obj->vy<10)obj->vy=10;
    obj->mode=NAV;
  }
  if(obj->habitat==H_SPACE){
    obj->vx=obj->vx*.65+4*Random(-1)-2;
    obj->vy=obj->vy*.65+4*Random(-1)-2;
  }
  obj->subtype=PILOT;
  obj->items=0;

  obj->ai=0;
  obj->accel=0;
  obj->ang_a=0;
  obj->damage=5;
  obj->durable=TRUE;
  obj->life=5000;
  obj->radio=10;

  obj->state=100;
  obj->weapon0.n=0;
  obj->weapon1.n=0;
  obj->weapon2.n=0;
  return(1);
}

int EjectPilotsObj(struct HeadObjList *lh,Object *obj){
  /*
    Eject pilots from ship obj
    Rescue the transported pilots of dead objs 
    returns:
    the number of pilots ejected.
   */

  struct ObjList *ls;
  Object *pilot;
  Segment *s;
  float r;
  int n=0;
  int gnet;

  if(obj==NULL)return(0);
  if(lh==NULL)return(0);
  if(!(obj->items & ITPILOT))return(0);

  gnet=GameParametres(GET,GNET,0);
  ls=lh->next;
  while(ls!=NULL){

    if(ls->obj->in!=obj){ls=ls->next;continue;}

    if(ls->obj->type!=SHIP){
      fprintf(stderr,"ERROR EjectPilotsObj()from obj:%d (%d) type:%d %d != SHIP parent:%d\n",obj->pid,obj->player,ls->obj->type,ls->obj->subtype,ls->obj->parent->pid);
      ls=ls->next;continue;
    }

    /* Ejecting from a landed ship */
    if(obj->mode==LANDED){
      
      pilot=ls->obj;
      s=LandZone(obj->in->planet);
      
      if(s==NULL){
	fprintf(stderr,"ERROR EjectPilots(): Segment==NULL\n");
	exit(-1);
      }
      r=s->x1-s->x0-2*pilot->radio;
      pilot->in=obj->in;
      pilot->habitat=H_PLANET;
      pilot->experience=0;
      pilot->pexperience=0;
      pilot->planet=NULL;
      pilot->mode=LANDED;
      pilot->x=s->x0+pilot->radio+r*(Random(-1));
      pilot->y=obj->y;
      pilot->x0=pilot->x;
      pilot->y0=pilot->y;
      pilot->accel=0;
      pilot->ang_v=pilot->ang_a=0;
      pilot->vx=pilot->vy=0;
      pilot->a=0;
      pilot->ai=0;
      pilot->items=0;
      //	pilot->selected=FALSE;
      Experience(obj,pilot->level*30); /* experience for rescue a pilot*/
      if(pilot->player==actual_player)printf("Pilot %d saved in planet %d\n",pilot->pid,pilot->in->id);
      DelAllOrder(pilot);
      if(gnet==TRUE){
	SetModified(pilot,SENDOBJALL);
      }
      n++;
      
    }
    /* --Ejecting from a landed ship */


    /* Ejecting from a destroyed  ship */
    if(obj->mode==NAV){
      
      pilot=ls->obj;
      
      pilot->in=obj->in;
      pilot->planet=NULL;
      pilot->habitat=obj->habitat;
      pilot->mode=NAV;
      pilot->x=obj->x;
      pilot->y=obj->y;
      pilot->vx=0.75*obj->vx+6*Random(-1)-3;
      if(obj->habitat==H_PLANET){
	pilot->vy=fabs(0.75*obj->vy+6*Random(-1)-3)+10;
      }else{
	pilot->vy=0.75*obj->vy+6*Random(-1)-3;
      }
      pilot->a=obj->a;
      pilot->ai=0;
      pilot->items=0;
      if(pilot->player==actual_player)printf("Pilot %d ejected from ship %d\n",pilot->pid,obj->pid);

      DelAllOrder(pilot);
      if(gnet==TRUE){
	SetModified(pilot,SENDOBJALL);
	if(pilot->modified!=SENDOBJALL){
	  printf("PILOT EJECT mod: %d\n",pilot->modified);
	}
      }
      n++;
      
    }
    /* --Ejecting from a destroyed  ship */
    
    ls=ls->next;
  }
  return(n);
}

void CheckPilots(struct HeadObjList *hol,Object *cvobj){
  /* 
     check if  destroyed ship has pilots.
  */
  
  struct ObjList *ls;
  int sw=0;
  int gnet;
  int proc=0;  
  ls=hol->next;
  gnet=GameParametres(GET,GNET,0);
  proc=GetProc();

  while(ls!=NULL){
    sw=0;
    if(proc!=players[ls->obj->player].proc){ls=ls->next;continue;}

    if(ls->obj->state<=0)sw=1;
    if(1){
      
      if(sw&&(ls->obj->items & ITPILOT)){
	EjectPilotsObj(hol,ls->obj);
 	ls->obj->items=(ls->obj->items)&(~ITPILOT);
      }
      
      /***** ship destroyed Create Pilot****/
      if(sw&&(ls->obj->items & ITSURVIVAL)){
	sw=0;
	/* GetPointsObj(hol,players,ls->obj);/\* points and experience *\/ */
	if(CreatePilot(ls->obj)){
	  Explosion(&listheadobjs,cvobj,ls->obj,0);
#if SOUND
	  if(ls->obj->habitat==H_PLANET)
	    Play(ls->obj,EXPLOSION0,1);
#endif
	}
	if(ls->obj->player==actual_player){
	  printf("Ejecting pilot from ship %d\n",ls->obj->pid);
	}
	DelAllOrder(ls->obj);
	if(gnet==TRUE){
	  ls->obj->modified=SENDOBJALL;
	  //SetModified(ls->obj,SENDOBJALL);
	  if(ls->obj->modified!=SENDOBJALL){
	    printf("ERROR CheckPilots() (%d)%d mod: %d\n",ls->obj->player,ls->obj->pid,ls->obj->modified);
	  }
	}
      }
    }

    ls=ls->next;    
  }
  return;
}


int CountSelected(struct HeadObjList *lh,int player){
  /*
    printf the selected ships
    return the number of selected ships
  */

  struct ObjList *ls;
  int n=0;
  
  if(lh==NULL)return(0);
  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->selected==TRUE && ls->obj->player==player){
      n++;
    }
    ls=ls->next;
  }
  return n;
}
int CountNSelected(struct HeadObjList *lh,int player){
  /*
    Count the number of selected objects.
    returns:
    0 if no objects selected.
    1 one object selected.
    2 more than one object selected.
  */

  struct ObjList *ls;
  int n=0;
  
  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->player!=player || ls->obj->type!=SHIP){ls=ls->next;continue;}
    if(ls->obj->selected==TRUE){
      n++;
      if(n>1)return(2);
    }
    ls=ls->next;
  }
  return(n);
}


Object *FirstSelected(struct HeadObjList *lh,int player){
 /*
    printf the selected ships
    return the first selected ship
  */

  struct ObjList *ls;
  
  if(lh==NULL)return(0);
  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->selected==TRUE && ls->obj->player==player){
      return(ls->obj);
    }
    ls=ls->next;
  }
  return(NULL);
}

float Distance2(Object *obj1,Object *obj2){
  /*
    Not USED
    version 0.2
    Calc the distance between 2 objects.
    Returns:
    distance pow 2

  */

  float x1,x2,y1,y2,rx,ry,r2;

  if(obj1==NULL)return(-1);
  if(obj2==NULL)return(-1);

  if(obj1->habitat==H_SHIP)obj1=obj1->in;
  if(obj2->habitat==H_SHIP)obj2=obj2->in;


  if(obj1->in==obj2->in){
    x1=obj1->x;
    y1=obj1->y;
    x2=obj2->x;
    y2=obj2->y;
  }
  else{
    switch(obj1->habitat){
    case H_PLANET:
      x1=obj1->in->x;
      y1=obj1->in->y;
      break;
    case H_SPACE:
      x1=obj1->x;
      y1=obj1->y;
      break;
    default:
      fprintf(stderr,"ERROR in AreEnemy() habitat unknown\n");
      exit(-1);
    }
    
    switch(obj2->habitat){
    case H_PLANET:
      x2=obj2->in->x;
      y2=obj2->in->y;
      break;
    case H_SPACE:
      x2=obj2->x;
      y2=obj2->y;
      break;
    default:
      fprintf(stderr,"ERROR in AreEnemy() habitat unknown\n");
      exit(-1);
    }
  }

  rx=x1 - x2;
  ry=y1 - y2;
  r2=rx*rx+ry*ry;
  return(r2);  
}

int GameOver(struct HeadObjList *lhead,struct Player *players,int player){
    /* 
       Conditions for GAME OVER :
       -no ships or (only pilots and gold less than 1100)
    */
  int n=0;  
  n=CountObjs(lhead,player,SHIP,-1);
  if(n==0)return(1);

  if(players[player].gold<GetPrice(NULL,FIGHTER,ENGINE4,CANNON4)){
    if(n==CountObjs(lhead,player,SHIP,PILOT)){ /* there are only pilots */
      return(1);
    }
  }
  return(0);
}

/*****************************************************************/
/*  Obj Tree. Verlet lists. NOT USED  */
/*****************************************************************/

struct ObjTree *Add2ObjTree(struct ObjTree *head,Object *obj){
  /*
    Add the object obj to the tree struct head
    returns :
    a pointer to the first element of the tree.
  */
  
  struct ObjTree *new,*parent;
  if(obj==NULL){
    return(NULL);
  }

  if(IsInObjTree(head,obj->id)){
    printf("obj %d already added\n",obj->id);
    printf("Printing objs tree...\n"); 
    PrintObjTree (treeobjs);
    printf("...done\n"); 

    return(head);
  }  


  if(head==NULL){
    /* first element */
    
    head=malloc(sizeof(struct ObjTree));
    MemUsed(MADD,+sizeof(struct ObjTree));
    if(head==NULL){
      fprintf(stderr,"ERROR in malloc Add2ObjTree()\n");
      exit(-1);
    }
    head->next=NULL;
    head->in=NULL;
    head->obj=obj;
    printf("first add: %d ",obj->id);
    return(head);
  }

  if(obj->in!=NULL){
    /* look for the parent */
    parent=Look4ObjTree(head,obj->in);
    if(parent==NULL){
      printf("Error: Add2objTree(): container: %d doesn't exist. id: %d not added to tree\n",obj->in->id,obj->id);
      return(head);
    }
    new=malloc(sizeof(struct ObjTree));
    MemUsed(MADD,+sizeof(struct ObjTree));

    if(new==NULL){
      fprintf(stderr,"ERROR in malloc Add2ObjTree()\n");
      exit(-1);
    }
    new->next=parent->in;
    new->in=NULL;
    new->obj=obj;
    parent->in=new;
    printf("Added: %d in %d\n",obj->id,parent->obj->id);
    return(head);
  }
  
  /* add to the beginning of the list */
  parent=head;
  new=malloc(sizeof(struct ObjTree));
  MemUsed(MADD,+sizeof(struct ObjTree));
  if(new==NULL){
    fprintf(stderr,"ERROR in malloc Add2ObjTree()\n");
    exit(-1);
  }
  new->next=parent;
  new->in=NULL;
  new->obj=obj;
  printf("Added: %d ",obj->id);
  return(new);
  
}

void DestroyTree(struct ObjTree *head){
   
  if(head==NULL)return;

  DestroyTree(head->next);
  head->next=NULL;
  DestroyTree(head->in);
  head->in=NULL;
  printf("deleting: %p\n",head);
  //  DestroyObj(head->obj);
  free(head);
  head=NULL;
  MemUsed(MADD,-sizeof(struct ObjTree));

}

struct ObjTree *DelObjTree(struct ObjTree *head,Object *obj){
  /*
    Del from the tree struct head the item that contain 
    the object obj
    returns:
    a pointer to the head tree
   */

  struct ObjTree *tmp;

  if(head==NULL)return(NULL);

  if(head->obj==obj){
    printf("\tDelObjTree: found: %d\n",head->obj->id);
    tmp=head;
    DestroyTree(head->in);
    head->obj->in=NULL;

    free(head);
    head=NULL;
    MemUsed(MADD,-sizeof(struct ObjTree));
    return(tmp->next);
  }

  if(head->next!=NULL)  printf("\tDelObjTree: check: %d\n",head->next->obj->id);
  tmp=head->next;
  head->next=DelObjTree(head->next,obj);


  if(head->in!=NULL)  printf("\tDelObjTree: check: %d\n",head->in->obj->id);
  head->in=DelObjTree(head->in,obj);


  return(head);
}
void PrintObjTree (struct ObjTree *head){
  /*
    version 01
    Prints the tree struct pointed by head
   */
  static int level=0;


  if(head==NULL)return;
  
  if(head->obj!=NULL){
    printf("l %d:  %d ",level,head->obj->id);
    if(head->obj->in!=NULL)printf("in %d\n",head->obj->in->id);
    else printf("\n");
  }

  PrintObjTree(head->next);
  level++;
  PrintObjTree(head->in);
  level--;

  
}

int IsInObjTree(struct ObjTree *head,int id){
  /*
    returns 1 if the object with id id belongs
    to the ObjTree head.
    return 0 if not is in the list.
   */
  int a=0;
  if(head==NULL)return(0);

  if(id==head->obj->id)return(1);

  a=(IsInObjTree(head->next,id));
  if(a==1)return(1);

  return(IsInObjTree(head->in,id));
  //  return(a);
}

struct ObjTree *Look4ObjTree(struct ObjTree *head,Object *obj){
  /*
    search the object obj in the tree struct head
    returns:
    the item found
    NULL if is not found.
   */
  struct ObjTree *ret=NULL;
  
  if(head==NULL)return(NULL);
  if(obj==NULL)return(NULL);
  //  printf("Look4 %d\n",obj->id);
  if(obj==head->obj)return(head);

  
  //  if(head->next!=NULL){
  ret=(Look4ObjTree(head->next,obj));
    //  }
  
  if(ret!=NULL)return(ret);
  //  if(head->in!=NULL){
  return(Look4ObjTree(head->in,obj));
  //  }
  //  return(ret);
}


struct VerletList *CreateVerletList(struct HeadObjList hol){
  /*
    Create a Verlet list of nearest objects from the obj. list hol.
    returns:
    a pointer to the created Verlet list.
   */

  struct ObjList *ls1,*ls2,*nols;
  Object *obj1,*obj2;
  float r2,rx,ry;
  float RCUT=1000000000;

  struct VerletList *vlh,*vl;

  vlh=malloc(sizeof(struct VerletList));
  MemUsed(MADD,+sizeof(struct VerletList));
  if(vlh==NULL){
    fprintf(stderr,"ERROR in malloc CreateVerletList()\n");
    exit(-1);
  }
  vl=vlh;
  vlh->obj=NULL;
  vlh->nextobj=NULL;
  vlh->next=NULL;

  ls1=hol.next;
  while(ls1!=NULL){ 
    obj1=ls1->obj;

    if(vlh->obj==NULL){ /* first element*/
      vlh->obj=obj1;
      printf("%d ",obj1->id);
    }
    else{/*next elements*/
      vl=malloc(sizeof(struct VerletList));
      MemUsed(MADD,+sizeof(struct VerletList));

      if(vl==NULL){
	fprintf(stderr,"ERROR in malloc CreateVerletList()\n");
	exit(-1);
      }
      
      vl->obj=obj1;
      vl->nextobj=NULL;
      vl->next=vlh->next;
      vlh->next=vl;
      printf("%d ",obj1->id);
    }
    
    ls2=ls1->next;
    while(ls2!=NULL){ 
      obj2=ls2->obj;

      rx=obj2->x - obj1->x;
      ry=obj2->y - obj1->y;
      r2=rx*rx+ry*ry;
      if(r2<RCUT){
	/* Adding to verlet list */
	if(vl->nextobj==NULL){
	  /* is the first */
	  vl->nextobj=malloc(sizeof(struct ObjList));
	  MemUsed(MADD,+sizeof(struct ObjList));
	  if(vl->nextobj==NULL){
	    fprintf(stderr,"ERROR in malloc CreateVerletList()\n");
	    exit(-1);
	  }
	  vl->nextobj->obj=obj2;
	  vl->nextobj->next=NULL;
	  printf("%d ",obj2->id);
	}
	else{
	  /* adding at the beginning */
	  nols=malloc(sizeof(struct ObjList));
	  MemUsed(MADD,+sizeof(struct ObjList));
	  if(nols==NULL){
	    fprintf(stderr,"ERROR in malloc CreateVerletList()\n");
	    exit(-1);
	  }
	  nols->obj=obj2;
	  nols->next=vl->nextobj;
	  vl->nextobj=nols;
	  printf("%d ",obj2->id);
	}
      }
      
      ls2=ls2->next;
    }
    printf("\n");
    ls1=ls1->next;
  }
  return(vlh);
}


void PrintVerletList(struct VerletList *hvl){
  /*
    print the Verlet list
   */
  struct ObjList *ls;
  struct VerletList *vl;
  

  vl=hvl;
  while(vl->next!=NULL){
    ls=vl->nextobj;
    printf("%d: ",vl->obj->id);
    while(ls!=NULL){
      printf("%d ",ls->obj->id);
      ls=ls->next;
    }
    printf("\n");
    vl=vl->next;
  }
}
void DestroyVerletList(struct VerletList *hvl){
  /*
    free memory of Verlet List
   */

  struct ObjList *ls,*ls0;
  struct VerletList *vl,*vl0;
  
  /* deleting the asociated lists, near neighbors*/
  vl=hvl;
  while(vl->next!=NULL){
    ls=vl->nextobj;

    while(ls!=NULL){
      ls0=ls;
      vl->nextobj=vl->nextobj->next;
      ls=vl->nextobj;
      free(ls0);
      ls0=NULL;
      MemUsed(MADD,-sizeof(struct ObjList));
    }
    vl=vl->next;
  }
  /* Deleting the verlet list */
  vl=hvl;
  while(vl->next!=NULL){
    vl0=vl->next;
    hvl->next=hvl->next->next;
    free(vl0);
    vl0=NULL;
    MemUsed(MADD,-sizeof(struct VerletList));
    vl=hvl;
  }
  free(hvl);
  hvl=NULL;
  MemUsed(MADD,-sizeof(struct VerletList));
}
