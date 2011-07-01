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


#include <stdlib.h>
#include "general.h"
#include "objects.h"
#include "ai.h"
#include "sound.h"
#include "spacecomm.h"
#include "graphics.h"
#include "functions.h"
#include "sectors.h"

extern struct TextMessageList listheadtext;
extern Object *cv;     /* coordenates center */
extern struct Player *players;
extern struct CCDATA *ccdatap; 
extern struct HeadObjList *listheadcontainer; /* lists of objects that contain objects: free space and planets*/
extern struct HeadObjList *listheadkplanets;  /* lists of planets known by players */
extern int g_memused;

#if DEBUG
int debugwar=FALSE;
int debugshop=FALSE;
int debug=FALSE;
int debugai=FALSE;
int debugrisk=FALSE;
#endif

int MAXnf2a=15;
int MINnf2a=6;

void ai(struct HeadObjList *lhobjs,Object *obj,int act_player){
  /*
    main ai subroutine
    used by all ships controled by computer.
  */

  struct Order *mainord,*execord,order;
  Object *objt=NULL;
  Segment *segment=NULL;
  Object *ship_enemy;
  float a,b,ia;
  float d2_enemy;
  float vx,vy,v2;
  char text[MAXTEXTLEN];
  float alcance2;
  struct NearObject nobjs[4];
  int i;
  float d2min;
  int time;
  int danger=0;
  int control=COMPUTER;
  int mainordid;

  if(obj==NULL){
    fprintf(stderr,"ERROR 1 in ai()\n");
    exit(-1); 
  }

  if(obj->ai==0)return;

  d2_enemy=-1;
  mainord=execord=NULL;

  ship_enemy=NULL;

  vx=obj->vx;
  vy=obj->vy;
  v2=vx*vx+vy*vy;
  a=obj->a;
  b=atan2(vy,vx);  /* velocity angle */

  time=GetTime();

  /*Getting info */

  control=players[obj->player].control;

#if DEBUG
  if(debugai&&obj==cv){printf("time:%d %d\n",obj->cdata->td2[0],obj->cdata->a);}
#endif

  if(obj->cdata->obj[0]!=NULL || (time - obj->cdata->td2[0]) > 10){

    for(i=0;i<4;i++){
      nobjs[i].obj=NULL;
      nobjs[i].d2=-1;
    }

    NearestObjAll(listheadcontainer,obj,nobjs); // HERE double loop

    d2min=obj->radar*obj->radar;
    
    if(nobjs[0].obj!=NULL){
      if(nobjs[0].obj->type==SHIP ){ /* take off to intercept */
	d2min*=2;
      }

      if(nobjs[0].obj->type==ASTEROID && control==COMPUTER){ //HERE send to asteroids from ControlCenter()
	d2min=25*obj->radar*obj->radar;
      }
      
      if(nobjs[0].d2>d2min){ /* nearest ship must be in radar range */
	nobjs[0].obj=NULL;
	nobjs[0].d2=-1;
      }
    }

    for(i=0;i<4;i++){
      obj->cdata->obj[i]=nobjs[i].obj;
      obj->cdata->d2[i]=nobjs[i].d2;
      obj->cdata->td2[i]=time;
    }
  }

  /*--Getting info */

  ship_enemy=obj->cdata->obj[0];
  d2_enemy=obj->cdata->d2[0];

  if(ship_enemy!=NULL  && obj->player==act_player && cv!=obj){
    int value=0; 
    objt=obj;

    switch(ship_enemy->type){
    case SHIP:
      if(obj->in!=NULL && (players[obj->player].team == players[obj->in->player].team)){
	objt=obj->in;
	if(ship_enemy->habitat==H_SPACE){
	  snprintf(text,MAXTEXTLEN,"(P %d) ENEMIES NEAR",objt->pid);
	  value=1;
	}
	else{
	  snprintf(text,MAXTEXTLEN,"Planet %d under attack!!",objt->pid);
	  value=3;
	}
      }
      else{
	snprintf(text,MAXTEXTLEN,"(%c %d) ENEMIES NEAR",Type(obj),obj->pid);
	value=1;
      }
      break;
    case ASTEROID:
      snprintf(text,MAXTEXTLEN,"(%c %d) ASTEROIDS NEAR",Type(obj),obj->pid);
      break;
    default:
      break;
    }
    Add2TextMessageList(&listheadtext,text,objt->id,objt->player,0,100,value);
  }



  /******************** 
     select the order 
  ********************/


  mainord=ReadOrder(NULL,obj,MAINORD);

  if(mainord!=NULL){
    if(mainord->id==RETREAT && obj->mode==LANDED){
      ReadOrder(NULL,obj,MAXPRIORITY); /* deleting the order */
      order.priority=1;
      order.id=GOTO;
      order.time=4;
      order.g_time=time;
      
      order.a=obj->in->x; 
      order.b=obj->in->y; 
      order.c=obj->in->id; 
      order.d=obj->in->type; 
      order.e=obj->in->id; 
      order.f=order.h=0; 
      order.g=obj->in->mass;
      DelAllOrder(obj);
      AddOrder(obj,&order);   /* adding go to the planet */
      mainord=ReadOrder(NULL,obj,MAINORD);
    }
  }
  
  mainordid=mainord!=NULL?mainord->id:-1;
  danger=Risk(lhobjs,obj,mainordid,&order.id); /* Checking for danger */

  /* if(cv==obj)printf("risk: %d\n",danger); */

#if DEBUG
  if(debugai && cv==obj){
    printf("DANGER:%d order: %d\n",danger,order.id); 
    if(obj->dest!=NULL)
      printf("(%d) CHOICE: %d %d\n",obj->id,order.id,obj->dest->id);
    else
      printf("(%d) CHOICE: %d NULL\n",obj->id,order.id);

    printf("actorder:%d\n",obj->actorder.id);
  }
#endif

  if(obj->actorder.id==-1 && obj->norder<10){ /*  */

    order.priority=20;
    order.time=0;
    order.g_time=time;
    
    order.a=order.b=order.c=order.d=0;
    order.e=order.f=order.g=order.h=0;
    
    if(danger==0){ /* Execute mainorder */

      if(mainord!=NULL){
	/*order.id=ord->id; */
	order.id=mainord->id;

	if(mainord->id==EXPLORE && mainord->h==1){/* select sector */
	  int a,b;

	  SelectSector(&(players[obj->player].ksectors),obj,&a,&b);
	  mainord->a=a*SECTORSIZE+SECTORSIZE/2;
	  mainord->b=b*SECTORSIZE+SECTORSIZE/2;
	  mainord->h=0;
	}

	switch(mainord->id){
	case RETREAT:
	case GOTO:
	case EXPLORE:
	  objt=NULL;
	  if(mainord->c!=-1){ /* dest is an object */
	    objt=Coordinates(lhobjs,mainord->c,&mainord->a,&mainord->b);
	    if(objt==NULL){ /* error or dest is killed */
	      DelAllOrder(obj);
	      order.id=NOTHING;
	    }
	  }

	  if(objt!=NULL){

/* 	ord.a:  x coordinates of the objetive. */
/*      ord.b:  y coordinates of the objetive. */
/*      ord.c: id of the objetive. -1 if is a universe point. */
/*      ord.d: objetive type. */
/*      ord.e:  objetive id. */
/*      ord.g: mass of the objetive. */

	    order.c=mainord->c;
	    order.d=mainord->d;
	    order.e=objt->id;
	    order.f=0;
	    order.g=objt->mass;
	    if(objt->type==PLANET){
	      if(obj->habitat==H_PLANET){
		if(obj->in->id==objt->id){
		  if(obj->player==act_player && mainord->h==0){
		    /*		    fprintf(stdout,"(%c %d) HE LLEGADO a %d\n",Type(obj),obj->id,obj->in->id); */
		    snprintf(text,MAXTEXTLEN,"(%c %d) ARRIVED TO %d",Type(obj),obj->pid,obj->in->id);
		    Add2TextMessageList(&listheadtext,text,obj->id,obj->player,0,100,0);
		  }
		  mainord->h=1;
		  order.id=LAND;
		  /*
		    if(players[obj->in->player].team!=players[obj->player].team){ 
		    if(players[obj->in->player].team!=0){
		    order.id=ATTACK; 
		    }
		    } 
		  */
		}
		else{
		  order.id=TAKEOFF;
		}
	      }
	    }
	    if(objt->type==SHIP){
	      if(obj->habitat==H_PLANET){
		if(objt->habitat==H_PLANET){
		  if(obj->in==objt->in){
		    order.id=LAND;
		  }
		  else{
		    order.id=TAKEOFF;
		  }
		}
		else{
		  order.id=TAKEOFF;
		}
	      }
	    }
	  }
	  if(mainord->c==-1){ /* destination is a sector */ 
	    if(fabs(obj->x-mainord->a)<400 && fabs(obj->y-mainord->b)<400){
	      if(obj->player==act_player && mainord->h==0 && mainord->id!=EXPLORE){
		snprintf(text,MAXTEXTLEN,"(%c %d) ARRIVED TO %d %d",
			 Type(obj),obj->pid,(int)(mainord->a/SECTORSIZE)-(mainord->a<0),(int)(mainord->b/SECTORSIZE)-(mainord->b<0));
		Add2TextMessageList(&listheadtext,text,obj->id,obj->player,0,100,0);
	      }
	      mainord->h=1;
	      order.id=STOP;
	    }
	    if(obj->habitat==H_PLANET){
	      order.id=TAKEOFF;
	      if(obj->gas<.20*obj->gas_max){
		if(obj->mode!=LANDED && obj->in->player==obj->player)
		  order.id=LAND;  
	      }
	    }
	  }
	  break;
	case TAKEOFF:
	  order.id=TAKEOFF;
	  if(obj->habitat!=H_PLANET){
	    DelAllOrder(obj);
	    order.id=NOTHING;
	  }
	  break;
	case STOP:
	  /* if ship is in a planet and is not LANDED, change order to GOTO this planet */
	  if(obj->in!=NULL && obj->mode!=LANDED){ 
	    ReadOrder(NULL,obj,MAXPRIORITY); /* deleting the order */
	    
	    order.priority=1;
	    order.id=GOTO;
	    order.time=0;
	    order.g_time=time;
	    
	    order.a=obj->in->x; 
	    order.b=obj->in->y; 
	    order.c=obj->in->id; 
	    order.d=obj->in->type; 
	    order.e=obj->in->id; 
	    order.f=order.h=0; 
	    order.g=obj->in->mass;
	    DelAllOrder(obj);
	    AddOrder(obj,&order);   /* adding go to the planet */
	  } 
	  break;
	default:
	  break;
	}

      } /*  if(mainord!=NULL) */
      else{
#if DEBUG
	if(debugai&&cv==obj)printf("macro=NULL actorder : %d\n",obj->actorder.id);
#endif

	if(obj->mode==NAV){
	if(obj->habitat==H_SPACE){

	/* 
	   all is ok, is in space , there no orders=> goto nearest empty or ally planet
	 */
	  obj->dest=obj->cdata->obj[3]; /* planet_ally */
	  if(obj->dest==NULL){
	    obj->dest=obj->cdata->obj[2];/* planet_inexplore;*/
	  }
	  if(obj->dest==NULL){
	    obj->dest=obj->cdata->obj[1]; /* planet_enemy; */
	  }
	}

	if(obj->habitat==H_PLANET){
	  if(obj->cdata->obj[0]==NULL){ /* there are no enemies */
	    if(obj->dest==NULL){
	      obj->dest=obj->in; /* if there no enemies goto here; */
	    } 
	  }
	}
	if(obj->dest!=NULL){
	  order.id=GOTO;
	  order.priority=1; 
	  order.time=0; 
	  order.g_time=time; 
	  order.a=obj->dest->x;  
	  order.b=obj->dest->y;  
	  order.c=obj->dest->id;  
	  order.d=obj->dest->type;  
	  order.e=obj->dest->pid;  
	  order.f=order.g=order.h=0;  
	  order.g=obj->dest->mass;
	  DelAllOrder(obj); 
	  AddOrder(obj,&order);   /* adding go to the nearest planet */
	}
	else{
#if DEBUG
	  if(debugai){
	    fprintf(stderr,"ai(): Debug Warning: id: %d pid:%d dest=NULL player: %d\n",
		    obj->id,obj->pid,obj->player);
	  }
#endif
	}
	

	}
      }
    } /*  if(danger==0) */

    if(0&&order.id==GOTO){
      if(obj->mode==LANDED){
	order.id=TAKEOFF;	
      }
    }
#if DEBUG
    if(cv==obj && debugai)printf("ADD:(%d) %d\n",obj->id,order.id);
#endif


    /****************** 
     * Add the order 
     ******************/

    switch(order.id){
      
    case BRAKE:
      break;
    case STOP:
      order.priority=20;
      order.id=STOP;
      order.time=8;
      AddOrder(obj,&order);
      break;

    case ATTACK:
      order.time=20;
      order.a=0;
      order.b=0;
      if(obj->dest!=NULL){
	float rx,ry;
	rx=obj->dest->x - obj->x;
	ry=obj->dest->y - obj->y;
	b=atan2(ry,rx);
	
	ia=obj->a-b; /* angle between objetive and ship direction */
	if(ia > PI)ia-=2*PI;
	if(ia < -PI)ia+=2*PI;
	if(ia>0)order.a=1;
	if(ia<0)order.a=-1;
	if(obj->habitat==H_PLANET){
	  order.b=HigherPoint(obj->in->planet);
	}
      }

      AddOrder(obj,&order);
      break;

    case LAND:
      if(obj->mode!=LANDED){
	order.id=LAND;
	order.time=6;
	segment=LandZone(obj->in->planet);
	
	order.a=segment->x0;                  /* begin of landzone */
	order.b=segment->x1 - segment->x0;    /* size of land zone */
	order.c=segment->y0;                  /* high of land zone */
	order.d=HigherPoint(obj->in->planet); /* higher point of planet */
	order.e=LXFACTOR/2;
	order.f=order.d+50+obj->in->mass/10000; /* work high */

      }
      else{
	order.id=NOTHING;
	order.time=10;
      }
      
      order.priority=20;
      order.g_time=time;
      AddOrder(obj,&order);
      break;
    case TAKEOFF:

      b=PI/2;

      /* MOVE */
      order.priority=20;
      order.id=TURNACCEL;
      order.time=20;
      
      order.a=b;            /* objetive */
      order.b=0.2;          /* condition, precission */ 
      order.c=0;

      if(fabs(obj->a-b)>0.4){
	order.id=TURN;
	order.b=.08;
	order.time=1;
      }

      AddOrder(obj,&order);
      
      break;
    case NOTHING:
      obj->ang_a=0;
      obj->accel=0;
      order.time=20;
      
      AddOrder(obj,&order);
      break;

    case EXPLORE:
    case GOTO:
    case RETREAT:
#if DEBUG
      if(cv==obj && debugai)printf("(%d): GOTO  time:%d\n",obj->id,time);
#endif
      if(obj->habitat==H_PLANET){

#if DEBUG
	if(debugai && obj->dest!=NULL){
	  printf("(%c %d): ARRIVED TO %d\n",Type(obj),obj->id,obj->dest->id);
	}
#endif
	/* 	DelAllOrder(obj); */
	return;
      }
#if DEBUG
      if(cv==obj && debugai)printf("GOTO ord:%p\n",mainord);
#endif
      if(danger){
	if(obj->dest==NULL){
#if DEBUG
	  if(debugai){
	    printf("(%d) %d Error ai() GOTO\n",obj->id,obj->pid);
	  }
#endif
	  /* 	  ExecSave("dat/save"); */
	  return;
	}
	order.a=obj->dest->x;
	order.b=obj->dest->y;
      }
      else{
	if(mainord!=NULL){
	  order.a=mainord->a;
	  order.b=mainord->b;
	}
      }

      order.priority=20;
      order.id=GOTO;
      order.time=10;
      
      AddOrder(obj,&order);
      break;

    default:
      fprintf(stderr,"ERROR 2 in ai()\n");
      exit(-1);
      break;
    }
  } /*   if(obj->actorder==NULL && obj->norder<10) */
  

  /******************* 
   * execute the order 
   *******************/
  
  /* reads the next order */
  
  if(obj->actorder.id==-1){
    ReadOrder(&(obj->actorder),obj,MAXPRIORITY);
  }
  
  if(obj->actorder.id!=-1){
    execord=&(obj->actorder);

#if DEBUG
    if(debugai && cv==obj)printf("Executing ordid:%d time: %d\n",execord->id,time);
#endif    

    if(execord->time>=0){
      if(cv==obj)printf("order: %d\n",execord->id);      
      switch(execord->id){
      case FIRE:
	if(!obj->weapon->cont1 && obj->gas > obj->weapon->projectile.gascost && 
	   d2_enemy<1000000 && d2_enemy>0){
	  if(obj->dest==NULL){ /*  obsolete order */
	    execord->time=0;
	    break;
	  }
	  ia=obj->a - execord->a;
	  if(ia > PI)ia-=2*PI;
	  if(ia < -PI)ia+=2*PI;

	  if( fabs(ia)<.05){
	    obj->weapon=ChooseWeapon(obj);
	    alcance2=.5*obj->weapon->projectile.max_vel*obj->weapon->projectile.life;
	    alcance2*=alcance2;
	    if(alcance2>d2_enemy){
	      if(FireCannon(lhobjs,obj,NULL)==0){
		Play(obj,FIRE0,1);
	      }
	      if(GameParametres(GET,GNET,0)==TRUE){
		if(GetProc()==players[obj->player].proc){
		  obj->ttl=0;
		  SetModified(obj,SENDOBJMOD0);
		}
	      }
	    }
	  }
	}
	break;
      case NOTHING:

#if DEBUG
	if(debugai && cv==obj)printf("ai(): NOTHING %d\n",execord->time);
#endif

	if(obj->mode==LANDED && fabs(obj->a-PI/2)>0.05){
	  execord->priority=20;
	  execord->id=TURN;
	  execord->a=PI/2;            /* objetive */
	  execord->b=.04;
	  execord->c=0;	
	  execord->time=10;
	  ExecTurn(obj,PI/2);
	}
	else{
	  obj->ang_v=0;
	  obj->ang_a=0;
	  obj->accel=0;
	}
	break;
      case TURNACCEL:
#if DEBUG
	if(debugai && cv==obj)printf("TURNACCEL %d\n",execord->time);
#endif
	if(obj->gas>0){
	  obj->accel+=obj->engine.a;
	  if(obj->accel > obj->engine.a_max)
	    obj->accel=obj->engine.a_max;
	  /* if max vel is reached, accel=0 */
	  ia=b-execord->a;
	  if(ia > PI)ia-=2*PI;
	  else
	    if(ia < -PI)ia+=2*PI;
	  if(fabs(ia) < execord->b){
	    if(v2>0.8*obj->engine.v2_max ){ 
	      obj->accel=0; 
#if DEBUG
	      if(debugai && cv==obj)printf("v2 mayor:%f\n",v2);
#endif
	    }
	    else{
#if DEBUG
	      if(debugai && cv==obj)printf("v2:%f\n",v2);
#endif
	    }
#if DEBUG
	    if(debugai && cv==obj)printf("iang (1):%f  %f\n",fabs(b-execord->a),execord->b);
#endif
	  }
	  else{
#if DEBUG
	    if(debugai && cv==obj)printf("iang (2):%f  %f\n",fabs(b-execord->a),execord->b);
#endif
	  }
	}
	ExecTurn(obj,execord->a);
	break;
      case TURN:
#if DEBUG
	if(debugai && cv==obj)printf("TURN %d\n",execord->time);
#endif
	obj->accel=0;
	ExecTurn(obj,execord->a);
	break;

      case ACCEL:
	break;
	
      case STOP:
#if DEBUG
	if(debugai && cv==obj)printf("STOP %d\n",execord->time);
#endif
	ExecStop(obj,0);
	break;
      case ATTACK:
#if DEBUG
	if(0||(cv==obj &&debugai))printf("ATTACK %d\n",execord->time);
#endif
	ExecAttack(lhobjs,obj,execord,mainord,d2_enemy);
	break;	

      case LAND:
#if DEBUG
	if(debugai && cv==obj)printf("LAND %d\n",execord->time);
#endif
	ExecLand(obj,execord);
	break;
      case EXPLORE:
      case RETREAT:
      case GOTO:
	ExecGoto(obj,execord);
	break;
	
      default:
	break;
      }
      execord->time--;
    }/*     if(mainord->time>0) */

    if(execord->time<=0){
      ReadOrder(&(obj->actorder),obj,MAXPRIORITY);
    }
  } /*  if(obj->actorder.id!=-1) */

  /********************** 
   * --execute the order 
   *********************/


  return;
} /*  --ai() */


