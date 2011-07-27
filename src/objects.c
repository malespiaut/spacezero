 /*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyright(C) 2006-2011  M.Revenga
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
extern struct TextMessageList listheadtext;
extern int actual_player,actual_player0;
extern int g_memused;
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
  obj=malloc(sizeof(Object));
  g_memused+=sizeof(Object);
  if(obj==NULL){
    fprintf(stderr,"ERROR in malloc NewObj()\n");
    exit(-1);
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
      obj->modified=SENDOBJNOTSEND;/* dont send explosion */
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
      obj->radio=2;
      break;
    case LASER: /*  shot4 */
      obj->state=1;
      obj->gas_max=0;
      obj->radio=50;
      break;
    case EXPLOSION:
      obj->durable=TRUE;
      obj->life=100;
      obj->damage=obj->parent->damage/16;
      obj->parent=NULL;
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
      obj->radio=9;
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
    /*     obj->planet->gold=2000+8000*(Random(-1)); */
    obj->planet->gold=(int)((MINPLANETGOLD+((MAXPLANETGOLD-MINPLANETGOLD)*
					    Random(-1))*(float)obj->mass/MAXPLANETMASS)*RESOURCEFACTOR);
    obj->planet->reggold=0.035*((float)obj->mass/MAXPLANETMASS)+0.02+0.02*(Random(-1));
    obj->planet->A=0;
    obj->planet->B=0;
    //    printf("reg:%.4f masss: %d\n",obj->planet->reggold,obj->mass);
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
    g_memused+=sizeof(Data);
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
  g_memused+=sizeof(struct ObjList);
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
    weapon->max_n=5;
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
    weapon->projectile.damage=100*DAMAGEFACTOR;
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
    eng->v_max=40;/* VELMAX; 15; */
    eng->ang_a=0.0004; /*  missile */
    eng->ang_a_max=.2;
    eng->ang_v_max=0.3;
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