Segment *LandZone(struct Planet *planet){
  /* 
     returns a pointer to a landzone segment of the planet planet.
     NULL if it is not found.
  */

  Segment *s;

  s=planet->segment;
  
  while(s!=NULL){
    if(s->type==LANDZONE)
      return s;
    s=s->next;
  }
  return(NULL);
}

int HigherPoint(struct Planet *planet){
  /* 
     returns the highest point of the planet
   */ 
  Segment *s;
  int h=-1;

  if(planet==NULL)return(-1); /* invalid value */
  s=planet->segment;
  if(s!=NULL)
    h=s->y0;

  while(s!=NULL){
    if(s->y0 > h)h=s->y0;
    if(s->y1 > h)h=s->y1;
    s=s->next;
  }
  return(h);
}

void ExecGoto(Object *obj,struct Order *ord){
  /* 
     Send a ship to an universe point or to an object.
     ord.a:  x coordinates of the objetive.
     ord.b:  y coordinates of the objetive.
     ord.c:  id of the objetive. -1 if is a universe point.
     ord.d:  objetive type.
     ord.e:  objetive id.
     ord.f:  switch variable.
     return:
     0 if the objective is reached.
  */
  float rx,ry;
  float c;
  float d2,d02;
  float ab,bc,ac;
  float b,v2;
  int swaccel;

  rx=ord->a - obj->x; 
  ry=ord->b - obj->y;

  d2=rx*rx+ry*ry;

  switch((int)ord->d){
  case PLANET:
    d02=100000;  /* ori   d02=22500; */
    if(ord->g>100000)d02=150000;
    if(ord->g>125000)d02=200000;
    if(d2<d02){  /* ori 40000 objetive reached. */
      ExecBrake(obj,4);
      return;
    }
    break;
  case SHIP:
  default:
    d02=40000;
    v2=obj->vx*obj->vx+obj->vy*obj->vy;
    if(v2>225)d02*=2;
    if(d2 <d02){  /* ori 40000 objetive reached. */
      ExecStop(obj,0);
      return;
    }
    break;
  }
  
  if(d2 <d02){  /* ori 40000 objetive reached. */
    ExecStop(obj,0);
    return;
  }

  c=atan2(ry,rx);
  ac=obj->a-c;
  if(ac > PI)ac-=2*PI;
  if(ac < -PI)ac+=2*PI;
  ac=ac >= 0 ? ac : -ac;

  if((int)ord->f==1){
    if(ac>.025){
      ExecTurn(obj,c);
      /*     return; */
    }
    else{
      obj->ang_a=0;
      ord->f=0;
    }
  }
  else{
    if(ac>.05){ 
      ExecTurn(obj,c);
      ord->f=1;
      /*     return; */
    }
    else{
      obj->ang_a=0;
    }
  }

  v2=obj->vx*obj->vx+obj->vy*obj->vy;
  b=atan2(obj->vy,obj->vx);  /*  velocity angle */
  ab=obj->a-b;
  if(ab > PI)ab-=2*PI;
  if(ab < -PI)ab+=2*PI;
  ab=ab >= 0 ? ab : -ab;
  
  bc=b-c;
  if(bc > PI)bc-=2*PI;
  if(bc < -PI)bc+=2*PI;
  bc=bc >= 0 ? bc : -bc;
  
  if(obj->gas>0){
    swaccel=0;


    if(ab > .08 && bc > .08){
      swaccel=1;
    }
    if(v2<0.9*obj->engine.v2_max*(1-0.4375*(obj->state<25))){
      swaccel=1;
    }
    if(swaccel){
      obj->accel+=obj->engine.a;
      if(obj->accel > obj->engine.a_max)
	obj->accel=obj->engine.a_max;
    }
    else{
      obj->accel=0;
    }
  }
  else{
    obj->accel=0;
  }
  return;
  
}

void ExecLand(Object *obj,struct Order *ord){
  /*
    version 01.Jan 2011 
    Execute the order LAND in a planet.
   */
  float v2;
  float x0;
  float a,ia;
  float b;
  float dx,dy;
  float vxmax,vymax;
  float fvy;
  float fa;/* 0.125; */
  int swa,swvx,swvy;
  float ang_min,ang_max;
  int pmass;
  int fabsdx;

  ang_min=0;
  ang_max=PI;
  b=PI/6; /* max ang */

  if(obj->habitat!=H_PLANET){ /* obsolete order */
    ord->time=0; 
    return;
  }

  if(obj->mode==LANDED){
    if(fabs(obj->a-PI/2)>0.05){
      ExecTurn(obj,PI/2);
      return;
    }
  }

  v2=obj->vx*obj->vx+obj->vy*obj->vy;

  /* Stopping */ 
  if(v2>100){
    ExecStop(obj,0);
    return;
  }

  if(obj->vy*obj->vy>25){
    ExecStop(obj,0);
    return;
  }

  x0=ord->a+obj->radio+(ord->b-2.*obj->radio)*(obj->pid%10)/10.; /* Landing point */

  dx=obj->x-x0;

  if(dx>LXFACTOR/2 || dx<-LXFACTOR/2)dx*=-1;

  if(dx>200)dx=200;
  if(dx<-200)dx=-200;

  vxmax=vymax=5;

  fabsdx=fabs(dx);
  if(fabsdx<100){vxmax=4;
    if(fabsdx<50){vxmax=3;
      if(fabsdx<25){vxmax=2;
	if(fabsdx<15){vxmax=1;
	  if(fabsdx<10){vxmax=.2;
	  }
	}
      }
    }
  }


  dy=obj->y-ord->f;

  if(dy<100){vymax=4;
    if(dy<50){vymax=2;
      if(dy<30){vymax=1;
      }
    }
  }

  fvy=1-fabs(obj->vy/vymax);
  if(fvy<0)fvy=0;
  if(obj->vy>0)fvy=1;  

/*  brake */
  a=PI/2;

  swvx=0;
  if(dx>0){
    if(obj->vx>-vxmax && obj->vx<-vxmax+1){a=-b;} /* OK */
    if(obj->vx<-vxmax ){a=-b;swvx=1;}    /* brake */
    if(obj->vx>-vxmax+1 ){a=b;swvx=1;} /*accel */
  }
  if(dx<0){
    if(obj->vx>vxmax-1 && obj->vx<vxmax){a=b;} /* OK */
    if(obj->vx<vxmax-1 ){a=-b;swvx=1;}  /* accel */
    if(obj->vx>vxmax ){a=b;swvx=1;} /* brake */
  }

  swvy=0;
  if(obj->vy<-vymax){swvy=1;}
  if(obj->vy>1)swvy=0;
  if(dy<0){swvy=1;}

  if(swvy){
    if(swvx){
      if(a==-b)a=-b/2;
      if(a==b)a=b/2;
    }
    else a=0;
  }

/* going down  */

  if((fabs(obj->x-x0)< obj->radio)){

    if(fabs(obj->vx)<1){
      a=0;
      swvx=0;
      if(fabs(obj->a-PI/2)<.1)
	obj->a=PI/2;
      obj->vx=0;
      obj->ang_a=0;
      ord->f=ord->c;
      dy=obj->y-ord->f;
      if(dy<100)vymax=4;
      if(dy<50)vymax=2;
      if(dy<30)vymax=1;
    }
  }

  ia=obj->a-a-PI/2;
  if(ia > PI)ia-=2*PI;
  if(ia < -PI)ia+=2*PI;

  swa=0;
  /*  if(ia>0.5*obj->engine.ang_a*DT*DT*(100./obj->mass)){ */
  if(fabs(ia)>.07){



    if(obj->a<PI/2-b)ia=-1;
    if(obj->a>PI/2+b)ia=1;

    if(obj->a<0)ia=-1;
    if(obj->a<-PI/2)ia=1;


    obj->ang_a+=obj->engine.ang_a*(ia > 0 ? -1 : 1);

    if(obj->ang_a > obj->engine.ang_a_max) 
      obj->ang_a=obj->engine.ang_a_max;
    if(obj->ang_a < -obj->engine.ang_a_max)
      obj->ang_a=-obj->engine.ang_a_max; 

    if(ia>0 && obj->ang_a>0)obj->ang_a=0;
    if(ia<0 && obj->ang_a<0)obj->ang_a=0;


  }
  else{
    obj->ang_a=0;
    obj->ang_v=0;
    obj->a=a+PI/2;
    swa=1;
  }

  if(swa==0){
    if(swvy){
      if(obj->a>0 && obj->a<PI)swa=1;
    }
  }
  if((obj->vy > 0) && (obj->y > ord->f)){
    swa=0;
  }

  /* accelerate */ 
/*   if(d && obj==cv) */
/*     printf("swvx=%d swvy=%d swa=%d dx=%.1f vxm=%.1f\n",swvx,swvy,swa,dx,vxmax); */

  if(swa&&(swvx||swvy)){
    pmass=obj->in->mass;
    fa=1;

    fa=(fabs(fabs(obj->vx)-vxmax)+fabs(fabs(obj->vy)-vymax))/10-0.1;
    fa+=0.4*pmass/MAXPLANETMASS;
    if(fa<.3)fa=.3; 
    if(fa>1.1)fa=1.1; 

    if(obj->gas>0){
      obj->accel+=fa*obj->engine.a;
      if(obj->accel > obj->engine.a_max)
	obj->accel=obj->engine.a_max;
    }
  }
  else{
    obj->accel=0;
  }
  return;
}


void ExecAttack(struct HeadObjList *lhobjs,Object *obj,struct Order *ord,struct Order *morder,float d2){
  /*
    version 04 21Oct2010
    attack subroutine
  */
  
  float rx,ry,b,d,ia,ib,ic,id;
  float alcance2;
  int swaccel=0;

  if(obj==NULL){
    fprintf(stderr,"ERROR in ExecAttack()\n");
    exit(-1); 
  }

  if(obj->dest==NULL){
    ord->time=0;
    return;
  }

  if(obj->dest->in!=obj->in && obj->mode==LANDED){
    ExecStop(obj,0);
    if(cv==obj)printf("stop\n");
    return;
  }
  if(morder!=NULL){
    if(morder->id==STOP){
      if(obj->vx*obj->vx+obj->vy*obj->vy>.4){
	//	if(obj==cv)printf("stoping\n");
	ExecStop(obj,0);
	return;
      }
    }
  }

  rx=obj->dest->x - obj->x;
  ry=obj->dest->y - obj->y;
  b=atan2(ry,rx);

  ia=obj->a-b; /* angle between objetive and ship direction */
  if(ia > PI)ia-=2*PI;
  if(ia < -PI)ia+=2*PI;
  
  /**** Shooting ******/
  /* if(obj==cv)printf("aiming %f\n",fabs(ia)); */
  if(fabs(ia)<.05){
    /* if(obj==cv)printf("aiming 2 %f %f \n",ia,ord->a); */
    if(ia*ord->a<0 || fabs(ia)<.001){
      /* if(obj==cv)printf("aiming 3\n"); */
      (ord->a)*=-1;

      
      obj->weapon=ChooseWeapon(obj);
      alcance2=.5*obj->weapon->projectile.max_vel*obj->weapon->projectile.life;
      alcance2*=alcance2;
      if(alcance2>rx*rx+ry*ry){
	if(FireCannon(lhobjs,obj,obj->dest)==0){
	  Play(obj,FIRE0,1);
	}
	if(GameParametres(GET,GNET,0)==TRUE){
	  if(GetProc()==players[obj->player].proc){
	    obj->ttl=0;
	    SetModified(obj,SENDOBJMOD0);
	  }
	}
      }
    }
  }

  /***** if a big asteroid is near: stop *****/
  if(obj->dest->type==ASTEROID && 
     (obj->dest->subtype==ASTEROID1 || obj->dest->subtype==ASTEROID2)){
    if(d2<160000 && obj->vx*obj->vx+obj->vy*obj->vy>50){    
      ExecStop(obj,0);
      return;
    }
  }


  /***** inner planet *****/
  
  if(obj->habitat==H_PLANET && obj->engine.a_max && obj->mode!=LANDED){


    /***** if is near ground: go up *****/

    //    if(obj->y<.5*LYFACTOR){  /* refer to high planet point */
    if(obj->y<ord->b+.35*LYFACTOR){  /* refer to higher planet point */
      ord->time=0;
      if(obj->a<PI/2 && obj->a>-PI/2){
	obj->ang_a+=obj->engine.ang_a;
      }
      else{
	obj->ang_a-=obj->engine.ang_a;
      }
      
      if(obj->ang_a > obj->engine.ang_a_max) 
	obj->ang_a=obj->engine.ang_a_max; 
      if(obj->ang_a < -obj->engine.ang_a_max) 
	obj->ang_a=-obj->engine.ang_a_max; 
      
      ib=obj->a-PI/2;
      if(ib > PI)ib-=2*PI;
      if(ib < -PI)ib+=2*PI;

      if(fabs(ib)<0.5)  /**obj->engine.ang_a*DT*DT*(100./obj->mass))  */
 	obj->ang_a=0; 

      if(obj->vy < 10+(float)obj->in->mass/15000){
	if(obj->engine.a_max){
	  obj->accel+=obj->engine.a;
	  if(obj->accel > obj->engine.a_max)
	    obj->accel=obj->engine.a_max;
	}
      }
      else{
	obj->accel=0;
      }
      return;
    }    /***** --if is near ground go up *****/
    else{  
      int vmax2=144;
      if(obj->vx*obj->vx+obj->vy*obj->vy>vmax2 || obj->vy>0 ){
	ExecStop(obj,0);
	return;
      }
    }
  }
  /* --inner planet */
  
  /* turning  */
  if(fabs(ia)>.1){
    if(ia>0)ic=0.25;
    if(ia<0)ic=-0.25;
    ExecTurn(obj,b-ic);
  }

  /***** accel *****/

  if(morder!=NULL){
    if(morder->id==RETREAT||morder->id==STOP){
      return;
    }
  }

  switch(obj->type){
  case SHIP:
    swaccel=0;
    if(obj->mode==LANDED){
      if(obj->gas<.4*obj->gas_max){
	obj->accel=0;  
	return;
      }
      if(obj->weapon0.n<0.75*obj->weapon0.max_n){
	obj->accel=0;  
	return;
      }
    }
    
    if(obj->habitat==H_PLANET){
      obj->accel=0;
      swaccel=0;
    }
    else{
      d=atan2(obj->vy,obj->vx);
      id=obj->a-d; /* angle between ship and ship velocity */
      if(id > PI)id-=2*PI;
      if(id < -PI)id+=2*PI;

      if(d2<40000){
	swaccel=1;
	obj->accel=0;
	return;
      }

      if(fabs(id)<.1&& fabs(obj->vx*obj->vx+obj->vy*obj->vy - obj->engine.v2_max)<0.1){
	obj->accel=0;
	swaccel=0;
	return;
      }
      else{swaccel=1;}
      
      
      /* Accelerating */
      if(fabs(id)>.1){
	swaccel=1;
      }
    }

    if(swaccel){
      if(obj->engine.a_max){
	obj->accel+=obj->engine.a;
	if(obj->accel>obj->engine.a_max)obj->accel=obj->engine.a_max;
      }
    }
    break;
  default:
    break;
  }
}