Object *RemoveDeadObjs(struct HeadObjList *lhobjs , Object *cv0){
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
  int sw=0;
  int swx=0;
  int gnet;

  ret=cv0;
  gnet=GameParametres(GET,GNET,0);

  ls=lhobjs->next;

  while(ls!=NULL){
    freels=NULL;
    sw=0;
    swx=0;
    if(gnet==TRUE){
      if(ls->obj->modified==SENDOBJDEAD){sw=1;}
    }
    else{
      if(ls->obj->state<=0){sw=1;}
    }

    if(sw){

      freels=ls;

      /* explosion */
      switch(ls->obj->type){
      case ASTEROID:
	//	printf("REM ASTEROID: %d %d\n",ls->obj->id,ls->obj->modified);
	swx=1;
	break;
      case SHIP:
	swx=1;
	if(ls->obj->mode==SOLD)swx=0;

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
	Explosion(lhobjs,ls->obj);
	/* sound */
#if SOUND
	Play(ls->obj,EXPLOSION0,1);
#endif
      }
      /* --explosion */
    }

    if(freels!=NULL){

      if(freels->obj->type==SHIP && freels->obj->player == actual_player){
	snprintf(text,MAXTEXTLEN,"(%c %d) SHIP DESTROYED\n",Type(freels->obj),freels->obj->pid);
	Add2TextMessageList(&listheadtext,text,freels->obj->id,freels->obj->player,0,100,0);
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

      /*      printf("[K:%d]\n",freels->obj->id); */
      if(freels->obj==cv0)ret=NULL;

      if(freels!=NULL)
	RemoveObj(lhobjs,freels->obj);
    }
    ls=ls->next;
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
      ls=ls0->next;
    }
    else{ /* its the first element */ 
      freels=ls;
      lhobjs->next=lhobjs->next->next;
      ls=lhobjs->next;
      ls0=lhobjs->next;
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
    g_memused-=sizeof(Object);

    free(freels);
    g_memused-=sizeof(struct ObjList);
    freels=NULL;
    lhobjs->n--;
  } /*  if(ls->obj==obj2remove) */
  else{
    fprintf(stderr,"ERROR en removeobj()\n");
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
  g_memused+=sizeof(struct Planet);

  if(planet==NULL){
    fprintf(stderr,"ERROR in malloc NewPlanet()\n");
    exit(-1);
  }

  planet->segment=malloc(sizeof(Segment));
  g_memused+=sizeof(Segment);
  if(planet->segment==NULL){
    fprintf(stderr,"ERROR in malloc NewPlanet()2\n");
    exit(-1);
  }

  /* first segment */
  planet->segment->next=NULL;
  s=planet->segment;
  
  x=0;y=0;
  y=50*Random(-1);
  s->x0=0;
  s->y0=50*Random(-1);
  s->x1=20*Random(-1);
  s->y1=s->y0+40*(Random(-1))-20;
  if(s->y1<0)s->y1=0;
  s->type=TERRAIN;

  x=s->x1;
  y=s->y1;
  
  /* size and position of landing zone */
  l=FACTORLANDZONESIZE*(Random(-1))+LANDZONEMINSIZE;
  if(l>width-x)l=width-x;

  x0=(width-LANDZONEMINX-l)*(Random(-1))+LANDZONEMINX;

  if(x0<x)x0=x;


  while(x < width){
    
    ix =20*(Random(-1));
    iy =40*(Random(-1))-20;

    if(x>width-100){
      if(y-planet->segment->y0 > 100)factor=2;

      if(y>planet->segment->y0){
	iy =-factor*20*(Random(-1));
      }
      else{
	iy =20*(Random(-1));
      }
    }

    if(y+iy<5)continue;
    if(y+iy>MAXPLANETHEIGHT)continue;

    
    s->next=malloc(sizeof(Segment));
    g_memused+=sizeof(Segment);
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
  g_memused+=sizeof(Segment);
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



void Explosion(struct HeadObjList *lh,Object *obj){
  /*
    Create explosion objects.
   */  
  int i;
  float v,vx,vy;
  float a;
  Object *nobj;
  /*  return; */
  /*  printf("Explosion of object %d type %d stype:%d\n",obj->id,obj->type,obj->subtype); */
  for(i=0;i<16;i++){  
    a=2.*PI*(Random(-1));
    v=2.*VELMAX*(Random(-1)); 
    vx=2.*v*cos(a) + obj->vx;
    vy=2.*v*sin(a) + obj->vy;
    nobj=NewObj(lh,PROJECTILE,EXPLOSION,obj->x,obj->y,vx,vy,
		CANNON0,ENGINE0,obj->player,obj,obj->in);
    if(nobj!=NULL){
      Add2ObjList(lh,nobj);
      nobj->parent=NULL;
    }
    /*    printf("%d ",i); */
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

int CountShipsInPlanet(struct HeadObjList *lh,int planetid,int type,int stype,int max){
  /*
    Count the number of ships in planet planetid of type and subtype.
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
    
    if(ls->obj->habitat==H_PLANET){
      if(ls->obj->in->id==planetid)n++;
    }
    else{
      if(planetid==0)n++;
    }

    if(max>0){
      if(n>=max)return(max);
    }
    
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

  if(pid<=GameParametres(GET,GNPLANETS,0))return(SelectObj(lh,pid));

  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->player != player){ls=ls->next;continue;}
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


Object *SelectOneShip(struct HeadObjList *lh,Region reg,Object *cv,int ctrl){ 
  /*
    select the nearest obj to mouse pointer
    return the obj selected
  */

  struct ObjList *ls;
  Rectangle rect;
  int x,y;
  Point a;
  int sw1=0; /* first selected */
  Object *ret;
  float d2,d2min=10000;

  if(lh==NULL)return(NULL);
  if(cv==NULL)return(NULL);

  ret=cv;

  rect.x=reg.rect.x;
  rect.y=reg.rect.y;

  if(reg.habitat>0){ /* select a region inside a planet */
    a.x=rect.x;
    a.y=rect.y;
  }
  else{ /* select a region in map view */
    a.x=rect.x;
    a.y=rect.y;
  }

  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->player!=cv->player){ls=ls->next;continue;}
    if(ls->obj->type!=SHIP){ls=ls->next;continue;}

    if(ctrl==FALSE)ls->obj->selected=FALSE;

    if(reg.habitat>0){  /* select a region inside a planet */
      if(ls->obj->habitat==H_SPACE){ls=ls->next;continue;}
      if(reg.habitat!=ls->obj->in->id){ls=ls->next;continue;}
      d2=(rect.x-ls->obj->x)*(rect.x-ls->obj->x)+(rect.y-ls->obj->y)*(rect.y-ls->obj->y);
    }
    else{ /* select a region in map view */
      if(ls->obj->habitat==H_PLANET){
	ls=ls->next;continue;
      }
      else{
	x=ls->obj->x;
	y=ls->obj->y;
      }
      d2=(rect.x-ls->obj->x)*(rect.x-ls->obj->x)+(rect.y-ls->obj->y)*(rect.y-ls->obj->y);
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



Object *NearestObj(struct HeadObjList *lh,Object *obj,int type,int status,float *d2){
  /*
    Return a pointer to the nearest object of obj with status status.
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

    switch(obj->type){
    case SHIP:
      if(obj2->ttl<MINTTL){ls=ls->next;continue;}
      if(obj2->subtype==PILOT){
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

    switch(status){
    case PENEMY:
      if(obj2->player==player || obj2->player==0){ls=ls->next;continue;}
      if(players[obj2->player].team==players[player].team){ls=ls->next;continue;}

      break;
    case PINEXPLORE:
      if(obj2->player!=0){ls=ls->next;continue;}
      break;
    case PALLY:
      if(players[obj2->player].team!=players[player].team)
	{ls=ls->next;continue;}
      break;
    case PUNKNOWN:
      fprintf(stderr,"ERROR: status not implemented\n");
      exit(-1);
      break;
    default:
      fprintf(stderr,"ERROR: status unknown\n");
      exit(-1);
      break;
    }

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

int NearestObjs(struct HeadObjList *lh,Object *obj,int type,int status,int n,struct NearObject *objs){
  /*
    NOT USED. obsolete
    Add to *objs the n nearest objs to obj found with status status of type type.
    returns:
    the number of objets added to *objs.
  */

  struct ObjList *ls;
  float rx,ry,r2;
  float x0,y0,x1,y1;
  float r2min;
  int player;
  int sw1p,sw2p;
  int i,j;
  int m;
  int pheight=0;

  if(obj==NULL)return(0);
  /*   printf("obj.type=%d   ID:%d\n",obj->type,obj->id); */

  if(n>16)n=16;
  
  player=obj->player;
  r2min=-1;
  obj->dest_r2=-1;
  sw1p=sw2p=FALSE;


  for(i=0;i<n;i++){
    objs[i].obj=NULL;
    objs[i].d2=-1;
   }

  if(obj->habitat==H_PLANET){
    sw1p=TRUE;
    pheight=GameParametres(GET,GHEIGHT,0);
  }

  m=0;
  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->type!=type){ls=ls->next;continue;}
    if(ls->obj==obj){ls=ls->next;continue;}

    if(ls->obj->type==SHIP && ls->obj->ttl<MINTTL){ls=ls->next;continue;}

    if(type==PLANET){
      if(GameParametres(GET,GKPLANETS,0)==FALSE) 
	if(IsInIntList((players[player].kplanets),ls->obj->id)==0){
	  ls=ls->next;continue;
	}
    }


    if(sw1p && ls->obj->habitat==H_PLANET && 
       ls->obj->in==obj->in)sw2p=TRUE;

    if(status){
      switch(status){
      case PENEMY:
	if(ls->obj->player==player || ls->obj->player==0)
	  {ls=ls->next;continue;}
	if(players[ls->obj->player].team==players[player].team)
	  {ls=ls->next;continue;}
	break;
      case PINEXPLORE:
	if(ls->obj->player!=0){ls=ls->next;continue;}
	break;
      case PALLY:
	if(players[ls->obj->player].team!=players[player].team)
	  {ls=ls->next;continue;}
	break;
      default:
	break;
      }
    }

    if(sw1p==FALSE && ls->obj->habitat==H_PLANET){ls=ls->next;continue;}

    if(sw1p && ls->obj->habitat==H_PLANET &&
       obj->in!=ls->obj->in){ls=ls->next;continue;}

    if(sw2p){
      if(ls->obj->habitat!=H_PLANET){ls=ls->next;continue;}
    }

    x0=obj->x;
    y0=obj->y;
    
    x1=ls->obj->x;
    y1=ls->obj->y;

    if(sw1p && !sw2p){
      x0=obj->in->x;
      y0=obj->in->y;
    }

    rx=x0 - x1;
    ry=y0 - y1;
    r2=rx*rx+ry*ry;

    /*HERE add planet height */    

    for(i=0;i<n;i++){
      if(r2<objs[i].d2 || objs[i].d2==-1){
	for(j=0;j<n-i-1;j++){
	  objs[n-j-1].obj=objs[n-j-2].obj;
	  objs[n-j-1].d2=objs[n-j-2].d2;
	}
	m++;
	objs[i].obj=ls->obj;
	objs[i].d2=r2;
	break;
      }
    }
    ls=ls->next;
  }
  return(m);
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
  float r2min;
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
  r2min=-1;

  radar2=obj->radar*obj->radar;

  if(obj->habitat==H_PLANET){
    pheight2=GameParametres(GET,GHEIGHT,0);
    pheight2*=pheight2;
  }

  /* free space and planets */
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
	    fprintf(stderr,"ERROR 2 in Nerarestobjall()\n");
	    ls=ls->next;continue;
	  }
	}
	/* ignore pilots */
	if(obj2->subtype==PILOT){ls=ls->next;continue;}
	
	break;
      case PLANET:
	if(j!=0){ls=ls->next;continue;}

/* 	if(obj2->player!=obj->player){ /\* if are equal must be in list *\/ */
/* 	  if(!IsInIntList(players[player].kplanets,obj2->id)){ */
/* 	    ls=ls->next;continue; */
/* 	  } */
/* 	} */
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
    g_memused-=sizeof(struct ObjList);
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
    g_memused-=sizeof(Data);

  }
  free(obj);
  obj=NULL;
  g_memused-=sizeof(Object);
}

void DestroyPlanet(struct Planet *planet){
  Segment *s,*s0;

  s=planet->segment;
  while(s!=NULL){
    s0=s;
    s=s->next;
    free(s0);
    s0=NULL;
    g_memused-=sizeof(Segment);

  } 
  free(planet);
  planet=NULL;
  g_memused-=sizeof(struct Planet);

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
  g_memused+=sizeof(struct Planet);
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
  g_memused+=sizeof(Segment);
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
    g_memused+=sizeof(Segment);
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
			int source,int dest,int mid,int time,int value){
  /* 
     version 02. April 2 2011
     add the integer id to the list by priority given by value
     if is not already added.
     where:
     cad: the message.
     source: the object id
     dest: the destination player. -1 : for all players
     mid: is a message indentifier.
     time: the duration of the message in centiseconds.
     returns:
     0 if the message is added to the list
     1 if not, because is already added.
   */
  struct TextMessageList *list;
  struct TextMessageList *lh;

  /* Add text at the end of the list */

  lh=listhead;
  while(lh->next!=NULL){
    if(lh->next->info.source==source && lh->next->info.id==mid && lh->next->info.value==value){ /* already added */
      //      if(value)printf("message (%s)rejected\n",cad);
      return(1);
    }
    if(lh->next->info.value<value){ /* added before end */
      break;
    }
    lh=lh->next;
  }

  /*   printf("m addded:%s\n",cad); */
  list=malloc(sizeof(struct TextMessageList));
  g_memused+=sizeof(struct TextMessageList);
  if(list==NULL){
    fprintf(stderr,"ERROR in malloc Add2TextMessageList()\n");
    exit(-1);
  }
  list->info.source=source;
  list->info.dest=dest;
  list->info.id=mid;
  list->info.value=value;
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
  //  if(value)printf("message (%s) added\n",cad);
  listhead->info.n++;
  /*   gdk_beep(); */
  return (0);
}



int Add2TextList(struct TextList *listhead,char *cad,int color){
  /* 
     add the cad to the list
  */
  struct TextList *list;
  struct TextList *lh;

  /* Add text at the end of the list */
  lh=listhead;
  while(lh->next!=NULL){
    lh=lh->next;
  }

  list=malloc(sizeof(struct TextList));
  g_memused+=sizeof(struct TextList);

  if(list==NULL){
    fprintf(stderr,"ERROR in malloc Add2TextList()\n");
    exit(-1);
  }
  strncpy(list->text,cad,MAXTEXTLEN);
  list->color=color;
  list->next=NULL; 
  lh->next=list;
  return (0);
}

int DestroyTextList(struct TextList *head){
  /* 
     delete all the list
     return:
     the number of items deleted
  */
  struct TextList *lh0;
  struct TextList *lh;
  int n=0;
  lh=head->next;
  while(lh!=NULL){
    lh0=lh;
    lh=lh->next;
    free(lh0);
    lh0=NULL;
    g_memused-=sizeof(struct TextList);

    n++;
  }
  head->next=NULL;
  return (n);
}

int PrintTextList(struct TextList *head){
  struct TextList *lh;

  lh=head->next;
  while(lh!=NULL){
    printf("%s\n",lh->text);
    lh=lh->next;
  }
  return(0);
}
int CountTextList(struct TextList *head){
  /*
    returns:
    the number of texts of the the list head.
   */
  struct TextList *lh;
  int n=0;
  lh=head->next;
  while(lh!=NULL){
    n++;
    lh=lh->next;
  }
  return(n);
}
int PosTextList(struct TextList *head,int m){
  /*
    returns:
    the position of the text with color m
   */

  struct TextList *lh;
  int n=0;
  lh=head->next;
  while(lh!=NULL){
    if(lh->color==m)return(n);
    n++;
    lh=lh->next;
  }
  return(-1);
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
      fprintf(stderr,"ERROR in GetPrice() obj type:%d\n",obj->type);
      return(-1);
    }

    if(obj->type==SHIP && obj->subtype==PILOT){
      return(-1);
    }

    stype=obj->subtype;
    eng=obj->engine.type;
    weapon=obj->weapon0.type;
    level=obj->level;
  }

  if(stype<SHIP0 || stype>SHIPMAX){
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
  if(0&&obj->player!=obj->in->player){
    printf("Warning: Buyship() player: %d planet: %d\n",player.id,obj->in->player);
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
    price=GetPrice(NULL,SHIP1,ENGINE3,CANNON3);
    break;
  case FIGHTER:
    price=GetPrice(NULL,SHIP3,ENGINE4,CANNON4);
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
  
  if(hl==NULL)return(0);

  while(hl->next!=NULL){
    ls0=hl->next;
    hl->next=hl->next->next;
    ls0->obj=NULL;
    ls0->next=NULL;
    free(ls0);
    ls0=NULL;
    g_memused-=sizeof(struct ObjList);
    hl->n--;
    n++;
  }
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
  int id1,id2;
  int n=0;


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

    
    id1=ls1->obj->id;
    if(ls1->obj->type!=PLANET)id1=0;
    if(ls1->obj->habitat==H_PLANET)id1=ls1->obj->in->id;

    ls0=malloc(sizeof(struct ObjList));
    g_memused+=sizeof(struct ObjList);
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
    if(hlist2->next->obj->habitat==H_PLANET)id2=hlist2->next->obj->in->id;
    
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
      if(ls2->next->obj->habitat==H_PLANET)id2=ls2->next->obj->in->id;
      if(id1 < id2 )break;
    }

    ls0->next=ls2->next;
    ls2->next=ls0;

    /*    printf("%d ",ls0->obj->id); */
    hlist2->n++;
    n++;
  }
  /*  printf("\n"); */
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

  for(i=0;i<GameParametres(GET,GNPLANETS,0)+1;i++){
    if(hcontainer[i].next!=NULL){ 
      fprintf(stderr,"WARNING: CCL() not NULL\n"); 
    } 
    hcontainer[i].n=0;
  }

  nplanets=GameParametres(GET,GNPLANETS,0);
  proc=GetProc();

  ls=lh->next;
  while(ls!=NULL){
    
    if(ls->obj->state<=0 && proc==players[ls->obj->player].proc){
      ls=ls->next;continue;
    }

    switch(ls->obj->type){
    case PROJECTILE:
      if(ls->obj->subtype==EXPLOSION){ 
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
      if(ValueCell(cell,obj)==value0){
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
     Create a list with all the objects that can collide.
     returns the number of objects of the list.
   */
  struct ObjList *ls;
  int nplayers;
  int i;
  int proc;

  proc=GetProc();
  nplayers=GameParametres(GET,GNPLAYERS,0);

  for(i=0;i<nplayers+1;i++){
    if(hkplanets[i].next!=NULL){ 
      fprintf(stderr,"WARNING: CkpL() not NULL\n"); 
    } 
    hkplanets[i].n=0;
  }

  ls=lh->next;
  while(ls!=NULL){

    if(ls->obj->type!=PLANET){ls=ls->next;continue;}

    for(i=0;i<nplayers+2;i++){
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
      g_memused+=sizeof(struct ObjList);
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
	if(GetProc()==players[obj->player].proc){
	  SetModified(obj,SENDOBJALL);
	}
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
      //      if(obj->cost==0)obj->cost=0.01;

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
	if(obj->weapon1.rate>9)
	  obj->weapon1.rate--;
	if(obj->weapon1.max_n<14){ /* max 15 missiles */
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
	if(obj->level==1){
	  if(obj->weapon1.type==CANNON0){
	    NewWeapon(&obj->weapon1,CANNON8);
	  }
	}
	if(obj->level==2){
	  if(obj->weapon2.type==CANNON0){
	    NewWeapon(&obj->weapon2,CANNON9);
	  }
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


Object *MarkObjs(struct HeadObjList *lh,Region reg,Object *cv,int ctrl){
  /*
    Set the selected mark on in all ship inside the region reg.
    return a pointer to the first selected obj

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
	    ret=ls->obj;
	    sw1++;
	  }
	  ls->obj->selected=TRUE;
	  if(ls->obj==cv)ret=cv;
	  n++;
	}
      }
    }
    else{ /* select a region in map view */
      
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
	    ret=ls->obj;
	    sw1++;
	  }
	  ls->obj->selected=TRUE;
	  if(ls->obj==cv)ret=cv;
	  n++;
	}
      }
    }
    ls=ls->next;
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
     dont take in account obj
     returns:
     0 if the planet is totally empty
     1 if there are some ally landed
     2 if there are some enemy ship landed

  */

  struct ObjList *ls;
  if(obj==NULL)return(-1);
  if(planet==NULL)return(-1);

  ls=listheadobjs.next;
  while(ls!=NULL){
    if(ls->obj->in==planet){
      if(ls->obj->type==SHIP && ls->obj->subtype!=PILOT && ls->obj->mode==LANDED){
	if(ls->obj!=obj){
	  if(players[ls->obj->player].team!=players[obj->player].team){
	    return(2);
	  }
	  else{
	    return(1);
	  }
	}
      }
    }
    ls=ls->next;
  }
  //  printf("PLANET EMPTY\n");
  return(0);
}

int UpdateCell(struct HeadObjList *lh,int *cell){
  /* 
     version 02 080411
*/
  struct ObjList *ls;
  Object *obj;
  int n=0;
  int dx,dy,dxdy;
  int nx0,ny0,nx,ny;
  int nydx;
  int dx2,dy2;
  int i,j;
  int value;
  int gnet;

  dx=GameParametres(GET,GULX,0)/DL;
  dy=GameParametres(GET,GULY,0)/DL;
  dxdy=dx*dy;  
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


int ValueCell(int *cell,Object *obj){
  /*
    HERECOM
  */
  int x,y;
  int dx,dy;
  int nx,ny;
  int index;
  int gulx,guly;

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

  gulx=GameParametres(GET,GULX,0);
  guly=GameParametres(GET,GULY,0);
  
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
  case SHIP1: /* EXPLORER */
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
    obj->cost=0.01;
    break;
  case SHIP0: /* not used */
  case SHIP2: /* not used */
  case SHIP3: /*  FIGHTER */
    obj->gas_max=1000;
    obj->gas=obj->gas_max;
    obj->shield=0;
    obj->state=90;
    obj->mass=100;
    /*    obj->items=obj->items | ITSURVIVAL;*/
    obj->items=ITSURVIVAL;
    obj->cargo=10; /* TODO not implemented */
    obj->radio=10;
    obj->ai=1;
    obj->damage=25;
    obj->cost=0.01;

    /*       NewWeapon(&obj->weapon1,CANNON8); */
    /*       NewWeapon(&obj->weapon2,CANNON9); */
    break;
  case SHIP4: /*  cargo queen ship */
    obj->gas_max=2000;
    obj->gas=obj->gas_max;
    obj->shield=0.9;
    obj->state=90;
    obj->mass=400;
    obj->cargo=20;
    obj->radio=20;
    obj->ai=1;
    obj->damage=25;
    obj->cost=0.04;
    break;
    
  case SHIP5: /* SATELLITE: */
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
    obj->cost=0.005;
    break;
  case SHIP6: /* TOWER: */
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
    obj->cost=0;
      break;
  case SHIP7: /* PILOT */
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
    obj->cost=0.0;
    break;
  default:
    fprintf(stderr,"ERROR ShipProperties(): unknown subtype\n");
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
    obj->vx*=.75;
    obj->vy*=.75;
  }
  obj->subtype=PILOT;
  /*  obj->items=obj->items | ITPILOT;*/
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
  Explosion(&listheadobjs,obj);
#if SOUND
  if(obj->habitat==H_PLANET)
    Play(obj,EXPLOSION0,1);
#endif
  return(1);
}


int EjectPilots(struct HeadObjList *lh){
  /*
    Eject pilots
    Rescue the transported pilots of dead objs 
    only own pilots, not allied pilots. 
    returns:
    the number of pilots ejected.
   */

  struct ObjList *ls;
  Object *obj,*pilot;
  Segment *s;
  float r;
  int n=0;
  int proc;
  int ejectsw=0;

  proc=GetProc();
  ls=lh->next;
  while(ls!=NULL){

    if(ls->obj->type!=SHIP){ls=ls->next;continue;}

    switch(ls->obj->subtype){
    case FIGHTER:
      obj=ls->obj;

      if(proc==players[obj->player].proc){
	if(obj->state<=0 && obj->level>=0){
	  
	  /***** ship destroyed ****/

	  CreatePilot(obj);
	  if(obj->player==actual_player)printf("Ejecting pilot from ship %d\n",obj->pid);
	  DelAllOrder(obj);
	  if(GameParametres(GET,GNET,0)==TRUE){
	    SetModified(obj,SENDOBJALL);
	    /* check */
	    if(obj->modified!=SENDOBJALL){
	      if(obj->modified==SENDOBJKILL){
		obj->modified=SENDOBJALL;
	      }
	      else{
		printf("Warning:Ejectpilots():  modified:%d\n",obj->modified);
	      }
	    }
	  }
	}   
      }
      break;

    case PILOT:
      if(ls->obj->habitat==H_SHIP){
	if(proc==players[ls->obj->player].proc){
	  
	  ejectsw=0;	  
	  if(ls->obj->in->subtype==PILOT)ejectsw=1;

	  if(ls->obj->in->state<=0 && 
	     ls->obj->in->level>=0 && 
	     proc==players[ls->obj->in->player].proc)ejectsw=1; 
	  
	  if(players[ls->obj->in->player].proc!=proc && 
	     (ls->obj->in->modified==SENDOBJDEAD ||ls->obj->in->modified==SENDOBJKILL) && 
	     ls->obj->in->level>=0)ejectsw=1;
	     
	  if(ejectsw){ /* ejecting */

	    pilot=ls->obj;
	    obj=ls->obj->in;

	    pilot->in=obj->in;
	    pilot->planet=NULL;
	    pilot->habitat=obj->habitat;
	    pilot->mode=NAV;
	    pilot->x=obj->x;
	    pilot->y=obj->y;
	    pilot->vx=0.75*obj->vx+6*Random(-1)-3;
	    pilot->vy=0.75*obj->vy+6*Random(-1)-3;
	    if(obj->habitat==H_PLANET)pilot->vy+=10;
	    pilot->a=obj->a;
	    pilot->ai=0;
	    pilot->items=0;
	    if(pilot->player==actual_player)printf("Pilot %d ejected from ship %d\n",pilot->pid,obj->pid);
	    DelAllOrder(pilot);
	    if(GameParametres(GET,GNET,0)==TRUE){
	      SetModified(pilot,SENDOBJALL);
	      if(pilot->modified!=SENDOBJALL){
		printf("PILOT EJECT mod: %d\n",pilot->modified);
	      }
	    }
	    n++;
	  }
	  else{
	    /* when landing rescue pilots */
	    if(ls->obj->in->mode==LANDED){
	      pilot=ls->obj;
	      obj=ls->obj->in;
	      s=LandZone(obj->in->planet);
	      
	      if(proc==players[obj->player].proc){
		obj->items=obj->items & (~ITPILOT); 

		if(GameParametres(GET,GNET,0)==TRUE){
		  SetModified(obj,SENDOBJALL);
		}
	      }

	      if(s==NULL){
		fprintf(stderr,"ERROR EjectPilots(): Segment==NULL\n");
		exit(-1);
	      }
	      r=s->x1-s->x0-2*pilot->radio;
	      pilot->in=obj->in;
	      pilot->habitat=H_PLANET;
	      pilot->experience=0;
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
	      if(pilot->player==actual_player)printf("Pilot %d saved in planet %d\n",pilot->pid,pilot->in->id);
	      DelAllOrder(pilot);
	      if(GameParametres(GET,GNET,0)==TRUE){
		SetModified(pilot,SENDOBJALL);
	      }
	      n++;
	    }
	  }
	}
      }
      /* case pìlot */ 
      break;
    default:
      break;
    }
    ls=ls->next;
  }
  return(n);
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





/*****************************************************************/
/*  Obj Tree. Verlet lists. NOT USED  */


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
    g_memused+=sizeof(struct ObjTree);
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
      printf("Error: Add2objTree(): container: %d doesnt exist. id: %d not added to tree\n",obj->in->id,obj->id);
      return(head);
    }
    new=malloc(sizeof(struct ObjTree));
    g_memused+=sizeof(struct ObjTree);

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
  g_memused+=sizeof(struct ObjTree);
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
  g_memused-=sizeof(struct ObjTree);

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
    g_memused-=sizeof(struct ObjTree);
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
  g_memused+=sizeof(struct VerletList);
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
      g_memused+=sizeof(struct VerletList);

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
	  g_memused+=sizeof(struct ObjList);
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
	  g_memused+=sizeof(struct ObjList);
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
      g_memused-=sizeof(struct ObjList);
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
    g_memused-=sizeof(struct VerletList);
    vl=hvl;
  }
  free(hvl);
  hvl=NULL;
  g_memused-=sizeof(struct VerletList);
}