void ExecTurn(Object *obj,float b){
  /* 
     version 02
     The ship turns to angle b
  */
  int sgnia;
  float inca;
  float ia;

  ia=obj->a-b;
  if(ia > PI)ia-=2*PI;   /*  ia+=((ia<-PI)-(ia>PI))*(2*PI); */
  else{
    if(ia < -PI)ia+=2*PI;
  }

  if(fabs(ia)<.05 && fabs(obj->ang_v)<.05){ 
    obj->a=b; 
    obj->ang_a=obj->ang_v=0; 
  } 
  else{
    
    sgnia=ia>0?-1:1;

    if(fabs(obj->ang_v)>fabs(0.28*ia)){ /* decelerate */
     sgnia=obj->ang_v>0?-1:1;      
    }      
    
    inca=sgnia*obj->engine.ang_a;
    
    if(inca>0 && obj->ang_a<0)obj->ang_a=0;
    else{
      if(inca<0 && obj->ang_a>0)obj->ang_a=0;
    }
    
    obj->ang_a+=inca;
    
    if(obj->ang_a > obj->engine.ang_a_max){ 
      obj->ang_a=obj->engine.ang_a_max; 
    }
    else{
      if(obj->ang_a < -obj->engine.ang_a_max) 
	obj->ang_a=-obj->engine.ang_a_max; 
    }
  }
  return;
}


void ExecStop(Object *obj,float v0){
  /*
    version 04
    Stop the ship to a velocity lower than v0.
   */
  float a,b,ia;
  float v2;
  float ang_v;
  float inca;
  float A;

  v0=0;

  if(obj->mode==LANDED){
    if(fabs(obj->a-PI/2)>0.05){
      ExecTurn(obj,PI/2);
    }
    else{
      obj->ang_v*=.95;
      obj->ang_a=0;
    }
    return;
  }

  if(!obj->engine.a_max)return;

  v2=obj->vx*obj->vx+obj->vy*obj->vy;

  if(v2<0.01){

    obj->accel=0;
    if(obj->ang_v>0.025||obj->ang_v<.025){
      obj->ang_v*=.95;
      obj->ang_a=0;
    }
    return;
  }
  ang_v=atan2(obj->vy,obj->vx);

  b=ang_v+PI;
  if(b>PI)b-=2*PI;

  a=obj->a - b;
  if(a > PI)a-=2*PI;
  if(a < -PI)a+=2*PI;
  ia=fabs(a);

  inca=obj->engine.ang_v_max*DT;

  if(ia>inca){
    obj->accel*=.8;
    ExecTurn(obj,b);
  }
  else{
    obj->a=b;
    obj->ang_a=0;
  }

  if(ia<PI/2){
    if(v2>20)
      A=1;
    else if(v2>2)
      A=.2;
    else if(v2>.01)
      A=0.01;
    else{
      A=0;
      obj->accel=.5;
    }
    obj->accel+=A*obj->engine.a;
    
    if(obj->accel>obj->engine.a_max)
      obj->accel=obj->engine.a_max;
  }
  else{
    obj->accel=0;
  }
}


int ExecBrake(Object *obj,float v0){
  /*
    version 01
    the ship decelerate to speed v0
    returns:
    0 objetive reached
    1 velocity is greater than v0
    2 error, ship has not engine
   */
  float a,b,ia;
  float v2;

  if(!obj->engine.a_max)return(2);

  v2=obj->vx*obj->vx+obj->vy*obj->vy;

  if(v2<v0*v0){
    //    obj->accel=0;
    return(0); /* reached objective */
  }

  b=atan2(obj->vy,obj->vx)+PI;
  if(b>PI)b-=2*PI;

  a=obj->a - b;
  if(a > PI)a-=2*PI;
  if(a < -PI)a+=2*PI;
  ia=fabs(a);

  if(ia>obj->engine.ang_v_max*DT){
    obj->accel=0;
    ExecTurn(obj,b);
  }
  else{
    obj->a=b;
    obj->ang_a=0;
  }

  if(ia<.1){
    if(v2-v0*v0<50){
      obj->accel=0;
    }
    obj->accel+=obj->engine.a;
    if(obj->accel>obj->engine.a_max)
      obj->accel=obj->engine.a_max;
  }
  else{
    obj->accel=0;
  }
  return(1);
}


Object *CCUpgrade(struct HeadObjList *lhobjs,struct Player *player){
  /*
    version 00 18Nov10
    Keep simple:
    Upgrade:
    
    -the first landed ship of minlevel

    return:
    the type of ship to upgrade
    in *obj2upgrade the ship to upgrade
    -1 if ther are no objjects to upgrade.
   */
  struct ObjList *ls;
  Object *obj,*retobj;
  int minlevel=0;
  int sw=0;
  int playerid=player->id;
  int obj2upgrade=0;
  float cut;
  retobj=NULL;


/* what upgrade */
/* default update fighter */

  cut=20*player->balance/player->nplanets;
  cut=cut>.9?.9:cut;
  cut=cut<.1?.1:cut;

 if(Random(-1)>cut){
   obj2upgrade=1;  /* TOWER */
 }

/*--what upgrade */

  ls=lhobjs->next;
  while(ls!=NULL){
    obj=ls->obj;
    if(obj->player!=playerid){ls=ls->next;continue;}
    if(obj->type!=SHIP){ls=ls->next;continue;}
    if(obj->mode!=LANDED){ls=ls->next;continue;}
    if(obj->level>=player->maxlevel-1){ls=ls->next;continue;}
    if(obj2upgrade==0){ /* not a tower*/
      if(obj->subtype==TOWER){ls=ls->next;continue;}
    }
    else{ /* a tower */
      if(obj->subtype!=TOWER){ls=ls->next;continue;}
    }

    if(sw==0){
      minlevel=obj->level;
      retobj=obj;
      sw++;
    }
    else{
      if(obj->level<minlevel){
	minlevel=obj->level;
	retobj=obj;
      }
      if(obj->level==minlevel){
	if(Random(-1)<.20){
	  minlevel=obj->level;
	  retobj=obj;
	}
      }
    }
    ls=ls->next;
  }
  if(0&&retobj!=NULL){
    printf("player:%d UPGRADE %d \n",retobj->player,retobj->pid);
  }
  return(retobj);
}



int CCBuy(struct CCDATA *ccdata,struct Player player,int *planetid){
  /*
    version 01 29Oct10
    Keep simple:
    Buy: 
    -if tower<2 tower    
    -if no are more inexplore planets, no explorer. sell explorer.
    -if there unknown planets, 5 explorer by planet max 8.

    -if tower<3
    --fighter or tower or explorer.
    -max rel. fighter/tower 1

    return the type of ship to buy
    in *planetid the planet id where to buy.
    -1 if dont buy.
   */

  int np;
  struct PlanetInfo *pinfo;

  if(ccdata->planetlowdefense!=NULL){
    if(player.id!=ccdata->planetlowdefense->player){
      fprintf(stderr,"WARNING: player: %d planetlow: %d\n",player.id,ccdata->planetlowdefense->id);
      ccdata->planetlowdefense=NULL;
      return(-1);
    }
  }

  pinfo=GetPlanetInfo(ccdata,ccdata->planetlowdefense);
  /* if ntower < 2 buy tower */
  if(pinfo->ntower<2){
    *planetid=ccdata->planetlowdefense->id;
    //    printf("CCBuy 1: planet: %d ntower: %d\n",ccdata->planetlowdefense->planet->id,ccdata->planetlowdefense->ntower);

    return(TOWER);
  }

  if(pinfo->ntower<3 && player.balance<0){
    *planetid=ccdata->planetlowdefense->id;
    //    printf("CCBuy 2\n");
    return(TOWER);
  }

  if(player.balance<0){
    //    printf("balance: %f \n",player.balance);
    return(-1);
  }

  /* if there unknown planets, 5 explorer by planet max 8.*/
  np=GameParametres(GET,GNPLANETS,0);

  if(np-ccdata->nkplanets>0 && ccdata->nexplorer<8){
    if(ccdata->nkplanets>1 || ccdata->nexplorer<5){
      *planetid=ccdata->planetlowdefense->id;
      return(EXPLORER);
    }
  }

  /* if number of fighters is very low  */
  if((float)ccdata->nfighter/(float)ccdata->nplanets < 0.5 &&
     player.balance>.15){
    *planetid=ccdata->planetweak->id;
    return(FIGHTER);
  }


  /* if ntowers/nplanets < 2.5 buy tower */
  if(ccdata->nplanets>5 && 
     ((float)ccdata->ntower/(float)ccdata->nplanets)<2.5){
    *planetid=ccdata->planetlowdefense->id;
    //    printf("CCBuy 3\n");
    return(TOWER);
  }


  /* max rel. fighter/tower 1 */
  if((float)(ccdata->nfighter+ccdata->nexplorer/2)/(float)ccdata->ntower < 1.2){ //HERE adjust this value
    *planetid=ccdata->planetweak->id;
    return(FIGHTER);
  }

  /* if ntower < 3 buy tower */
  if(pinfo->ntower<3){
    *planetid=ccdata->planetlowdefense->id;
    //    printf("CCBuy 4\n");
    return(TOWER);

  }
  return(-1);
}

void ControlCenter(struct HeadObjList *lhobjs,struct Player player){
  /*
    version 07 070411
    Manage COMPUTER SHIPS orders.
    equivalent to human keyboard orders.
    one order by turn (20/s.)

    Decide if sell ships
    Decide if buy ships
    Decide if send some ship to a planet.
    Decide if send some ship to explore.
    Decide if send ship to another one, to form groups.(TODO)
  */

  struct ObjList *ls;
  Object *obj;
  struct NearObject nobjs[5];
  struct Order ord;
  int ordersw; /* send only one ship by turn */
  int i;
  int m;
  int no;
  int price;
  int swgoto;

  float d2;
  Object *niplanet=NULL;
  Object *naplanet=NULL;
  Object *neplanet=NULL;
  Object *nplanet=NULL;
  struct Order *actord=NULL;
  int sw=0;
  float maxx,maxy;
  int time;
  int gnplanets;

  struct CCDATA *ccdata;
  static int cont=0;


  struct PlanetInfo *pinfo1;

  pinfo1=NULL;
  no=0;
  maxx=0.55*GameParametres(GET,GULX,0);
  maxy=0.55*GameParametres(GET,GULY,0);


  time=GetTime();


  /* gathering information */

  ccdata=&ccdatap[player.id];
  ccdata->time--;

  if(ccdata->time<=0){ /* Rebuild all */
    ccdata->time=200+player.id;

    /* 
       number of ships in every planet,
       build the ccdata list 
    */
    /* HERE TODO quitar destroyccplanet()*/
    //DestroyCCPlanetInfo(ccdata);
    /* update ccdata list with known info */
    CalcCCInfo(lhobjs,&listheadkplanets[player.id],player.id,ccdata);
    /* Calc the planet with less towers and weakest. */
    CalcCCPlanetStrength(player.id,ccdata);
  }

  //  printf("player: %d --",player.id);

  if(player.id==2)cont++;

  if(0&&player.id==4){
    printf("player: %d -- %p\n",player.id, ccdata->planetinfo);   
    PrintCCPlanetInfo(ccdata);    
  }
  /* --gathering information */

  gnplanets=GameParametres(GET,GNPLANETS,0);
#if DEBUG
  if(ccdata->war && debugwar){
    printf("WAR player: %d war:%d p1:%p\n",player.id,ccdata->war,pinfo1);
  }
#endif

  pinfo1=War(lhobjs,player,ccdata);

#if DEBUG
  if(ccdata->war && debugwar){
    printf("WAR\tplayer: %d war:%d p1:%p\n",player.id,ccdata->war,pinfo1);
  }
#endif

  if(BuyorUpgrade(lhobjs,player,ccdata)!=0){
    //    printf("CCCP %d %d\n",ccdata->planetweak->id,GetTime());
    CalcCCInfo(lhobjs,&listheadkplanets[player.id],player.id,ccdata);
    CalcCCPlanetStrength(player.id,ccdata);
    //    printf("CCCP %d\n",ccdata->planetweak->id);
  }

  /*** Sell ships ***/

  if((player.gold<1 && player.balance<0)){
    obj=ObjMinExperience(lhobjs,player.id);
    if(obj!=NULL){
      price=.5*GetPrice(NULL,obj->subtype,obj->engine.type,obj->weapon->type);
      if(price>0){
	players[obj->player].gold+=price;
	obj->state=-1;
	/*      obj->modified=SENDOBJKILL; */
	if(GameParametres(GET,GNET,0)==TRUE){
	  if(GetProc()==players[obj->player].proc){
	    obj->ttl=0;
	    SetModified(obj,SENDOBJKILL);
	  }
	}
      }
/*       return; */
    }
  }
  /*** --Sell ships ***/

  /* Send ships to inexplore or enemy planet*/  

  m=4;  
  for(i=0;i<m+1;i++){
    nobjs[i].obj=NULL;
    nobjs[i].d2=-1;
  }

  ordersw=0;
  ls=lhobjs->next;
  while(ls!=NULL && ordersw==0){
    obj=ls->obj;
    if(obj->player != player.id){ls=ls->next;continue;}
    if(obj->type != SHIP){ls=ls->next;continue;}
    if(obj->engine.type <= ENGINE1){ls=ls->next;continue;}
    
    /* Getting info from enemy */

    if(obj->habitat==H_PLANET){
      if(players[obj->in->player].team!=players[obj->player].team){
	CalcEnemyPlanetInfo(lhobjs,ccdata,obj);

      }
    }

    /*--Getting info from enemy */
    
    actord=ReadOrder(NULL,obj,MAINORD);

    if(actord==NULL){
      ord.priority=1;
      ord.id=NOTHING;
      ord.time=0;
      ord.g_time=time;
      ord.a=0;
      ord.b=0;
      ord.c=-1;
      ord.d=0;
      ord.e=ord.f=ord.g=0;
      ord.h=1;
      DelAllOrder(obj);
      AddOrder(obj,&ord);
      actord=ReadOrder(NULL,obj,MAINORD);
    }

    switch(actord->id){
    case NOTHING:
      switch(obj->mode){
      case LANDED:
	/* sending ship to atack or to new planets */
	if(CountShipsInPlanet(lhobjs,ls->obj->in->id,SHIP,-1,2)<2)break;

	/* goto nearest inexplore or empty planet */
	no=NearestCCPlanets(ccdata,obj,PINEXPLORE,nobjs);


	/* if war */
	if(ccdata->war && obj->subtype==FIGHTER){
	  switch(ccdata->war){
	  case 2:
	    if(pinfo1!=NULL){
	      if(pinfo1->nassigned<MAXnf2a){ // HERE this number must be global
		nobjs[0].obj=ccdata->planet2meet;
	      }
	    }
	    else{
#if DEBUG
	      if(debugwar){
		printf("Warning pinfo1==NULL player: %d war: %d\n",player.id,ccdata->war);
	      }
#endif

	    }
	    break;
	  case 3:
	    if(obj->habitat==H_PLANET && obj->in->id==ccdata->planet2meet->id){
	      nobjs[0].obj=ccdata->planet2attack;
#if DEBUG
	      if(debugwar){
		printf("player: %d send %d to attack\n",ccdata->player,obj->pid);
	      }
#endif
	    }
	    if(0&&obj->habitat==H_PLANET && obj->in->id==ccdata->planet2attack->id){
	      ccdata->time=0;
	      ccdata->war=0;
#if DEBUG
	      if(debugwar){
		printf("attack finished(0)\n");
	      }
#endif
	    }
	    break;
	  default:
	    break;
	  }
	}
	/*--if war */

	if(player.nplanets<gnplanets || no>0){
	  swgoto=0;
	  if(obj->gas>.98*obj->gas_max && obj->state > 95){
	    swgoto++;
	  }
	  if(obj->weapon0.n>=.95*obj->weapon0.max_n){
	    swgoto++;
	  }
	  if(swgoto==2){
	    
	    if(nobjs[0].obj!=NULL){
	      if(nobjs[0].obj!=obj->in){
		ord.priority=1;
		ord.id=GOTO;
		ord.time=0;
		ord.g_time=time;
		ord.a=nobjs[0].obj->x;
		ord.b=nobjs[0].obj->y;
		ord.c=nobjs[0].obj->id;
		ord.d=nobjs[0].obj->type;
		ord.e=nobjs[0].obj->pid;
		ord.g=nobjs[0].obj->mass;
		ord.f=ord.h=0;

		DelAllOrder(obj);
		AddOrder(obj,&ord);
		ordersw++;
	      }
	    }
	    else{
	      if(no==0 && obj->subtype==EXPLORER){   /* explore */
		
		ord.priority=1;
		ord.id=EXPLORE;
		ord.time=0;
		ord.g_time=time;
		
		ord.a=ord.b=0;
		ord.c=-1;
		ord.d=ord.e=ord.f=ord.g=0;
		ord.h=1;
		DelAllOrder(obj);
		AddOrder(obj,&ord);
		ordersw++;
	      }
	    }
	  }
	}
	break;
      case NAV:/* actual order=NOTHING */

	if(1){
	  if(obj->gas>0.25*obj->gas_max && obj->cdata->a==0){ /* return or explore*/
	    naplanet=NearestObj(lhobjs,obj,PLANET,PALLY,&d2); /* double loop */
	    niplanet=NearestObj(lhobjs,obj,PLANET,PINEXPLORE,&d2); /* double loop */
	    
	    neplanet=NULL;
	    if(naplanet==NULL  && niplanet==NULL){ 
	      neplanet=NearestObj(lhobjs,obj,PLANET,PENEMY,&d2); /* double loop */
	    } 
	    nplanet=NULL;
	    if(naplanet!=NULL){
	      nplanet=naplanet;
	    }
	    if(niplanet!=NULL){
	      nplanet=niplanet;
	    }
	    if(nplanet==NULL){
	      nplanet=neplanet;
	    }
	    
	    if (nplanet!=NULL){
	      ord.priority=1;
	      ord.id=GOTO;
	      ord.time=0;
	      ord.g_time=time;
	      ord.a=nplanet->x;
	      ord.b=nplanet->y;
	      ord.c=nplanet->id;
	      ord.d=nplanet->type;
	      ord.e=nplanet->pid;
	      ord.f=ord.h=0;
	      ord.g=nplanet->mass;
	      DelAllOrder(obj);
	      AddOrder(obj,&ord);
	      ordersw++;
	    }
	    else{
	      ord.priority=1;
	      ord.id=EXPLORE;
	      ord.time=0;
	      ord.g_time=time;
	      ord.a=0;
	      ord.b=0;
	      ord.c=-1;
	      ord.d=0;
	      ord.e=ord.f=ord.g=0;
	      ord.h=1;
	      DelAllOrder(obj);
	      AddOrder(obj,&ord);	      
	      ordersw++;
	    }
	  }
	} //if(1)
	break;
      default:
	fprintf(stderr,"ERROR in ControlCenter. mode unknown %d (id:%d)\n",obj->mode,obj->id);
	exit(-1);
	break;
      }
      break;
    case STOP:
      if(obj->vx*obj->vx+obj->vy*obj->vy<.1){
	ord.priority=1;
	ord.id=NOTHING;
	ord.time=0;
	ord.g_time=time;
	ord.a=ord.b=ord.c=ord.d=ord.e=ord.f=ord.g=ord.h=0;
	DelAllOrder(obj);
	AddOrder(obj,&ord);
      }
      
      break;
    case GOTO:
      if(obj->mode==LANDED){
	ord.priority=1;
	ord.id=NOTHING;
	ord.time=0;
	ord.g_time=time;
	ord.a=ord.b=ord.c=ord.d=ord.e=ord.f=ord.g=ord.h=0;
	DelAllOrder(obj);
	AddOrder(obj,&ord);
	
      }
      break;
    case EXPLORE:
/*       printf("%d EXPLORE\n",obj->id); */
      sw=0;
      if(obj->x>maxx || obj->x<-maxx || obj->y>maxy || obj->y<-maxy){
	/* universe border reached, return to nearest planet */
	sw=1;
      }
      if(obj->gas<0.3*obj->gas_max){ /* return */
	sw+=2;
      }
      if(obj->cdata->obj[2]!=NULL && obj->cdata->d2[2] < 2*obj->radar*obj->radar){
	sw+=4; /* just discovered planet */
      }
/*       if(obj->cdata->obj[2]!=NULL){ */
/* 	printf("%d np:%d\n",obj->id,obj->cdata->obj[2]->id); */
/*       } */
/*       else{ */
/* 	printf("%d np:NULL\n",obj->id); */
/*       } */
/*       if(obj==cv){ */
/* 	if(obj->cdata->obj[2]!=NULL) */
/* 	  printf("np:%d\n",obj->cdata->obj[2]->id); */
/*       } */
      if(sw){
	nplanet=NULL;
	if(sw>=4){
	  nplanet=obj->cdata->obj[2];
	}
	if(nplanet==NULL){
	  nplanet=NearestObj(lhobjs,obj,PLANET,PALLY,&d2); /* double loop */
	  if(nplanet==NULL){
	    nplanet=NearestObj(lhobjs,obj,PLANET,PINEXPLORE,&d2); /* double loop */
	  }
	}
	if (nplanet!=NULL){
	  ord.priority=1;
	  ord.id=GOTO;
	  ord.time=0;
	  ord.g_time=time;
	  ord.a=nplanet->x;
	  ord.b=nplanet->y;
	  ord.c=nplanet->id;
	  ord.d=nplanet->type;
	  ord.e=nplanet->pid;
	  ord.f=ord.h=0;
	  ord.g=nplanet->mass;
	  DelAllOrder(obj);
	  AddOrder(obj,&ord);
	  ordersw++;
	}
	else{ /*if there no known planets, no own planets: stop */ 
	  /* */
	  if(sw==1){ 
	    /* if is out universe and nplanet==NULL 
	       -> GOTO known universe. goto 0,0 */
	    ord.priority=1;
	    ord.id=GOTO;
	    ord.time=0;
	    ord.g_time=time;
	    ord.a=0;
	    ord.b=0;
	    ord.c=-1;
	    ord.d=0;
	    ord.e=ord.f=ord.g=ord.h=0;
	    DelAllOrder(obj);
	    AddOrder(obj,&ord);
	    ordersw++;
	  }
	  else{
	    ord.priority=1;
	    ord.id=STOP;
	    ord.time=0;
	    ord.g_time=time;
	    ord.a=ord.b=ord.c=ord.d=ord.e=ord.f=ord.g=ord.h=0;
	    DelAllOrder(obj);
	    AddOrder(obj,&ord);
	  }
	}
      }
      break;
    default:
      fprintf(stderr,"ERROR in ControlCenter. order unknown %d\n",actord->id);
      exit(-1);
      break;
    }
    
    ls=ls->next;
  }

  if(ordersw>0){
    ccdata->time=0;
  }
  return;
}



Object *ObjFromPlanet(struct HeadObjList *lhobjs,int planetid,int player){
  /* 
     return the first LANDED object from the 
     planet with id id.
  */
  
  struct ObjList *ls;
  Object *obj;
  obj=NULL;
  ls=lhobjs->next;
  while(ls!=NULL){
    if(ls->obj->player==player){
      if(ls->obj->mode==LANDED && ls->obj->in->id==planetid){
	return(ls->obj);
      }
    }
    ls=ls->next;
  }
  return(obj);
}




Object *ObjMinExperience(struct HeadObjList *lhobjs,int player){
  /* 
     return a LANDED object (EXPLORER or FIGHTER) from player player
     with less experience in order to sell it.
  */
  
  struct ObjList *ls;
  Object *obj;
  int level=0;
  float experience=0;
  
  obj=NULL;
  ls=lhobjs->next;
  while(ls!=NULL){
    if(ls->obj->player==player){
      if(ls->obj->type==SHIP && ls->obj->mode==LANDED){

	switch(ls->obj->subtype){
	case EXPLORER:
	case FIGHTER:
	  if(obj==NULL){ /* first element */
	    level=ls->obj->level;
	    experience=ls->obj->experience;
	    obj=ls->obj;
	  }
	  if(ls->obj->level<=level){
	    if(ls->obj->level<level){
	      level=ls->obj->level;
	      experience=ls->obj->experience;
	      obj=ls->obj;
	    }
	    else{
	      if(ls->obj->experience<experience){
		experience=ls->obj->experience;
		obj=ls->obj;
	      }
	    }
	    if(obj->level==0 && obj->experience==0)return(ls->obj);
	  }
	  break;
	default:
	  break;
	}	
      }
    }
    ls=ls->next;
  }
  return(obj);
}


Object *Coordinates(struct HeadObjList *lhobjs,int id,float *x,float *y){
  /* 
     Load in x and y the coordinates of the object with id id 
     returns:
     a pointer to the object.
     NULL if the object doesnt exist.
  */
  struct ObjList *ls;
  ls=lhobjs->next;
  while(ls!=NULL){
    if(ls->obj->id==id){
      if(ls->obj->habitat==H_PLANET){
	*x=ls->obj->in->x;
	*y=ls->obj->in->y;
      }
      else{
	*x=ls->obj->x;
	*y=ls->obj->y;
      }
      return(ls->obj);
    }
    ls=ls->next;
  }
  return(NULL);
}


int Risk(struct HeadObjList *lhobjs,Object *obj,int morderid,int *orderid){
  /* 
     version 07 11Mar2011
     check if there are enemies, low fuel, ship is very damned or low ammunition
     choose the order 
     order can be ATTACK, TAKEOFF, GOTO, NOTHING, LAND, STOP.
     -TODO Add GOAWAY

     returns the type of danger found:
     0  no danger,
     >0 if there some danger: enemies, low gas, low state
     1 if there an enemy
     2 if has low gas or if the damage is high
     in *orderid the id of the order 
  */

  Object *planet_enemy,*planet_ally,*planet_inexplore,*ship_enemy;
  float  d2_inexplore,d2_enemy,d2_ally;

  int i;
  int num_actions=3;
  float action[3]={1,1,1};/* NOTHING, ATTACK, GOTO nearest*/
  int max_action;
  int ret=0;


  if(obj->cdata==NULL){
    fprintf(stderr,"ERROR: Risk(): obj %d type: %d cdata is NULL\n",
	    obj->id,obj->type);
    exit(-1);
  }

  *orderid=NOTHING; /*default, no problem */

  ship_enemy=obj->cdata->obj[0];
  obj->dest=ship_enemy;

  /******** no danger *************/
  if(ship_enemy == NULL && 
     obj->gas>.75*obj->gas_max &&
     obj->state>50 &&
     (float)obj->weapon0.n/obj->weapon0.max_n > .75){

    if(obj->gas>.75*obj->gas_max){
      obj->cdata->a=0;
    }
    return(0);
  }     
  /********************************/

  planet_enemy=obj->cdata->obj[1];
  d2_enemy=obj->cdata->d2[1];

  planet_inexplore=obj->cdata->obj[2];
  d2_inexplore=obj->cdata->d2[2];

  planet_ally=obj->cdata->obj[3];
  d2_ally=obj->cdata->d2[3];


  if(ship_enemy!=NULL){

    action[0]*=.5;action[2]*=.5;
    action[1]*=1.1;

    if(obj->subtype==TOWER){
      if(ship_enemy->in==obj->in){
	*orderid=ATTACK;
	ret=1;
      }
      else{
	*orderid=NOTHING;
	ret=0;
      }
      return(ret);
    }

    if(ship_enemy->type==ASTEROID && ship_enemy->habitat==H_SPACE){
      if(obj->weapon0.n==0){action[1]*=0;}
    }

    if(obj->mode==LANDED){
      if( (obj->state<50||obj->gas<.75*obj->gas_max)){
	if(ship_enemy->habitat==H_SPACE){
	  action[0]*=0;
	  action[1]*=0;
	  action[2]*=1.5;
	}
      }
      if(ship_enemy->in==obj->in){
	action[0]*=0;
	action[1]=1;
	action[2]*=0;
      }
    }
  }
  else{
    action[1]*=0;
  }

  /*** if no weapon go to nearest ****/
  if(obj->weapon0.n+obj->weapon1.n+obj->weapon2.n==0){
    action[1]*=0;action[2]*=1.1;
  }
  /*********************/

  if(obj->cdata->a){
    action[1]*=.5;action[2]*=1.2;
  }

  switch(obj->habitat){
  case H_PLANET:
    if(obj->gas<.25*obj->gas_max){
      action[1]*=.5;
      action[2]*=1.2;
    }
    if(obj->mode==LANDED){
      if(obj->weapon0.n<.75*obj->weapon0.max_n){
	action[0]*=0;
	action[2]*=1.1;       /* wait for ammunition */
      }
      if(obj->gas<.75*obj->gas_max){
	action[0]*=0;
	action[2]*=1.5; /* wait for gas */
      }
    }
    break;
  case H_SPACE:
    if(obj->gas<.25*obj->gas_max && obj->cdata->a==0){
      action[1]*=.5;action[2]*=1.2;
      if(obj->gas<.15*obj->gas_max){action[1]*=.5;action[2]*=1.2;} //.15
    }

    break;
  default:
    break;
  }

  if(obj->weapon0.n+obj->weapon1.n+obj->weapon2.n<20){action[2]*=1.2;}
  if(obj->state<25){
    action[2]*=1.5;
    if(obj->mode!=LANDED){
      action[1]=0;
    }
  }

  if(morderid==RETREAT){
    if(obj->mode!=LANDED){
      action[0]=0;action[1]=0;
      action[2]=1;    
    }
  }


  max_action=0;
  for(i=0;i<num_actions;i++){
#if DEBUG
    if((0||debugrisk)&&cv==obj)printf("%f ",action[i]);
#endif
    if(action[i]>action[max_action])max_action=i;
  }

#if DEBUG
  if((debugrisk)&&cv==obj){
    printf("maxaction=%d mode:%d\n",max_action,obj->mode);
    printf("state %f\n",obj->state);
    printf("gas %f\n",obj->gas);
  }
#endif

  switch(max_action){
  case 0:  /* NOTHING */
    ret=0;
    break;
  case 1:  /* ATTACK */
    *orderid=ATTACK;
    ret=1;
    if(obj->habitat==H_PLANET){
      if(obj->gas<.25*obj->gas_max || obj->state<25){
	if(obj->in->player==obj->player){
	  if(obj->mode!=LANDED){
	    *orderid=LAND;
	  }
	}
      }
      if(obj->mode!=LANDED && 
	 morderid==STOP){
	*orderid=LAND;
	printf("land\n");
      }
      
      if(ship_enemy->habitat==H_SPACE && morderid!=STOP){
	if(obj->weapon0.n>0.5*obj->weapon0.max_n && obj->gas>.50*obj->gas_max && obj->state>50){
	  if(obj->engine.a_max){
	    *orderid=TAKEOFF;
	  }
	}
	else{
	  if(obj->mode!=LANDED){
	    *orderid=LAND;
	  }
	}
      }
    }

    break;
  case 2:  /* GOTO nearest save planet */
    ret=2;
    *orderid=GOTO;

    if(obj->cdata->a==0)obj->cdata->a=1;

    obj->dest=NULL;

    if(planet_ally!=NULL && planet_inexplore!=NULL){
      if(d2_ally<d2_inexplore){
	obj->dest=planet_ally;
      }
      else{
	obj->dest=planet_inexplore;
      }
    }
    
    if(obj->dest==NULL){
      obj->dest=planet_ally;
      if(obj->dest==NULL){
	obj->dest=planet_inexplore;
      }
    }

    if(obj->dest==NULL){
      obj->dest=planet_enemy;
    }

    if(obj->dest==NULL){
      *orderid=EXPLORE;
    }

    if(obj->habitat==H_PLANET){
      if(ship_enemy==NULL){
	obj->dest=obj->in;
      }
    }

    if(obj->habitat==H_PLANET){
      if(obj->in==obj->dest){
	*orderid=LAND;
      }
      else{
	//	DelAllOrder(obj);
	*orderid=TAKEOFF;  /* HERE if there no planets must attack here*/

      }
      if(obj->mode==LANDED){
	*orderid=NOTHING;
      }
    }

    /* 
       If ship is without gas => STOP 
    */
#if DEBUG
    if(debugrisk && obj==cv){ 
      printf("Risk(0) cdata: %d  %d\n",obj->cdata->a,*orderid); 
    } 
#endif

    if(obj->cdata){
      if(obj->gas>.75*obj->gas_max){
	obj->cdata->a=0;
      }
      if(obj->habitat==H_SPACE){
	
	if(obj->gas<.04*obj->gas_max && obj->cdata->a==1){
	  *orderid=STOP;
	  obj->cdata->a=2;
	}
	
#if DEBUG
	if(debugrisk && obj==cv){ 
	  printf("Risk(-) cdata: %d  %d\n",obj->cdata->a,*orderid); 
	} 
#endif	
	if(obj->gas<.03*obj->gas_max){
	  obj->cdata->a=2;
	}
	
	if(obj->gas>.04*obj->gas_max && obj->cdata->a==2){
	  obj->cdata->a=3;
	}
	
	if(obj->gas>.25*obj->gas_max && obj->cdata->a==3){
	  obj->cdata->a=1;
	}
	
	switch(obj->cdata->a){
	case 0:
	  break;
	case 1:
	  break;
	case 2:
	  *orderid=NOTHING;
	  break;
	case 3:
	  *orderid=STOP;
	  break;
	default:
	  break;
	}
      }
    }
  
#if DEBUG
    if(debugrisk && obj==cv){ 
      printf("Risk(1) cdata: %d  %d\n",obj->cdata->a,*orderid); 
    } 
#endif
    
    break;
  default:
    fprintf(stderr,"ERROR in Risk()\n");
    exit(-1);
    break;
  } /*--switch max_action */

#if DEBUG  
  if(debugrisk){
    if(obj==cv && obj->dest!=NULL){
      printf("Risk() (%d)CHOOSE: %d  %d\n",obj->pid,*orderid,obj->dest->id);
    }
    if(obj==cv)printf("ret: %d\n",ret);
  }
#endif

  return(ret);
}


Weapon *ChooseWeapon(Object *obj){
  /*
    Choose what weapon to use from the available ones.
    returns:
    the weapon choosed.
  */

  int i,j;
  int max=0;
  int jmax=-1;
  int sum;
  Weapon *weapon;

  /*             SHOT1 MISSILE LASER */
  float a[5][3]={{500000,4000000,160000}, /* enemy distance2 */
		 {.25,.50,.25},           /* avability factor */
		 {100,100,300},           /* ship gas  */
		 {1,0,1},                 /* cost */
		 {1,1,1}};                /* number of proyectiles  */
  float b[5][3]={{0,0,0},   /* r */
		 {0,0,0},   /* n  */
		 {0,0,0},   /* gas */
		 {1,0,1},   /* cost */
		 {1,1,1}};   /* avability */

  weapon=NULL;

  /* shot asteroid only with SHOT */
  if(obj->cdata!=NULL){
    if(obj->cdata->obj[0]!=NULL){
      if(obj->cdata->obj[0]->type==ASTEROID){
	return(&obj->weapon0);
      }
    }
  }

  for(j=0;j<3;j++){
    if(j==0)weapon=&obj->weapon0;
    if(j==1)weapon=&obj->weapon1;
    if(j==2)weapon=&obj->weapon2;

    switch(weapon->projectile.type){
    case SHOT0:
      b[4][j]=0;
      break;
    case SHOT1:
    case MISSILE:/*SHOT3: */
    case LASER: /*SHOT4 */
      if(obj->dest_r2 < a[0][j])b[0][j]=1;
      if(weapon->n > a[1][j]*weapon->max_n)b[1][j]=1;
      if(obj->gas > a[2][j])b[2][j]=1;
      if(weapon->cont1!=0 )b[4][j]=0;/*.5;  HERE why 0.5 */
      if(weapon->n==0)b[4][j]=0;
      break;
     default:
      break;
    }
    sum=0;
    for(i=0;i<4;i++){
      sum+=b[i][j];
    }
    sum*=b[4][j];
    if(sum>max){max=sum;jmax=j;}
 
  }

  weapon=&obj->weapon0; /* default */
  if(jmax==1)weapon=&obj->weapon1;
  if(jmax==2)weapon=&obj->weapon2;
  
  return(weapon);
}


int FireCannon(struct HeadObjList *lhobjs,Object *obj1,Object *obj2){
  /*
    version 0.1 040810
   */

  float vp,r,d0,d1,ang,ang0;
  float x2,y2;
  float rx,ry;
  float vx1,vy1,vx2,vy2;
  int i;
  Object *obj;
  float gascost;
  int sw=1;
  int n;

  //  printf("FIRE\n"); 

  if(obj1==NULL){
    fprintf(stderr,"ERROR in FireCannon()\n");
    exit(-1); 
  }

  if(obj1->weapon==NULL)return(1);


  if(obj1->weapon->cont1){
    /*    printf("(%d) Weapon not ready\n",obj1->id); */
    return(2);
  }

  gascost=obj1->weapon->projectile.gascost;
  if(obj1->gas<gascost){
    /*    printf("(%d)Weapon without gas\n",obj1->id); */
    return(3);
  }

  if(obj1->weapon->n<1){
    /*    printf("(%d)Weapon no bullets\n",obj1->id); */
    return(4);
  }

  obj1->weapon->cont1=obj1->weapon->rate;

  obj1->gas-=gascost;
  if(obj1->gas < 0)obj1->gas=0;

  /*  gdk_beep(); */
  vp=obj1->weapon->projectile.max_vel;
  r=1.2*obj1->radio;

  if(AreEnemy(lhobjs,GetProc(),obj1)==0)sw=0;

  n=obj1->weapon->nshots;
  switch(obj1->weapon->type){
  case CANNON0:
    return(5);
    break;
  case CANNON1: /* shots */
  case CANNON2:
  case CANNON3:
  case CANNON5:
  case CANNON6:
    /* printf("FIRE2\n");  */


  ang0=0;
  if(obj2!=NULL){
    rx=obj2->x-obj1->x;
    ry=obj2->y-obj1->y;
    
    vx2=-obj1->vx; /* objetive at rest */
    vy2=-obj1->vy;
    
    d0=sqrt(rx*rx+ry*ry);
    rx=obj2->x+vx2*d0/vp-obj1->x;
    ry=obj2->y+vy2*d0/vp-obj1->y;
    
    ang0=atan2(ry,rx)-obj1->a;
  }
  for(i=0;i<n;i++){
    ang=obj1->a+ang0+0.04*(0.5*(1-n)+i);
    
    obj=NewObj(lhobjs,PROJECTILE,obj1->weapon->projectile.type,
	       obj1->x+r*cos(ang),obj1->y+r*sin(ang),
	       vp*cos(ang)+obj1->vx,vp*sin(ang)+obj1->vy,
	       CANNON0,ENGINE0,obj1->player,obj1,obj1->in);
    if(obj!=NULL){
      if(obj1->habitat==H_PLANET){
	obj->y+=obj1->radio;
	obj->y0+=obj1->radio;
      }
    }
    Add2ObjList(lhobjs,obj);
    
    if(sw==0){  /* if there are no enemies dont send */
      obj->modified=SENDOBJNOTSEND;
    }
    
    obj1->weapon->n--;
    if(obj1->weapon->n<0)obj1->weapon->n=0;
  }
  
  break;
  case CANNON4: /*tower, new precision cannon */
    //    printf("FIRE3 %f\n",vp); 
    if(obj2==NULL){
      ang0=0;
    }
    else{
      rx=obj2->x-obj1->x;
      ry=obj2->y-obj1->y;

      if(obj1->level<1){
	vx2=-obj1->vx; /* objetive at rest */
	vy2=-obj1->vy;

	d0=sqrt(rx*rx+ry*ry);
	rx=obj2->x+vx2*d0/vp-obj1->x;
	ry=obj2->y+vy2*d0/vp-obj1->y;
      }

      if(obj1->level==1){
	vx2=obj2->vx-obj1->vx; /* objetive in movement */
	vy2=obj2->vy-obj1->vy;
	
	d0=sqrt(rx*rx+ry*ry);
	rx=obj2->x+vx2*d0/vp-obj1->x;
	ry=obj2->y+vy2*d0/vp-obj1->y;
      }

      if(obj1->level>1){
	vx2=obj2->vx-obj1->vx; /* objetive in movement */
	vy2=obj2->vy-obj1->vy;

	d0=sqrt(rx*rx+ry*ry);
	rx=obj2->x+vx2*d0/vp-obj1->x;
	ry=obj2->y+vy2*d0/vp-obj1->y;
	
	d1=sqrt(rx*rx+ry*ry);
	rx=obj2->x+vx2*d1/vp-obj1->x;
	ry=obj2->y+vy2*d1/vp-obj1->y;
      }

      ang0=atan2(ry,rx)-obj1->a;
      if(ang0>0.25)ang0=0.25;
      if(ang0<-0.25)ang0=-0.25;
    }

    for(i=0;i<n;i++){
      //	ang=ang0+0.04*(int)((i+1)/2)*((i%2)*2-1);
      ang=obj1->a+ang0+0.04*(0.5*(1-n)+i);
      obj=NewObj(lhobjs,PROJECTILE,obj1->weapon->projectile.type,
		 obj1->x+r*cos(ang),obj1->y+r*sin(ang),
		 vp*cos(ang)+obj1->vx,vp*sin(ang)+obj1->vy,
		 CANNON0,ENGINE0,obj1->player,obj1,obj1->in);

      if(obj!=NULL){
	if(obj1->habitat==H_PLANET){
	  obj->y+=obj1->radio;
	  obj->y0+=obj1->radio;
	}
      }
      Add2ObjList(lhobjs,obj);
      if(sw==0){  /* if there are no enemies dont send */
	obj->modified=SENDOBJNOTSEND;
      }
      
      obj1->weapon->n--;
      if(obj1->weapon->n<0)obj1->weapon->n=0;
    }
    
    break;
  case CANNON7: //HERE, not finished, not used
    if(obj2==NULL){
      printf("Warning: FireCannon() obj2=NULL\n");
      break;
    }

    rx=obj2->x-obj1->x;
    ry=obj2->y-obj1->y;
    ang=atan2(ry,rx);
    vx1=vp*cos(ang); /* shot velocity */
    vy1=vp*sin(ang);
    
    if(vx1!=0){
      x2=obj2->x+obj2->vx*(obj2->x-obj1->x)/vx1;
      y2=obj2->y+obj2->vy*(obj2->y-obj1->y)/vy1;
    }
    else{
      x2=obj2->x;
      y2=obj2->y;
    }
    /*    printf("x2,y2: %f,%f\n",x2,y2); */
    rx=x2-obj1->x;
    ry=y2-obj1->y;

    ang0=obj1->a-atan2(ry,rx);
    if(ang0>0.25)ang0=0.25;
    if(ang0<-0.25)ang0=-0.25;

    vx1=vp*cos(ang0);
    vy1=vp*sin(ang0);

    for(i=0;i<n;i++){
      //ang=ang0+0.04*(int)((i+1)/2)*((i%2)*2-1);
      ang=obj1->a+ang0+0.04*(0.5*(1-n)+i);
      obj=NewObj(lhobjs,PROJECTILE,obj1->weapon->projectile.type,
	     obj1->x+r*cos(ang),obj1->y+r*sin(ang),
	     vp*cos(ang),vp*sin(ang),
		 CANNON0,ENGINE0,obj1->player,obj1,obj1->in);
      if(obj!=NULL){
	if(obj1->habitat==H_PLANET){
	  obj->y+=obj1->radio;
	  obj->y0+=obj1->radio;
	}
      }

      Add2ObjList(lhobjs,obj);
      if(sw==0){  /* if there are no enemies dont send */
	obj->modified=SENDOBJNOTSEND;
      }

      obj1->weapon->n--;
      if(obj1->weapon->n<0)obj1->weapon->n=0;
    }
    
    break;
  case CANNON8:  /* missile */
    ang=obj1->a;
    obj=NewObj(lhobjs,PROJECTILE,obj1->weapon->projectile.type,
	     obj1->x+r*cos(ang),obj1->y+r*sin(ang),
	   vp*cos(ang)+obj1->vx,vp*sin(ang)+obj1->vy,
	       CANNON0,ENGINE2,obj1->player,obj1,obj1->in);
    if(obj!=NULL){
      if(obj1->habitat==H_PLANET){
	obj->y+=obj1->radio;
	obj->y0+=obj1->radio;
      }
    }

    Add2ObjList(lhobjs,obj);
    if(sw==0){  /* if there are no enemies dont send */
      obj->modified=SENDOBJNOTSEND;
    }

    obj->a=obj1->a;
    obj1->weapon->n--;
    if(obj1->weapon->n<0)obj1->weapon->n=0;
    break;
  case CANNON9: /* laser */
    ang=obj1->a;
    obj=NewObj(lhobjs,PROJECTILE,obj1->weapon->projectile.type,
	       obj1->x+r*cos(ang),obj1->y+r*sin(ang),
	       vp*cos(ang)+obj1->vx,vp*sin(ang)+obj1->vy,
	       CANNON0,ENGINE0,obj1->player,obj1,obj1->in);
    if(obj!=NULL){
      if(obj1->habitat==H_PLANET){
	obj->y+=obj1->radio;
	obj->y0+=obj1->radio;
      }
    }

    Add2ObjList(lhobjs,obj);
    if(sw==0){  /* if there are no enemies dont send */
      obj->modified=SENDOBJNOTSEND;
    }

    obj1->weapon->n--;
    obj->a=obj1->a;
    break;

  default:
    fprintf(stderr,"ERROR FireCannon()\n");
    break;
  }
  if(GameParametres(GET,GNET,0)==TRUE){
    if(GetProc()==players[obj1->player].proc){
      SetModified(obj1,SENDOBJMOD);
    }
  }

  /* sound */
  //  Play(obj1,FIRE0,1);
  return(0);
}
void Play(Object *obj,int sid,float vol){
  /*
    play the sound identified by sid at volume vol.
   */
  static int enabled=1;

  if(enabled==0)return;
  if(obj==NULL && sid==-1 && vol==0){
    printf("sound disabled\n");
    enabled=0;
    return;
  }
  else{
    enabled=1;
  }

#if SOUND
  if(cv==NULL)return;
  //  if(keys.p==TRUE)return;
  if(cv==obj){
    PlaySound(sid,0,vol);
  }
  else{
    if(cv->habitat==H_PLANET){
      if(cv->in==obj->in){
	PlaySound(sid,0,0.5*vol);
      }
    }
  }
#endif
  return;
}

/* orders */ 
void DelAllOrder(Object *obj){
  /*
    Delete and free all the orders of the object *obj 
  */
  while(obj->lorder!=NULL){
    DelFirstOrder(obj);
  }
  obj->norder=0;
  obj->lorder=NULL;
  obj->actorder.id=-1;
}


void DelFirstOrder(Object *obj){
  /* 
     Delete the first order from the list of orders.
  */

  struct ListOrder *lo;
  
  if(obj->lorder==NULL)return;
  
  lo=obj->lorder;
  obj->lorder=obj->lorder->next;

  obj->norder--;

  if(obj->norder<0){
    fprintf(stderr,"ERROR: DelFirstOrder()\n");
  }
  free(lo);
  g_memused-=sizeof(struct ListOrder);
  lo=NULL;
}



struct Order *ReadOrder(struct Order *order0,Object *obj,int mode){
  /* 
     Reads the order of maximun priority deleting it 
     from the list.
     if mode is MAINORD the order is not deleted from the list
     Returns:
     a pointer to that order, 
     NULL if the list is empty.
     If *order is not NULL copy in that address the order.
     If there is not a valid order and *order0!=NULL then order0->id=-1

  */

  int pr;
  struct ListOrder *lo,*lop,*lom,*lopm,*freelo;
  struct Order *order=NULL;

  if(obj->lorder==NULL){
    if(order0 != NULL){
      order0->id=-1;
    }
    return(NULL);
  }

  lo=obj->lorder; /* header list */
  order=NULL;
  freelo=NULL;
  
  switch(mode){
  case MAXPRIORITY:
      
    lom=NULL;
    pr=-1;
    lop=NULL;
    lopm=NULL;
    
    while(lo!=NULL){
      if(lo->order.priority >= pr){ /* read the first of max priority */
	lopm=lop;
	lom=lo;
	pr=lo->order.priority;
      }
      lop=lo;
      lo=lo->next;
    }
    if(pr<5){
      if(order0 != NULL){
	order0->id=-1;
      }
      return(NULL);
    }
    /* deleting the order from the list */
    if(lopm==NULL){ /* first of the list */
      obj->lorder=obj->lorder->next;
    }    
    else{
      lopm->next=lopm->next->next;
    }
    obj->norder--;
    if(obj->norder<0){
      fprintf(stderr,"ERROR: ReadOrder()\n");
    }
    order=&(lom->order);
    freelo=lom;
    break;
  case MAINORD:  

    while(lo!=NULL){

      if(lo->order.priority <5){   /* SegFault al load continuo */
	order=&(lo->order);
	break;
      }
      lo=lo->next;
    }
    break;
  case FIRSTORD:
    if(lo!=NULL){
      lom=obj->lorder;
      order=&(obj->lorder->order);
      obj->lorder=obj->lorder->next;
      /*HERE      free(lom); */
     }
    break;

  default:
    fprintf(stderr,"ERROR ReadOrder()\n");
    exit(-1);
    break;
  }

  if(order0 != NULL && order != NULL){
    memcpy(order0,order,sizeof(struct Order));
  }

  if(order0 != NULL && order == NULL){
    order0->id=-1;
  }

  if(freelo != NULL){
    free(freelo);
    g_memused-=sizeof(struct ListOrder);
    freelo=NULL;
    return(NULL);
  }
  return(order);
}


int ReadMaxPriority(Object *obj){
  /*
    returns:
    the max priority order of object obj
    -1 if the list of orders are empty.
   */
  int pr;
  struct ListOrder *lo;
  

  pr=-1;
  if(obj->lorder==NULL)return(pr);
  
  lo=obj->lorder;
  while(lo!=NULL){
    if(lo->order.priority > pr){
      pr=lo->order.priority;
    }
    lo=lo->next;
  }
  return(pr);
}


int AddOrder(Object *obj,struct Order *order){
  /*
    Add an order to the object *obj
    returns:
    0
  */  
  struct ListOrder *lord;

  lord=malloc(sizeof(struct ListOrder));
  if(lord==NULL){
    fprintf(stderr,"ERROR in malloc AddOrder()3\n");
    exit(-1);
  }
  g_memused+=sizeof(struct ListOrder);


  lord->order.priority=order->priority;
  lord->order.id=order->id;
  lord->order.time=order->time;
  lord->order.g_time=order->g_time;
  lord->order.a=order->a;
  lord->order.b=order->b;
  lord->order.c=order->c;
  lord->order.d=order->d;
  lord->order.e=order->e;
  lord->order.f=order->f;
  lord->order.g=order->g;
  lord->order.h=order->h;
  lord->next=obj->lorder;

  obj->lorder=lord;
  obj->norder++;

  /* checking norders */

  if(obj->norder>10)printf("ship %d has %d orders\n",obj->id,obj->norder);
#if DEBUG
  if(debugai && obj==cv){
    printf("AddOrder()\n");
    PrintOrder(order);
    printf("------------\n");
    PrintOrder(&(obj->lorder->order));
    printf("------------\n");
  }
#endif
  return(0);
}

int CountOrder(Object *obj){
  /* 
     Returns the number of pending orders of the object *obj
   */

  int n=0;
  struct ListOrder *lo;

  lo=obj->lorder;
  while(lo!=NULL){
    n++;
    lo=lo->next;
  }
  return(n);
}

void PrintOrder(struct Order *ord){
  /*
    Print the order in std out. Only for debug.
   */

  if(ord==NULL){
    printf("NULL\n");
    return;
  }

  printf("Priority: %d\n",ord->priority);
  printf("id: %d\n",ord->id);
  printf("time: %d\n",ord->time);
  printf("g_time: %d\n",ord->g_time);
  printf("a:%g b:%g c:%g d:%g\n",ord->a,ord->b,ord->c,ord->d);
  printf("e:%g f:%g g:%g h:%g\n",ord->e,ord->f,ord->g,ord->h);

}

void TestOrder(Object *obj){
  /*
    debug only
   */
  int i;
  struct Order order;


  for(i=0;i<10;i++){
    order.priority=10-i;
    order.id=i;
    order.time=3;
    order.g_time=GetTime();
    order.a=0;
    order.b=0;
    order.c=0;
    order.d=0;
    order.e=0;
    order.f=0;
    order.g=0;
    order.h=i;
    AddOrder(obj,&order);
    printf("TEST %d\n",i);
    PrintOrder(&(obj->lorder->order));
  }

  printf("Number of orders: %d\n",CountOrder(obj));

  for(i=0;i<10;i++){
    /*    struct ListOrder *ReadMaxOrder(Object *obj){ */
      
    ReadOrder(&order,obj,MAXPRIORITY);


    /*    obj->actorder=NULL; */
    /*    obj->actorder=ReadOrder(obj,FIRSTORD); */
    /*    printf("actorder p:%p\n",obj->actorder); */
    printf("------------\n");
    if(order.id!=-1){
      PrintOrder(&order);
    }
    else{
      printf("NULL\n");
    }
    printf("------------\n");
    /*    PrintOrder(&(obj->lorder->order)); */
    printf("Number of orders: %d\n",CountOrder(obj));
  }
}

void CreatePirates(struct HeadObjList *lhobjs,int n, float x0,float y0){
  /*
    Create some pirates
   */
  Object *obj; 
  float x,y;
  int i;
  int stype=SHIP0;

  if(GameParametres(GET,GPIRATES,0)==FALSE)return;

  for(i=0;i<n;i++){
    x=1024*Random(-1);
    y=1024*Random(-1);
    
    if(i==0)stype=EXPLORER;
    else
      stype=FIGHTER;

    obj=NewObj(lhobjs,SHIP,stype,
	       x0+x,y0+y,0,0,
	       CANNON3,ENGINE3,GameParametres(GET,GNPLAYERS,0)+1,NULL,NULL);
    obj->a=PI/2;
    obj->player=GameParametres(GET,GNPLAYERS,0)+1;
    obj->ai=1;
    obj->planet=NULL;
    obj->habitat=H_SPACE;
    obj->weapon=&obj->weapon0;
    obj->weapon->n=obj->weapon->max_n;
    Add2ObjList(lhobjs,obj);
    players[obj->player].nbuildships++;
  }
}

void CreateAsteroids(struct HeadObjList *lhobjs,int n, float x0,float y0){
  /*
    Create some Asteroids
  */
  Object *obj; 
  float a,x,y,vx,vy;
  int i,size;
  
  vx=5.0-10.0*rand()/RAND_MAX;
  vy=5.0-10.0*rand()/RAND_MAX;
  for(i=0;i<n;i++){
    a=i*PI/3;
    x=400.0*rand()/RAND_MAX;
    y=400.0*rand()/RAND_MAX;
    size=(int)(3.0*rand()/RAND_MAX);
    obj=NewObj(lhobjs,ASTEROID,size+1,
	       x0+x,y+y0,vx,vy,
	       CANNON0,ENGINE0,0,NULL,NULL);
    
    //      obj->player=2;
    obj->ai=0;
    obj->in=NULL;
    obj->habitat=H_SPACE;
    obj->mode=NAV;
    Add2ObjList(lhobjs,obj);
  }  
  
}
void GetInformation(struct Player *p1,struct Player *p2,Object *obj){
  /*
    player 1 get some information from player 2
  */
  struct IntList *ks;
  char text[MAXTEXTLEN];  

  if(p1==NULL||p2==NULL)return;
  if(obj==NULL)return;

  ks=p2->kplanets;
  while(ks!=NULL){

    if(p1->team==p2->team){
      /* if are allies */ 
      if(!IsInIntList(p1->kplanets,ks->id)){
	p1->kplanets=Add2IntList(p1->kplanets,ks->id);
	}
    }
    else{
      /* if are enemies */ 
      if((2./3*GameParametres(GET,GNPLANETS,0))*((float)rand()/RAND_MAX)<1){
	if(!IsInIntList(p1->kplanets,ks->id)){
	  p1->kplanets=Add2IntList(p1->kplanets,ks->id);
	  snprintf(text,MAXTEXTLEN,"Received info from enemy");
	  /*	  fprintf(stdout,"Received info from enemy"); */
	  Add2TextMessageList(&listheadtext,text,obj->id,obj->player,0,100,0);
	}
      }
    }
    ks=ks->next;
  }

  /* player 2 give information to player 1 */
  if(p1->team==p2->team){
/* if are allies */ 

    ks=p1->kplanets;
    while(ks!=NULL){
      if(!IsInIntList(p2->kplanets,ks->id)){
	p2->kplanets=Add2IntList(p2->kplanets,ks->id);
      }
      ks=ks->next;
    }
  }
  return;
}


int AreEnemy(struct HeadObjList *lh,int p,Object *obj0){
  /*
    version 0.1
    check if there are a near ship belonging to another proccessor  
    only look for SHIPS
    return:
    > 0  if there are an object nearer than  belonges to another proccessor.
      0 if there are no near enemies.
      1 if its closer than 4 radar range
      2 if its closer than 2 radar range
      3 if its closer than 1 radar range
      4 if its closer than 0.5 radar range
    
  */

  struct ObjList *ls;
  float rx,ry,r2;
  float x0,y0,x1,y1,d2;
  Object *obj1=NULL;
  int sw=0;


  if(obj0==NULL)return(0);
  
  if(obj0->habitat!=H_PLANET){
    x0=obj0->x;
    y0=obj0->y;
  }
  else{
    if(obj0->in==NULL){
      fprintf(stderr,"ERROR 1 in AreEnemy id: %d\n",obj0->id);
      return(4);
    }
    x0=obj0->in->x;
    y0=obj0->in->y;
  }
  ls=lh->next;

  while(ls!=NULL){
    obj1=ls->obj;
    if(players[obj1->player].proc==p){ls=ls->next;continue;}
    if(obj1->type!=SHIP && obj1->type!=ASTEROID){ls=ls->next;continue;}

    if(obj1->habitat!=H_PLANET){
      x1=obj1->x;
      y1=obj1->y;
    }
    else{
      if(obj1->in==NULL){
	fprintf(stderr,"ERROR 2 in AreEnemy id: %d\n",obj1->id);
	ls=ls->next;continue;
      }
      x1=obj1->in->x;
      y1=obj1->in->y;
    }

    rx=x0 - x1;
    ry=y0 - y1;
    r2=rx*rx+ry*ry;
    d2=obj1->radar*obj1->radar;

    if(r2>16*d2){ /* 4 radar */
      ls=ls->next;continue;
    }
    if(r2>9*d2){ /* 3 radar */
      if(sw<1){sw=1;}
      ls=ls->next;continue;
    }
    if(r2>2.25*d2){  /* 1.5 radar   */ 
      if(sw<2){sw=2;}
      ls=ls->next;continue;
    }
    if(r2>810000){  /* 900p. */ 
      if(sw<3){sw=3;}
      ls=ls->next;continue;
    }
    return(4);
  }
  return(sw);
}



void CalcCCInfo(struct HeadObjList *lhobjs,struct HeadObjList *lhkplanets,int player,struct CCDATA *ccdata){

  /*
    version 01
    build the ccdata list;
    calc HERE more info
   */

  struct ObjList *ls;
  Object *obj,*planet;
  struct PlanetInfo *pinfo;

  pinfo=NULL;

  ccdata->nkplanets=0;
  ccdata->nplanets=0;
  ccdata->ninexplore=0;
  ccdata->nenemy=0;
  
  ccdata->nexplorer=0;
  ccdata->nfighter=0;
  ccdata->ntower=0;
  ccdata->ncargo=0;


  ls=lhkplanets->next;

  /* updating pinfo list  */
  while(ls!=NULL){ /* known planets */
    //    printf("%d ",ls->obj->id);
    planet=ls->obj;

    if(!IsInCCList(ccdata,planet)){
      AddNewPlanet2CCData(ccdata,planet);
      //      printf("planet addded\n");
    }

    ccdata->nkplanets++;

    if(ccdata->player==planet->player)ccdata->nplanets++;
    if(planet->player==0)ccdata->ninexplore++;
    else{
      if(players[planet->player].team!=players[player].team){
	ccdata->nenemy++;
      }
    }
    ResetPlanetCCInfo(ccdata,planet);
    ls=ls->next;
  }


  ls=lhobjs->next;
  while(ls!=NULL){
    obj=ls->obj;

    /* only add ally ships */
    if(players[obj->player].team!=players[player].team){ls=ls->next;continue;}

    if(obj->type!=SHIP){ls=ls->next;continue;}
    //    if(obj->mode!=LANDED){ls=ls->next;continue;}
    switch(obj->subtype){
      case EXPLORER:
	if(obj->player==player)ccdata->nexplorer++;
	break;
      case FIGHTER:
	  if(obj->player==player)ccdata->nfighter++;
	break;
     case TOWER:
	if(obj->player==player)ccdata->ntower++;
	break;
      case QUEEN:
	if(obj->player==player)ccdata->ncargo++;
	break;
      default:
	break;
    }
    AddobjCCData(ccdata,obj);
    ls=ls->next;
  }
}

int AddobjCCData(struct CCDATA *ccdata,Object *obj){
  /*
    add obj info  to ccdata structure:
    count the number of ships and their type in each planet
    count the assigned ships
    returns:
    1 if info added. 
    0 in nothing added.
   */

  struct PlanetInfo *pinfo;
  struct Order *order;
  int ret=0;
  int ordersw=0;

  if(obj==NULL)return(0);
  if(ccdata==NULL)return(0);
  order=ReadOrder(NULL,obj,MAINORD);
  if(order!=NULL){
    if(order->id==GOTO)ordersw=1;
  }

  pinfo=ccdata->planetinfo;
  while(pinfo!=NULL){

    if(obj->habitat==H_PLANET){
      if(pinfo->planet->id==obj->in->id){
	float strength;

	if(ccdata->player==obj->player){
	  strength=(obj->level+1)*(obj->state)/100*(obj->state>75)*(obj->gas>=.80*obj->gas_max)*(obj->weapon0.n>.8*obj->weapon0.max_n)*(obj->mode==LANDED);

	  if(obj->subtype!=TOWER){
	    pinfo->strengtha+=strength*(obj->state>95)*(obj->gas>.98*obj->gas_max)*(obj->weapon0.n>=.95*obj->weapon0.max_n);
	  }
	}
	else{ /* are enemies */
	  strength=(obj->level+1);
	}

	pinfo->strength+=strength;

	switch(obj->subtype){
	case EXPLORER:
	  pinfo->nexplorer++;
	  break;
	case FIGHTER:
	  pinfo->nfighter++;
	  break;
	case TOWER:
	  pinfo->ntower++;
	  break;
	case QUEEN:
	  pinfo->ncargo++;
	  break;
	default:
	  break;
	}

  	ret=1;
	if(ordersw==2)return(1);
      }
    }

    /* assigned */

    if(ordersw==1 && pinfo->planet->id==(int)order->c){
	pinfo->nassigned++;
	if(ret==1)return(1);
	ordersw=2;
    }
    
    /* --assigned */
    pinfo=pinfo->next;
  }
  return(ret);
}

void CalcCCPlanetStrength(int player,struct CCDATA *ccdata){
  /*
    Calc the planet with less towers
    Calc the planet with less strength.
    Only planets with one or more ships are included.(planets that can build).
   */

  struct PlanetInfo *pinfo,*pinfolow;
  int ntowers=0;
  int strength=0;
  int sw=0;

  if(ccdata->planetinfo==NULL){
    /* there are no known planets */
    return;
  }


  ccdata->planetlowdefense=NULL;
  ccdata->planetweak=NULL;
  pinfolow=NULL;
  pinfo=ccdata->planetinfo;

  while(pinfo!=NULL){ /* known planets */
    if(pinfo->planet->player != player){pinfo=pinfo->next;continue;}


    if(pinfo->ntower+pinfo->nfighter+pinfo->nexplorer+pinfo->ncargo>0){
      if(sw==0){
	ntowers=pinfo->ntower;
	ccdata->planetlowdefense=pinfo->planet;
	strength=pinfo->strength;
	ccdata->planetweak=pinfo->planet;
	pinfolow=pinfo;
	sw++;
      }
      else{
	if(pinfo->ntower < ntowers){
	  ntowers=pinfo->ntower;
	  ccdata->planetlowdefense=pinfo->planet;
	  pinfolow=pinfo;
	}
	if(pinfo->strength<strength){
	  strength=pinfo->strength;
	  ccdata->planetweak=pinfo->planet;
	}
      }
    }

    pinfo=pinfo->next;
  }

#if DEBUG
  if(debugwar){
    printf("CalcCCPlanetStrength(): player:%d ",player);
    if(ccdata->planetlowdefense!=NULL){
      printf("plow: %d ntower %d ",ccdata->planetlowdefense->id,pinfolow->ntower);
    }
    else{
      printf("plow: NULL ");
    }
    if(ccdata->planetweak!=NULL){
      printf("pwaek: %d ",ccdata->planetweak->id);
    }
    else{
      printf("pwaek: NULL ");
    }
    printf("\n");
  }
#endif
}

int GetCCPlanetInfo(struct CCDATA *ccdata,int pid,int info){
  /*HERE not used */
  struct PlanetInfo *pinfo;

  pinfo=ccdata->planetinfo;
  while(pinfo!=NULL){
    if(pinfo->planet->id!=pid){pinfo=pinfo->next;continue;}

    switch(info){
    case CCDATAPRINT:
      printf("planet: %d ne:%d nf:%d nt:%d\n",
	     pinfo->planet->id,pinfo->nexplorer,pinfo->nfighter,pinfo->ntower);
      break;
    case CCDATANSHIPS:
      return(pinfo->nexplorer+pinfo->nfighter+pinfo->ntower+pinfo->ncargo);
      break;
    case CCDATANEXPLORER:
      return(pinfo->nexplorer);
      break;
    case CCDATANFIGHTER:
      return(pinfo->nfighter);
      break;
    case CCDATANTOWER:
      return(pinfo->ntower);
      break;
    case CCDATANQUEEN:
      return(pinfo->ncargo);
      break;
    default:
      break;
    }
    pinfo=pinfo->next;
  }
  return(0);
}


void PrintCCPlanetInfo(struct CCDATA *ccdata){
  struct PlanetInfo *pinfo;

  pinfo=ccdata->planetinfo;
  while(pinfo!=NULL){

    printf("planet: %d ne:%d nf:%d nt:%d na:%d\n",
	   pinfo->planet->id,pinfo->nexplorer,pinfo->nfighter,pinfo->ntower,pinfo->nassigned);

    pinfo=pinfo->next;
  }
}


void DestroyCCPlanetInfo(struct CCDATA *ccdata){
  struct PlanetInfo *pinfo,*pinfo0;

  if(ccdata->planetinfo==NULL){
#if DEBUG
    if(debug){
      printf("DestroyPlanetInfo(): planetinfo=NULL\n");
    }
#endif
    return;
  }
  pinfo=ccdata->planetinfo;
  while(pinfo!=NULL){
    pinfo0=pinfo;
    pinfo=pinfo->next;
    free(pinfo0);
    pinfo0=NULL;
    g_memused-=sizeof(struct PlanetInfo);
  }
  ccdata->planetinfo=NULL;
}


int NearestCCPlanets(struct CCDATA *ccdata,Object *obj,int status,struct NearObject *objs){
  /*
    Add to *objs the nearest planet found with status status of type type.
    returns:
    the number of objets added to *objs.
  */

  float rx,ry,r2;
  float x0,y0,x1,y1;
  int player;
  int sw1p;
  int m=0;


  int nships;
  int sw=0;

  struct PlanetInfo *pinfo;


  if(obj==NULL)return(0);

  if(obj->habitat==H_PLANET)sw1p=TRUE;


  objs->obj=NULL;
  objs->d2=-1;

  player=ccdata->player;

  pinfo=ccdata->planetinfo;
  while(pinfo!=NULL){

    switch(status){
    case PENEMY:
      if(pinfo->planet->player==player){pinfo=pinfo->next;continue;}
      if(players[pinfo->planet->player].team==players[player].team){pinfo=pinfo->next;continue;}
      break;
    case PINEXPLORE:
      /* include also empty planets */
      if(pinfo->planet->player!=0 && pinfo->planet->player!=player){pinfo=pinfo->next;continue;}
      nships=pinfo->nexplorer+pinfo->nfighter+pinfo->ntower+pinfo->ncargo;
      if(pinfo->planet->player==player && nships>0){pinfo=pinfo->next;continue;}
      /* only send one ship */
      if(pinfo->nassigned>0){pinfo=pinfo->next;continue;} 
      break;
    case PALLY:
      if(players[pinfo->planet->player].team!=players[player].team){pinfo=pinfo->next;continue;}
      if(sw1p){ 
	if(pinfo->planet->id==obj->in->id){pinfo=pinfo->next;continue;}
      }
      break;
    default:
      break;
    }

    x0=obj->x;
    y0=obj->y;
    
    x1=pinfo->planet->x;
    y1=pinfo->planet->y;
    
    if(sw1p){
      x0=obj->in->x;
      y0=obj->in->y;
    }

    rx=x0 - x1;
    ry=y0 - y1;
    r2=rx*rx+ry*ry;
    
    if(sw==0){ /* first element */
      objs->obj=pinfo->planet;
      objs->d2=r2;
      m++,
      sw++;
    }
    else{
      if(r2<objs->d2){
	objs->obj=pinfo->planet;
	objs->d2=r2;
	m++;
      }
    }
    pinfo=pinfo->next;
    
  }
  return(m);
  
}


int WarCCPlanets(struct Player player,struct CCDATA *ccdata){
  /*
    version 01 22Ene2011
    Calc planet to attack and from which
    copy the info in ccdata.
    return:
    0 if no planet are found.
    1 if there planets.
   */
 
  struct PlanetInfo *pinfo,*pinfo1;
  Object *planet;
  int sw=0;
  int ret=0;
  float d2=0,d2min=0;
  int team0,team1,team2;
  int strategy=PLAYERSTRATDEFAULT;
  float strength;
  int rval;


  pinfo=ccdata->planetinfo;
  pinfo1=NULL;
  team0=players[ccdata->player].team;

  ccdata->planet2attack=NULL;
  ccdata->planet2meet=NULL;
  
  strategy=player.strategy;

  rval=100*Random(-1);
  if(rval<30){
    strategy=PLAYERSTRATDEFAULT;
  }
  else{
    if(rval<45){
      strategy=PLAYERSTRATRANDOM;
    }
    else{
      strategy=PLAYERSTRATSTRONG;
    }
  }

  switch(strategy){
  case PLAYERSTRATRANDOM: /*  to a random enemy planet from nearest */

    /* choose a random enemy planet */
    sw=0;
    pinfo1=NULL;
    ccdata->planet2attack=NULL;
    ccdata->planet2meet=NULL;
    pinfo=ccdata->planetinfo;
    while(pinfo!=NULL){
      if(pinfo->planet->player==0){pinfo=pinfo->next;continue;}
      team1=players[pinfo->planet->player].team;
      if(team1==team0){pinfo=pinfo->next;continue;}
      if(sw==0){ /* first time */
	ccdata->planet2attack=pinfo->planet;
	sw=1;
	pinfo=pinfo->next;continue;
      }
      if(100*Random(-1)<20){
	/* changing planet to attack */
	ccdata->planet2attack=pinfo->planet;
      }      
      pinfo=pinfo->next;
    }

    /* looking for the nearest */

    ccdata->planet2meet=GetNearPlanet(ccdata,ccdata->planet2attack,PALLY);
    break;
    
  case PLAYERSTRATDEFAULT: /* from nearest to nearest*/
    sw=0;
    ret=0;
    pinfo=ccdata->planetinfo;
    while(pinfo!=NULL){ /* double loop */
      
      if(pinfo->planet->player==0){pinfo=pinfo->next;continue;}
      team1=players[pinfo->planet->player].team;
      
      pinfo1=pinfo->next;
      while(pinfo1!=NULL){
	if(pinfo1->planet->player==0){pinfo1=pinfo1->next;continue;}
	if(pinfo->planet->player==pinfo1->planet->player){pinfo1=pinfo1->next;continue;}
	
	team2=players[pinfo1->planet->player].team;
	
	if(team1==team2){pinfo1=pinfo1->next;continue;}
	
	if(team1!=team0 && team2!=team0){pinfo1=pinfo1->next;continue;}
	
	if(sw==0){ /* first time */
	  d2min=(pinfo->planet->x - pinfo1->planet->x)*(pinfo->planet->x - pinfo1->planet->x) +
	    (pinfo->planet->y - pinfo1->planet->y)*(pinfo->planet->y - pinfo1->planet->y);
	  if(team0==team1){
	    ccdata->planet2meet=pinfo->planet;
	    ccdata->planet2attack=pinfo1->planet;
	    ret=1;
	  }
	  else{
	    ccdata->planet2meet=pinfo1->planet;
	    ccdata->planet2attack=pinfo->planet;
	    ret=1;
	  }
	  sw++;
	  pinfo1=pinfo1->next;continue;
	}
	
	d2=(pinfo->planet->x - pinfo1->planet->x)*(pinfo->planet->x - pinfo1->planet->x) +
	  (pinfo->planet->y - pinfo1->planet->y)*(pinfo->planet->y - pinfo1->planet->y);      
	
	if(d2<d2min){
	  d2min=d2;
	  if(team0==team1){
	    ccdata->planet2meet=pinfo->planet;
	    ccdata->planet2attack=pinfo1->planet;
	    ret=1;
	  }
	  else{
	    ccdata->planet2meet=pinfo1->planet;
	    ccdata->planet2attack=pinfo->planet;
	    ret=1;
	  }
	}
	//break;
	pinfo1=pinfo1->next;
      }
      pinfo=pinfo->next;
    }
    break;

  case PLAYERSTRATSTRONG:/* from strongest to nearest */
    /* looking the strongest planet */

    sw=0;
    pinfo1=NULL;
    planet=NULL;
    ccdata->planet2attack=NULL;
    ccdata->planet2meet=NULL;
    pinfo=ccdata->planetinfo;
    strength=0;
    while(pinfo!=NULL){
      if(pinfo->planet->player==0){pinfo=pinfo->next;continue;}
      if(player.id!=pinfo->planet->player){pinfo=pinfo->next;continue;}

      if(sw==0){ /* first time */
	planet=pinfo->planet;
	strength=pinfo->strengtha;
	sw=1;
	pinfo=pinfo->next;continue;
      }

      if(pinfo->strengtha>strength){
	planet=pinfo->planet;
	strength=pinfo->strengtha;
      }
      else{
	if(pinfo->strengtha==strength){
	  if(100*Random(-1)<20){ /* HERE must depend on number of planets*/
	    planet=pinfo->planet;
	  }
	}
      }

      pinfo=pinfo->next;
    }

    /* looking the nearest enemy planet */
    ccdata->planet2attack=GetNearPlanet(ccdata,planet,PENEMY);

    /* looking for the nearest ally planet 2 meet */

    ccdata->planet2meet=GetNearPlanet(ccdata,ccdata->planet2attack,PALLY);
    break;
 
  default:
    ret=0;
    break;
  }
  if(ccdata->planet2attack!=NULL && ccdata->planet2meet!=NULL)ret=1;

  if(ccdata->planet2meet==NULL || ccdata->planet2attack==NULL){
#if DEBUG
    if(debugwar){
      printf("(%d)planet2meet or planet2attack NULL\n",ccdata->player);
    }
#endif
    ret=0;
  }
#if DEBUG  
  if(debugwar && ret!=0){
    printf("WARCCPLANET() player: %d p2meet: %d p2attack: %d\n",
	   ccdata->player,ccdata->planet2meet->id,ccdata->planet2attack->id);
  }
#endif

  return(ret);
}



int DecideIfWar(struct Player player,struct CCDATA *ccdata){
  /*
    Decide if go to War
   */

  int ret=0;

  switch(player.profile){
  case PLAYERPROFDEFAULT:
    if(ccdata->nkplanets>5 &&
       ccdata->ntower>10 &&
       ccdata->nfighter>10 &&
       ccdata->ninexplore==0){
      ret=1; /* entering phase 1 */
    }
    if(ccdata->nfighter>10 &&
       ccdata->ninexplore==0 &&
       player.balance<0 && 
       (float)ccdata->nfighter/(float)ccdata->ntower>1){
      ret=1;
    }
    
    if(ccdata->nfighter>30 &&
       (float)ccdata->nfighter/(float)ccdata->ntower>1.5){
      ret=1;
    }
    break;
  case PLAYERPROFPACIFIST:
    ret=0;
    break;
  case PLAYERPROFAGRESSIVE:
    if(ccdata->nkplanets>3 &&
       ccdata->ntower>7 &&
       ccdata->nfighter>10){
      ret=1; /* entering phase 1 */
    }
    if(ccdata->nfighter>10 &&
       player.balance<0 && 
       (float)ccdata->nfighter/(float)ccdata->ntower>1){
      ret=1;
    }
    if(ccdata->nfighter>15 &&
       (float)ccdata->nfighter/(float)ccdata->ntower>1.5){
      ret=1;
    }
    break;

  default:
    ret=0;
    break;
  }
  return(ret);
}


struct PlanetInfo *GetPlanetInfo(struct CCDATA *ccdata,Object *planet){
  /*
    returns the info of planet from CCDATA
  */

  struct PlanetInfo *pinfo;

  if(ccdata==NULL || planet==NULL)return(NULL);

  pinfo=ccdata->planetinfo;
  while(pinfo!=NULL){
    if(pinfo->planet==planet)return(pinfo);
    pinfo=pinfo->next;
  }
  return(NULL);
}


Object *GetNearPlanet(struct CCDATA *ccdata,Object *planet1,int mode){
  /*
    returns in planet2 the nearest planet to planet1 depends on the value mode
    returns:
    0 if not found
    1 if found.

  */

  struct PlanetInfo *pinfo;
  int team1,team2;

  int sw=0;
  Object *planet2;
  float d2=0,d2min=0;



  if(planet1==NULL)return(NULL);
  if(ccdata==NULL)return(NULL);

  planet2=NULL;
  team1=players[ccdata->player].team;
  

  sw=0;
  planet2=NULL;
  pinfo=ccdata->planetinfo;
  while(pinfo!=NULL){
    
    switch(mode){
    case POWN:
      if(pinfo->planet->player==0){pinfo=pinfo->next;continue;}
      if(ccdata->player!=pinfo->planet->player){pinfo=pinfo->next;continue;}
      break;
    case PALLY:
      if(pinfo->planet->player==0){pinfo=pinfo->next;continue;}
      team2=players[pinfo->planet->player].team;
      if(team1!=team2){pinfo=pinfo->next;continue;}
      break;
    case PINEXPLORE:
      break;
    case PENEMY:
      if(pinfo->planet->player==0){pinfo=pinfo->next;continue;}
      team2=players[pinfo->planet->player].team;
      if(team1==team2){pinfo=pinfo->next;continue;}
      break;
    case PUNKNOWN:
      break;
    default:
      break;
    }
    
    
    if(sw==0){ /* first time */
      planet2=pinfo->planet;
      d2min=(pinfo->planet->x - planet1->x)*(pinfo->planet->x - planet1->x) +
	(pinfo->planet->y - planet1->y)*(pinfo->planet->y - planet1->y);
      sw=1;
      pinfo=pinfo->next;continue;
    }
    
    d2=(pinfo->planet->x - planet1->x)*(pinfo->planet->x - planet1->x) +
      (pinfo->planet->y - planet1->y)*(pinfo->planet->y - planet1->y);
    
    
    if(d2<d2min){
      d2min=d2;
      planet2=pinfo->planet;
    }
    pinfo=pinfo->next;
  }

  return(planet2);
}


int AddCCEnemyPlanetInfo(struct CCDATA *ccdata,Object *planet){
  /*
    returns:
    1 if info is succesfully reset 
    0 if not;
  */

  struct PlanetInfo *pinfo;

  if(ccdata==NULL || planet==NULL)return(0);
  return(0);
  pinfo=ccdata->planetinfo;
  while(pinfo!=NULL){
    if(pinfo->planet==planet){
      
      pinfo->time=GetTime();
      pinfo->nassigned=0;
      pinfo->strengtha++;

      return(1);
    }
    pinfo=pinfo->next;
  }
  return(0);
}

int IsInCCList(struct CCDATA *ccdata,Object *planet){
  /*
    returns:
    1 if planet is in ccdata info
    0 if not;
  */

  struct PlanetInfo *pinfo;

  if(ccdata==NULL || planet==NULL)return(0);

  pinfo=ccdata->planetinfo;
  while(pinfo!=NULL){
    if(pinfo->planet==planet)return(1);
    pinfo=pinfo->next;
  }
  return(0);
}

int AddNewPlanet2CCData(struct CCDATA *ccdata,Object *planet){
  /*
    Add a new planet to ccdata list
    planet info is set to default
    returns:
    1 if planet if added
    0 if not;
  */

  struct PlanetInfo *pinfo;

  if(ccdata==NULL || planet==NULL)return(0);

  pinfo=malloc(sizeof(struct PlanetInfo));
  if(pinfo==NULL){
    fprintf(stderr,"ERROR in malloc CalcCCInfo()\n");
    exit(-1);
  }
  pinfo->planet=planet;
  pinfo->time=GetTime();
  pinfo->nexplorer=0;
  pinfo->nfighter=0;
  pinfo->ntower=0;
  pinfo->ncargo=0;
  pinfo->strength=0;
  pinfo->strengtha=0;
  pinfo->nassigned=0;
  
  pinfo->next=ccdata->planetinfo;
  ccdata->planetinfo=pinfo;
    
  g_memused+=sizeof(struct PlanetInfo);
  
  return(1);
}

int AddPlanetInfo2CCData(struct CCDATA *ccdata,struct PlanetInfo *pinfo0){
  /*
    Add a new planet to ccdata list
    planet info from pinfo0
    returns:
    1 if planet if added
    0 if not;
  */

  struct PlanetInfo *pinfo;

  if(ccdata==NULL || pinfo0==NULL)return(0);

  pinfo=malloc(sizeof(struct PlanetInfo));
  if(pinfo==NULL){
    fprintf(stderr,"ERROR in malloc CalcCCInfo()\n");
    exit(-1);
  }
  pinfo->planet=pinfo0->planet;
  pinfo->time=pinfo0->time; 
  pinfo->nexplorer=pinfo0->nexplorer;	 
  pinfo->nfighter=pinfo0->nfighter;	 
  pinfo->ntower=pinfo0->ntower;	 
  pinfo->ncargo=pinfo0->ncargo;	 
  pinfo->strength=pinfo0->strength;	 
  pinfo->strengtha=pinfo0->strengtha;	 
  pinfo->nassigned = pinfo0->nassigned;    
  
  pinfo->next=ccdata->planetinfo;
  ccdata->planetinfo=pinfo;
    
  g_memused+=sizeof(struct PlanetInfo);
  
  return(1);
}

int ResetPlanetCCInfo(struct CCDATA *ccdata,Object *planet){
  /*
    returns:
    1 if info is succesfully reset 
    0 if not;
  */

  struct PlanetInfo *pinfo;

  if(ccdata==NULL || planet==NULL)return(0);

  pinfo=ccdata->planetinfo;
  while(pinfo!=NULL){
    if(pinfo->planet==planet){

      if((players[planet->player].team==players[ccdata->player].team)){
	pinfo->time=GetTime();
	pinfo->nexplorer=0;
	pinfo->nfighter=0;
	pinfo->ntower=0;
	pinfo->ncargo=0;
	pinfo->strength=0;
	pinfo->strengtha=0;
	pinfo->nassigned=0;
      }
      else{
	pinfo->nassigned=0;
      }
      return(1);
    }
    pinfo=pinfo->next;
  }
  return(0);
}


int CalcEnemyPlanetInfo(struct HeadObjList *lhobjs,struct CCDATA *ccdata,Object *obj){
  /*
    returns:
    1 if info is succesfully updated 
    0 if not;
  */
  struct ObjList *ls;
  struct PlanetInfo *pinfo;
  Object *planet=NULL;  

  if(obj==NULL)return(0);
  if(obj->in==NULL)return(0);

  planet=obj->in;

  if(ccdata==NULL || planet==NULL)return(0);

  pinfo=ccdata->planetinfo;
  while(pinfo!=NULL){
    if(pinfo->planet==planet){

      if((players[planet->player].team!=players[ccdata->player].team)){
	if(GetTime()-pinfo->time < 60){
	  /* dont update so often*/
	  return(0);
	}
	pinfo->time=GetTime();
	pinfo->nexplorer=0;
	pinfo->nfighter=0;
	pinfo->ntower=0;
	pinfo->ncargo=0;
	pinfo->strength=0;
	pinfo->strengtha=0;
	pinfo->nassigned=0;
      }
      break;
    }
    pinfo=pinfo->next;
  }

  if(pinfo!=NULL){
    if(pinfo->planet!=planet){
      fprintf(stderr,"Error in CalcEnemyPlanetInfo()\n");/* HERE this never happen, delete*/
      exit(-1);
    }
#if DEBUG
    if(debugwar){
      printf("********************\n");
      printf("Planet %d info: str: %f\n",planet->id,pinfo->strength);
      printf("********************\n");
    }
#endif

    ls=lhobjs->next;
    while(ls!=NULL){
      if(ls->obj->habitat!=H_PLANET){ls=ls->next;continue;}
      if(ls->obj->in!=planet){ls=ls->next;continue;}
      if((players[ls->obj->player].team==players[ccdata->player].team)){ls=ls->next;continue;}
      if(ls->obj->type!=SHIP){ls=ls->next;continue;}

      switch(ls->obj->subtype){
      case EXPLORER:
	pinfo->nexplorer++;
	break;
      case FIGHTER:
	pinfo->nfighter++;
	break;
      case TOWER:
	pinfo->ntower++;
	break;
      case QUEEN:
	pinfo->ncargo++;
	break;
      default:
	break;
      }
      /* HERE save the strength of enemy planets */
      pinfo->strength+=ls->obj->level+1;      
      if(ls->obj->level>1)pinfo->strength++;
      if(ls->obj->level>2)pinfo->strength++;
      if(ls->obj->level>3)pinfo->strength+=4;
      ls=ls->next;
    }
#if DEBUG
    if(debugwar){
      printf("********************\n");
      printf("Planet %d info: str: %f nf: %d (%d,%d)\n",
	     planet->id,pinfo->strength,pinfo->nfighter,
	     players[planet->player].team,players[ccdata->player].team);
      printf("********************\n");
    }
#endif
  }
  return(1);
}

int CountAssignedCCPlanetInfo(struct HeadObjList *lhobjs,struct CCDATA *ccdata,Object *planet){
/* 
   returns the number of ships assigned to different planet to planet  
*/
  struct ObjList *ls;
  Object *obj;
  int n=0;
  struct Order *order;

  if(planet==NULL)return(0);
  if(ccdata==NULL)return(0);
  if(lhobjs==NULL)return(0);
  
  ls=lhobjs->next;
  while(ls!=NULL){
    obj=ls->obj;    
    if(obj->habitat!=H_PLANET){ls=ls->next;continue;}
    if(obj->in!=planet){ls=ls->next;continue;}
    if((players[obj->player].team==players[ccdata->player].team)){ls=ls->next;continue;}
    if(obj->type!=SHIP){ls=ls->next;continue;}
 
    order=ReadOrder(NULL,obj,MAINORD);    
    if(order==NULL){ls=ls->next;continue;}

    if(order->id==GOTO && (int)order->c!=planet->id)n++;
    
    ls=ls->next;
  }
  
  return(n);
}


struct PlanetInfo *War(struct HeadObjList *lhobjs,struct Player player,struct CCDATA *ccdata){
  /*
    returns :
    planet2meet
*/


  struct PlanetInfo *pinfo1,*pinfo2;
  int nf2a=6; /* num of fighter 2 attack */
  static int cont;


  if(player.id==2)cont++;

  pinfo1=NULL;
  pinfo2=NULL;
  /* Decide if WAR */


  if(ccdata->war>=2){
    if(ccdata->planet2meet==NULL || ccdata->planet2attack==NULL){
#if DEBUG
      if(debugwar){
	printf("(1)player %d planet2meet or planet2attack NULL\n",player.id);
      }
#endif
      ccdata->planet2meet=ccdata->planet2attack=NULL;
      ccdata->time=0;
      ccdata->war=0;
      return(NULL);
    }

    if(ccdata->planet2attack->player==player.id){ /* planet conquered */

#if DEBUG
      if(debugwar){
	printf("player %d conquest planet %d\n",player.id,ccdata->planet2attack->id);
      }
#endif

      ccdata->planet2meet=ccdata->planet2attack=NULL;
      ccdata->time=0;
      ccdata->war=0;
      return(NULL);
    }

    if(players[ccdata->planet2meet->player].team != players[player.id].team){ /* origin planet lost */
#if DEBUG
      if(debugwar){
	printf("player: %d planet %d lost (%d)\n",player.id,ccdata->planet2meet->id,ccdata->war);
      }
#endif
      ccdata->planet2meet=ccdata->planet2attack=NULL;
      ccdata->time=0;
      ccdata->war=0;
      return(NULL);
    }
  }

  if(ccdata->war>1){
    pinfo1=GetPlanetInfo(ccdata,ccdata->planet2meet);
    if(pinfo1!=NULL){
      pinfo2=GetPlanetInfo(ccdata,ccdata->planet2attack);
    }
  }
#if DEBUG
  if(ccdata->war && debugwar){
    printf("INWAR player %d war:%d p1 %p p2%p\n",player.id,ccdata->war,pinfo1,pinfo2);
  }
#endif

  switch(ccdata->war){
  case 0:
    ccdata->planet2meet=ccdata->planet2attack=NULL;
    ccdata->war=DecideIfWar(player,ccdata);
    break;
  case 1:

    /* Choose planet to attack and from  */
    if(WarCCPlanets(player,ccdata)==0){
      ccdata->war=0;
      ccdata->planet2meet=ccdata->planet2attack=NULL;
      ccdata->time=0;
      return(NULL);
    }
    else{
      ccdata->war=2; /* entering phase 2 */
      if(ccdata->planet2meet==NULL || ccdata->planet2attack==NULL){
#if DEBUG    
	if(debugwar){
	  printf("(0)player %d planet2meet or planet2attack NULL\n",player.id);
	}
#endif
	ccdata->planet2meet=ccdata->planet2attack=NULL;
	ccdata->time=0;
	ccdata->war=0;
	exit(-1);
      }
    }

    break;
  case 2:
    if(pinfo1==NULL){
      ccdata->planet2meet=ccdata->planet2attack=NULL;
      ccdata->time=0;
      ccdata->war=0;
      return(NULL);
    }


    nf2a=1.5*pinfo2->strength;
    if(nf2a<MINnf2a)nf2a=MINnf2a;
    if(nf2a>MAXnf2a)nf2a=MAXnf2a;

#if DEBUG    
    if(debugwar){
      
      printf("player: %d war: %d time: %d\n",ccdata->player,ccdata->war,GetTime()); 
      printf("\t FROM %d: nf: %d st2a: % f nf2a:%d\n", 
	     ccdata->planet2meet->id,(pinfo1)->nfighter,(pinfo1)->strengtha,nf2a); 
      printf("\t TO %d:nf: %d st: %f nassigned: %d\n", 
	     ccdata->planet2attack->id,pinfo2->nfighter,pinfo2->strength,pinfo2->nassigned); 
    }
#endif    

    if((pinfo1)->strengtha > nf2a){
      /* entering phase 3 */
      ccdata->time2=GetTime();
      ccdata->war=3;
    }
    break;
  case 3:
    if(pinfo1==NULL){
      fprintf(stderr,"ERROR in war: planet2meet NULL war:%d\n",ccdata->war);exit(-1);
    }
    if(pinfo2==NULL){
      ccdata->planet2meet=ccdata->planet2attack=NULL;
      ccdata->time=0;
      ccdata->war=0;
      return(NULL);
    }

    nf2a=1.5*pinfo2->strength;
    if(nf2a<MINnf2a)nf2a=MINnf2a;
    if(nf2a>MAXnf2a)nf2a=MAXnf2a;

#if DEBUG
    if(debugwar){
      printf("player: %d war: %d time: %d\n",ccdata->player,ccdata->war,GetTime());
      //	printf("\t FROM %d: nf: %d st2a: % f nf2a:%d\n",ccdata->planet2meet->id,pinfo1->nfighter,pinfo1->strengtha,nf2a);
      printf("\t TO %d:nf: %d st: %f nassigned: %d\n",ccdata->planet2attack->id,pinfo2->nfighter,pinfo2->strength,pinfo2->nassigned);
    }
#endif    

    if(GetTime()-ccdata->time2>100){ /* attack finished */
      ccdata->planet2meet=ccdata->planet2attack=NULL;

#if DEBUG
      if(debugwar){
	printf("attack finished(3)\n");
      }
#endif

      ccdata->time=0;
      ccdata->war=0;
      return(NULL);
    }

    if(0&&pinfo2->nassigned>=nf2a){ /* entering phase 4 */
      ccdata->war=4;
#if DEBUG
      if(debugwar){
	printf("War() player %d entering phase 4\n",player.id);
      }
#endif
    }

    break;
  case 4:
    if(CountAssignedCCPlanetInfo(lhobjs,ccdata,ccdata->planet2meet)==0){ /* attack finished */
      /* entering phase 0 */
      ccdata->planet2meet=ccdata->planet2attack=NULL;
      pinfo2->strength+=2;
      ccdata->time=0;
      ccdata->war=0;
#if DEBUG
      if(debugwar){
	printf("attack finished(3)\n");
      }
#endif
      return(NULL);
    }

    break;
  default:
    break;
  }

  /* print some DEBUG info */
  if(ccdata->war){
#if DEBUG
    if(debugwar && !(cont%40)){
      printf("player: %d war: %d\n",ccdata->player,ccdata->war);
      if(ccdata->planet2meet!=NULL && ccdata->planet2attack!=NULL){
	printf("\tplanet2meet %d planet2attack %d\n",ccdata->planet2meet->id,
	       ccdata->planet2attack->id);
      }
    }
#endif
  }
  return(pinfo1);
}


int BuyorUpgrade(struct HeadObjList *lhobjs,struct Player player,struct CCDATA *ccdata){
  /* 
     buy or upgrade HERE check it
     return:
     0 if nothing is buy or upgrade
     1 if buy a ship
     2 if a ship is upgrade

*/
  Object *obj;
  Object *obj2up=NULL;
  int price;
  
  int np;
  int planet;
  int ret=0;
  int status;
  int buyid;
  static int buyupgradesw=0; /*0: nothing , 1: buy, 2: upgrade */
  float cut=.5;


  np=(player.nplanets);
  buyid=-1;
  cut=.5;


  /* decide what */   

  if(player.lastaction==0){

    if(player.maxlevel<2){
      cut=0;
    }
    else{
      if((float)ccdata->ntower/np>=2.8){
	if(player.balance<.7*np){
	  cut+=.2*(1-cut);
	}
	else{
	  cut-=.2*(cut);
	}
      }
      
      if((float)ccdata->ntower/np<2){
	cut-=.2*(cut);
      }
      if((float)ccdata->ntower/np<1){
	cut-=.2*(cut);
      }
      if((float)ccdata->nfighter/np<1){
	cut-=.2*(cut);
      }
      if((float)ccdata->nexplorer<5){
	cut-=.2*(cut);
      }
    }
    
    buyupgradesw=0;
    
    if(100*Random(-1)>cut){
      buyupgradesw=1; /* buy */
    }
    else{
      buyupgradesw=2; /* upgrade */
    }

    if(player.gold<700){
      buyupgradesw=0;
    }

    if(ccdata->nkplanets==0 || ccdata->planetlowdefense==NULL || 
       ccdata->planetweak==NULL){
      buyupgradesw=0;
    }

    player.lastaction=buyupgradesw;

  }

  /*--decide what */

  //  printf("BuyorUpgrade():%d\n",player.lastaction);
  switch(player.lastaction){
  case 0:
    break;
  case 1: /* buy ship */
    /*** Buy Ships ***/
    /*
      buy a tower in the planet with less towers.
      buy a fighter in the planet wiht less strength.
    */
    
    planet=-1;
    obj=NULL;
    buyid=CCBuy(ccdata,player,&planet);
    //    printf("BuyorUpgrade():\tCCBUY: %d\n",buyid);
    if(ccdata->war && buyid==FIGHTER && ccdata->planet2meet!=NULL){
      if(player.id==ccdata->planet2meet->player){ 
	planet=ccdata->planet2meet->id;
      }
    }      
    if(planet!=-1 && buyid!= -1){
      obj=ObjFromPlanet(lhobjs,planet,player.id);
      
      if(obj!=NULL && buyid>=0){
	status=BuyShip(player,obj,buyid);
	if(status==SZ_OK){
	  ret=1;
	  player.lastaction=0;
#if DEBUG
	  if(debugshop){
	    printf("player %d buy ship: planet: %d type: %d\n",player.id,obj->in->id,buyid);
	  }
#endif
	  ccdata->time=0;
	}
      }
    }
    
    break;
  case 2: /* upgrade ships */
    
    obj2up=CCUpgrade(lhobjs,&player);
    if(obj2up==NULL){
      player.lastaction=0;
    }	
    else{
      price=GetPrice(obj2up,0,0,0);
      if(price>0){
	if(players[obj2up->player].gold>price){
	  //      cv->level++;
	  players[obj2up->player].gold-=price;
	  Experience(obj2up,(int)(100*pow(2,obj2up->level) - obj2up->experience+1));
	  player.lastaction=0;
	  ret=2;
#if DEBUG
	  if(debugshop){
	    printf("player: %d OBJ UPGRADED to level: %d type: %d id: %d price: %d\n",
		   player.id,obj2up->level,obj2up->subtype,obj2up->pid,price);
	  }
#endif
	}
      }
    }
    break;
  default:
    break;
  }
  return(ret);
}
