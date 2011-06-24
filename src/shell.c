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

#include <string.h>
#include "general.h"
#include "shell.h"
#include "ai.h"
#include "objects.h"
#include "spacecomm.h"
#include "functions.h"
#include "graphics.h"

extern struct Player *players;
extern GdkPixmap *pixmap;
extern GdkGC *penGreen;
extern struct Habitat habitat;
extern struct Keys keys;
extern struct HeadObjList listheadobjs;       /* list of all objects */
extern Point mouse_pos;
extern int fobj[4];
extern struct Draw gdraw;
#if DEBUG
int debugshell=FALSE;
#endif

struct Shell{
  char name[16];
  char menu[128];
  char par[128];
  char options[16];
  int noptions;
  int orders[16];
  int order; 
};

struct Ordername{
  int order;
  char name[12];
};


struct Shell shells[10];
struct Ordername ordernames[15];



void initshell(void){
  /*
    Initialize the shell
   */

  ordernames[0].order=GOTO;
  strncpy(ordernames[0].name,"GOTO",12);
  ordernames[1].order=GOTO;
  strncpy(ordernames[1].name,"SELECT",12);

  strncpy(shells[0].name,"main",16);
  strncpy(shells[0].menu,
	  "G: GOTO   X: EXPLORE   S: SELECT   P: STOP   T: TAKEOFF   R: REPITE   B: BUY   U: UPGRADE   W: WRITE   E: SELL",128);
  strncpy(shells[0].options,"gxsptrbuwe",16);
  shells[0].noptions=10;
  shells[0].orders[0]=GOTO;
  shells[0].orders[1]=EXPLORE;
  shells[0].orders[2]=SELECT;
  shells[0].orders[3]=STOP;
  shells[0].orders[4]=TAKEOFF;
  shells[0].orders[5]=REPITE;
  shells[0].orders[6]=BUY;
  shells[0].orders[7]=WRITE;
  shells[0].orders[8]=SELL;
  shells[0].orders[9]=UPGRADE;

  strcpy(shells[0].par,"");
  shells[0].order=0;

  strncpy(shells[1].name,"goto",16);
  strncpy(shells[1].menu,"GOTO: ",128);
  strcpy(shells[1].par,"");
  shells[1].order=GOTO;

  strncpy(shells[1].name,"explore",16);
  strncpy(shells[1].menu,"EXPLORE",128);
  strcpy(shells[1].par,"");
  shells[1].order=EXPLORE;

  strncpy(shells[3].name,"select",16);
  strncpy(shells[3].menu,"SELECT: ",128);
  strcpy(shells[3].par,"");
  shells[3].order=SELECT;

  strncpy(shells[4].name,"stop",16);
  strncpy(shells[4].menu,"STOP",128);
  strcpy(shells[4].par,"");
  shells[4].order=STOP;

  strncpy(shells[5].name,"takeoff",16);
  strncpy(shells[5].menu,"TAKEOFF",128);
  strcpy(shells[5].par,"");
  shells[5].order=TAKEOFF;

  strncpy(shells[6].name,"repite",16);
  strncpy(shells[6].menu,"REPITE",128);
  strcpy(shells[6].par,"");
  shells[6].order=REPITE;

  strncpy(shells[7].name,"buy",16);
  strncpy(shells[7].menu,"BUY",128);
  strcpy(shells[7].par,"");
  shells[7].order=BUY;

  strncpy(shells[8].name,"write",16);
  strncpy(shells[8].menu,"WRITE",128);
  strcpy(shells[8].par,"");
  shells[8].order=WRITE;

  strncpy(shells[9].name,"upgrade",16);
  strncpy(shells[9].menu,"UPGRADE",128);
  strcpy(shells[9].par,"");
  shells[9].order=UPGRADE;
}

void Shell_02(GdkPixmap *pixmap,GdkFont *font,GdkGC *color,struct HeadObjList lhead,struct Keys *key,Object **pcv){
  /*
    version 02
    
  */
  static GdkGC *gc;
  static char message[128]="";
  static char messagetmp[128]="";
  static char ord[16]="";
  static char par[MAXTEXTLEN]="";
  static char lastpar[16]="";
  static int level=0;
  static int order=0;
  static int nn=0;
  static int lastorder=0;
  int i;
  static int swinit=0;
  static int shellid=0;
  char c,*d;
  int option;
  static int swinput=FALSE;
  Object *cv;

  cv=*pcv;
  if(swinit==0){
    initshell();
    strncpy(message,shells[0].menu,128);
    shellid=0;
    gc=color;
    swinit=1;

  }
  if(cv==NULL){
    printf("There are no ship selected!!\n");
    key->o=FALSE;
    level=0;
    key->esc=FALSE;
    return;
  }
  if(GetPlayerProc(players,cv->player)!=GetProc()){
    fprintf(stderr,"WARNING proc %d %d \n",GetProc(),GetPlayerProc(players,cv->player));
    key->esc=TRUE;
    return;
  }

  if(GetControl(players,cv->player)!=HUMAN){
    fprintf(stderr,"ERROR human  id:%d\n",cv->id);
    exit(-1); /* Never happen this */
  }

  if(key->esc==TRUE){
    key->o=FALSE;
    key->w=FALSE;
    level=0;
    shellid=0;
    key->esc=FALSE;
    return;
  }

  if(key->back==TRUE){
    Keystrokes(DELETELAST,NULL);
    key->back=FALSE;
  }

  if(key->enter==TRUE){
    if(order==REPITE && order !=0){ 
       order=lastorder;
       strcpy(par,"");
       strncpy(par,lastpar,16);
    } 
    if(order==GOTO||order==TAKEOFF)
      key->i=TRUE;

    /*    printf("order: %d par: %s\n",order,par); */
    lastorder=order;
    strcpy(lastpar,"");
    strncpy(lastpar,par,16);

    strcpy(message,"");
    strcpy(ord,"");
    strcpy(par,"");
    strncpy(message,shells[0].menu,128);
    swinput=FALSE;
    shellid=0;
    order=0;
    level=0;
    nn=0;
    key->enter=FALSE;
    key->w=FALSE;
    key->o=FALSE;
  }


  switch(shellid){
  case 0: /* main */

    option=-1;
    Keystrokes(RETURNLAST,&c);
    for(i=0;i<shells[0].noptions;i++){
      d=&shells[0].options[i];
      if(c==*d){
	option=i;
	break;
      }
    }

    if(option!=-1){
      printf("Error option: %d\n",i);
    }
    /*    gsptrbwe */
    switch(option){
    case 0: /* g GOTO */
      Keystrokes(RESET,NULL);
      sprintf(message,"GOTO");
      /*      printf("GOTO\n"); */
      order=GOTO;
      swinput=TRUE;

      break;
    case 1: /* s SELECT */
      Keystrokes(RESET,NULL);
      sprintf(message,"SELECT");
      /*      printf("SELECT\n"); */
      order=SELECT;
      swinput=TRUE;
      
      break;
    case 2: /* p STOP */
      Keystrokes(RESET,NULL);
      sprintf(message,"STOP");
      /*      printf("STOP\n"); */
      order=STOP;
      break;
    case 3: /* t TAKEOFF */
      Keystrokes(RESET,NULL);
      sprintf(message,"TAKEOFF");
      /*      printf("TAKEOFF\n"); */
      order=TAKEOFF;
      break;
    case 4: /* r REPITE */
      break;
    case 5: /* b BUY */
      break;
    case 6: /* w WRITE */
      Keystrokes(RESET,NULL);
      sprintf(message,"WRITE");
      /*      printf("WRITE\n"); */
      order=WRITE;
      swinput=TRUE;
      break;
    case 7: /* e SELL */
      break;
    default:
      break;  
    }

    if(swinput){
      strcpy(par,"");
      Keystrokes(LOAD,par);
    }

    if(key->p==TRUE){
      order=STOP;
      shellid=3;
      strcpy(ord,"STOP");
      key->p=FALSE;
    }
    break;
  case 1: /*goto */
    strcpy(par,""); 
    strcpy(ord,"GOTO: ");
    order=GOTO;
    Keystrokes(LOAD,par);
    strcpy(message,"");
    strcat(message,ord);
    strcat(message,par);
    break;

  case 2: /*select */
    strcpy(par,""); 
    strcpy(ord,"SELECT: ");
    order=SELECT;
    Keystrokes(LOAD,par);
    strcpy(message,"");
    strcat(message,ord);
    strcat(message,par);
    break;

  default:
    break;
  }
  sprintf(messagetmp,"%s %s",message,par);
#if DEBUG
  if(debugshell){
    printf("message: (%s) par: (%s)\n",message,par); 
  }
#endif
  DrawString(pixmap,font,gc,10,GameParametres(GET,GHEIGHT,0)+GameParametres(GET,GPANEL,0)/2+4,
		  messagetmp);
}


int Shell(int command, GdkPixmap *pixmap,GdkFont *font,GdkGC *color,struct HeadObjList *lhead,struct Player *ps,struct Keys *key,Object **pcv){
  /*
    version 02
  */
  static char cad[128]="";
  static char ord[16]="";
  static char par[MAXTEXTLEN]="";
  static char lastpar[MAXTEXTLEN]="";
  static char par0[MAXTEXTLEN]="";
  static int level=0;
  static int order=0;
  static int nn=0;
  static int lastorder=0;
  static Object *cv0=NULL;
  char pr1[12];
  char pr2[12];
  char pr3[12];

  int i;
  int player=1;
  int gameorder;
  struct ObjList *ls;
  Object *cv;
  Object *obj=NULL,*obj0;
  Object *firstselobj=NULL;
  int sw=0;

  
  switch(command){
  case 0:
    break;
  case 1:
    return(level);
    break;
  case 2:
    return(order);
    break;
  case 3:
    return(lastorder);
    break;
  default:
    break;
  }

  if(lhead==NULL || pixmap==NULL||color==NULL||ps==NULL||key==NULL){
    level=0;
    return(0);
  }  

  if(0&&*pcv==NULL){
    printf("There are no ship selected!!\n");
    key->o=FALSE;
    level=0;
    key->esc=FALSE;
    return(0);
  }

  cv=*pcv;
  if(cv0!=NULL){
    if(cv0!=cv){
      level=0;
    }
  }
  cv0=cv;

  player=ps->id;

  if(ps->proc!=GetProc()){
    fprintf(stderr,"WARNING proc %d %d \n",GetProc(),ps->proc);
    key->o=FALSE;
    level=0;
    key->esc=FALSE;
    return(0);
  }

  if(ps->control!=HUMAN){
    fprintf(stderr,"ERROR: computer controlled ship\n");
    key->o=FALSE;
    level=0;
    key->esc=FALSE;
    return(0);
  }

  if(key->esc|key->tab|key->Pagedown|key->Pageup|key->home){
    key->o=FALSE;
    key->p=FALSE;
    key->esc=FALSE;
    level=0;
    return(0);
  }

  if(level==0){
    key->g=key->s=key->p=key->t=key->r=key->b=key->w=key->e=FALSE;
    //aqui 
    strcpy(cad,"");
    strcpy(ord,"");
    strcpy(par,"");
    strcpy(par0,"");
    
    for(i=0;i<10;i++){
      key->number[i]=FALSE;
    }
    level=1;
  }
  
  if(level==1){
    if(*pcv!=NULL){
      strncpy(cad,"G: GOTO   X: EXPLORE   S: SELECT   P: STOP   T: TAKEOFF   R: REPITE   B: BUY   U: UPGRADE   W: WRITE   E: SELL",128); 
    }
    else{
      strncpy(cad,"S: SELECT   W: WRITE",128); 
      key->g=key->x=key->p=key->t=key->r=key->b=key->u=key->e=FALSE;
    }
//    strncpy(cad,shells[0].menu,128);
    
    if(key->g==TRUE){
      level=2;
      order=GOTO;
      strcpy(ord,"GOTO : ");
      key->g=FALSE;
    }

    if(key->x==TRUE){
      level=2;
      order=EXPLORE;
      strcpy(ord,"EXPLORE");
      key->x=FALSE;
    }

    if(key->s==TRUE){
      level=2;
      order=SELECT;
      strcpy(ord,"SELECT : ");
      key->s=FALSE;
    }
    if(key->p==TRUE){
      level=2;
      order=STOP;
      strcpy(ord,"STOP");
      key->p=FALSE;
    }
    if(key->t==TRUE){
      level=2;
      order=TAKEOFF;
      strcpy(ord,"TAKEOFF");
      key->t=FALSE;
    }
    if(key->r==TRUE){
      level=2;
      order=REPITE;
      strcpy(ord,"REPITE");
      key->r=FALSE;
    }
    if(key->b==TRUE){
      level=2;
      order=BUY;
      strcpy(ord,"BUY");
      key->b=FALSE;
    }
    if(key->u==TRUE){
      level=2;
      order=UPGRADE;
      strcpy(ord,"UPGRADE");
      key->u=FALSE;
    }

    if(key->w==TRUE){
      level=2;
      order=WRITE;
      strcpy(ord,"WRITE : ");
    }
    if(key->e==TRUE){
      level=2;
      order=SELL;
      strcpy(ord,"SELL");
      key->e=FALSE;
    }
    
    Keystrokes(RESET,NULL);
    
    /*    if(order!=REPITE)lastorder=order; */
  }
  /*  printf("last:%d order: %d\n",lastorder,order); */
  if(level==2){

    switch(order){
      
    case SELECT:
    case GOTO:

      strcpy(par,""); 
      Keystrokes(LOAD,par);
      strcpy(cad,"");
      strncat(cad,ord,128);

      DelCharFromCad(par,"1234567890,- fnFN");

      strncat(cad,par,128);
      break;
    case BUY:
      strcpy(cad,"");
      snprintf(pr1,12,"%d",GetPrice(NULL,SHIP1,ENGINE3,CANNON3));
      snprintf(pr2,12,"%d",GetPrice(NULL,SHIP3,ENGINE4,CANNON4));
      snprintf(pr3,12,"%d",GetPrice(NULL,TOWER,ENGINE1,CANNON4));
      snprintf(cad,128,"1: EXPLORER(%s)   2: FIGTHER(%s)   3: TOWER(%s)",pr1,pr2,pr3);
      level=3;
      break;
    case UPGRADE:
      strcpy(cad,"");
      snprintf(cad,128,"%s %s  %d %s",ord,par,
	      (int)(GetPrice(cv,0,0,0)),"   (Esc to cancel)");
      break;
    case EXPLORE:
    case STOP:
    case TAKEOFF:
    case REPITE:
      strcpy(cad,"");
      snprintf(cad,128,"%s %s",ord,par);
      break;
    case SELL:
      strcpy(cad,"");
      snprintf(cad,128,"%s %s  %d %s",ord,par,
	      (int)(0.5*GetPrice(cv,0,0,0)),"   (Esc to cancel)");
      break;
    case WRITE:
      strcpy(par,"");
      Keystrokes(LOAD,par);

      strcpy(cad,"");
      strncat(cad,ord,128);
      strncat(cad,par,128);
      break;
      
    default:
      break;
    }
  }

  if(level==3){
    switch(order){
    case BUY:
      Keystrokes(LOAD,par);
      DelCharFromCad(par,"123");
      switch(strtol(par,NULL,10)){
      case 1:
	strcpy(cad,"EXPLORER"); 
	break;
      case 2:
	strcpy(cad,"FIGTHER"); 
	break;
      case 3:
	strcpy(cad,"TOWER"); 
	break;
      default:
	break;
      }
      break;
    default:
      break;
    }
  }

  if(key->back==TRUE){
    nn--;
    if(nn<0)nn=0;

    strcpy(par0,"");
    strncpy(par0,par,nn);
    strncpy(&par0[nn],"\0",1);
    strcpy(par,par0);

    Keystrokes(DELETELAST,NULL);
    key->back=FALSE;
  }

  if(key->enter==TRUE){
    if(order==REPITE && order !=0){ 
       order=lastorder;
       strcpy(par,"");
       strncpy(par,lastpar,16);
    } 

    if(order==GOTO||order==TAKEOFF||order==EXPLORE||order==STOP){
      key->i=TRUE;
    }

    /* game orders */

    switch(order){
    case SELECT:
    case WRITE:
      gameorder=TRUE;
      break;
    default:
      gameorder=FALSE;
      break;
    }
    
    /* --game orders */

    /* ships orders */

    if(gameorder==FALSE){
      
      /* first selected goes to cv */
      ls=lhead->next;
      while(ls!=NULL){  //HERE create a selected list
	if(ls->obj->selected==TRUE){
	  if(ls->obj->player!=player){ls=ls->next;continue;}
	  obj=ls->obj;
	  obj0=ExecOrder(lhead,obj,obj->player,order,par);
	  if(sw==0){
	    if(obj0!=NULL){
	      firstselobj=obj0;
	      sw++;
	    }
	  }
	}
	ls=ls->next;
      }
      
    }
    else{
      obj0=ExecOrder(lhead,*pcv,player,order,par);
      if(sw==0){
	if(obj0!=NULL){
	  firstselobj=obj0;
	  sw++;
	}
      }
    }
    if(sw){
      if(firstselobj!=cv && firstselobj!=NULL){
	*pcv=firstselobj;
	if(*pcv!=NULL){
	  SelectionBox(pcv,1);
	  (*pcv)->selected=TRUE;
	}
      }
    }
    
    /* --ships orders */

    lastorder=order;
    strcpy(lastpar,"");
    strncpy(lastpar,par,16);

    strcpy(cad,"");
    strcpy(ord,"");
    strcpy(par,"");
    order=0;
    level=0;
    nn=0;
    key->enter=FALSE;
    key->w=FALSE;
    key->o=FALSE;
    for(i=0;i<10;i++){
      key->number[i]=FALSE;
    }
  }

  if(key->mdclick==TRUE){
    printf("order: dclick\n");
  }

  DrawString(pixmap,font,color,10,GameParametres(GET,GHEIGHT,0)+GameParametres(GET,GPANEL,0)/2+4,cad);
  return(0);
}

Object *ExecOrder(struct HeadObjList *lhead,Object *obj,int player,int order,char *par){
  /*
    version 01 18March11
    Add the order given in the shell to ships orders list
    returns:
    a pointer to obj
    If cv changes, returns the new cv
    NULL if the order its not allowed
  */

  int id1=0,id2=0;
  Object *obj_dest=NULL;
  Object *obj_destb=NULL;
  Object *ret=NULL;
  struct Order ord;
  int price;
  char text[MAXTEXTLEN];
  int time;
  float d2,d2b;

  int nargs;
  char arg1[100],arg2[100];

  /* game orders */
  ret=obj;

  switch(order){
  case SELECT:
  case WRITE:
    break;
  default:
    if(obj==NULL)return(NULL);
    break;
  }
  /* -- game orders */

  /* forbidden orders */

  switch(order){
  case GOTO:
  case TAKEOFF:
  case EXPLORE:
    if(obj->engine.type<=ENGINE1)return(NULL);
    break;
  case SELL:
  case BUY:
  case UPGRADE:
    if(obj->mode!=LANDED){
      printf("You must be landed.\n");
      return(NULL);
    }
    break;
  default:
    break;
  }

  /* --forbidden orders */

  
  nargs=Get2Args(par,&arg1[0],&arg2[0]);

  time=GetTime();

  ord.id=NOTHING;
  ord.time=0;

  switch(order){
  case GOTO:

    switch(nargs){
    case 1:
      
      switch(arg1[0]){
      case 'n':
      case 'N':

	/* goto nearest ally planet */
	
	obj_dest=NearestObj(lhead,obj,PLANET,PINEXPLORE,&d2); // HERE only one function
	obj_destb=NearestObj(lhead,obj,PLANET,PALLY,&d2b);
	
	if(obj_dest!=NULL && obj_destb!=NULL ){
	  if(d2b<d2){
	    obj_dest=obj_destb;
	  }
	}
	if(obj_dest==NULL){
	  obj_dest=obj_destb;
	}   
	if(obj_dest!=NULL)id1=obj_dest->id;
	
	break;
      case 'F':
      case 'f':
	switch(arg1[1]){
	case '1':
	  id1=fobj[0];
	  break;
	case '2':
	  id1=fobj[1];
	  break;
	case '3':
	  id1=fobj[2];
	  break;
	case '4':
	  id1=fobj[3];
	  break;
	default:
	  break;
	}
	obj_dest=SelectObj(lhead,id1);
	
	break;
      default:

	id1=strtol(arg1,NULL,10);
	if(id1!=0){
	  obj_dest=SelectpObj(lhead,id1,obj->player);
	}
	break;
      }

      if(obj_dest!=NULL){
	switch(obj_dest->type){
	case PLANET: /* if planet is unknown*/
	  if(IsInIntList((players[obj->player].kplanets),id1)==0){
	    obj_dest=NULL;
	    printf("Not Allowed. Planet or ship unknown.\n");
	  }
	  else{
	    printf("(%c %d) going to planet %d.\n",Type(obj),obj->pid,obj_dest->pid);
	  }
	  break;
	case SHIP:/* ship belongs to another player*/
	  if(obj_dest->player!=obj->player){
	    obj_dest=NULL;
	    printf("Not Allowed. Destiny is an enemy ship.\n");
	  }
	  else{
	    printf("(%c %d) going to ship %d.\n",Type(obj),obj->pid,obj_dest->pid);
	  }
	  break;
	default:
	  break;
	}
      }
      
      if(obj_dest!=NULL){
	ord.priority=1;
	ord.id=GOTO;
	ord.time=0;
	ord.g_time=time;
	ord.a=obj_dest->x;
	ord.b=obj_dest->y;
	ord.c=obj_dest->id;
	ord.d=obj_dest->type;
	ord.e=obj_dest->pid;
	ord.f=ord.g=ord.h=0;
	DelAllOrder(obj);
	AddOrder(obj,&ord);

	//	printf("Ship %d: going to sector %f %f\n",obj->id,ord.a,ord.b);

      }
      break;
    case 2:
      id1=strtol(arg1,NULL,10);
      id2=strtol(arg2,NULL,10);

      ord.priority=1;
      ord.id=GOTO;
      ord.time=0;
      ord.g_time=time;
      ord.a=id1*SECTORSIZE+SECTORSIZE/2;
      ord.b=id2*SECTORSIZE+SECTORSIZE/2;
      ord.c=-1;
      ord.d=0;
      ord.e=ord.f=ord.g=ord.h=0;
      DelAllOrder(obj);
      AddOrder(obj,&ord);

      printf("(%c %d) going to sector %d %d.\n",Type(obj),obj->pid,id1,id2);

      break;
    default:
      printf("shell() invalid entry\n");
      break;
    }

    break;
  case EXPLORE:
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
    printf("(%c %d) going to explore.\n",Type(obj),obj->pid);
    break;

  case SELECT:

    switch(arg1[0]){
      case 'F':
      case 'f':
	switch(arg1[1]){
	case '1':
	  id1=fobj[0];
	  break;
	case '2':
	  id1=fobj[1];
	  break;
	case '3':
	  id1=fobj[2];
	  break;
	case '4':
	  id1=fobj[3];
	  break;
	default:
	  break;
	}
	obj_dest=SelectObj(lhead,id1);
	
	break;
      default:

	id1=strtol(arg1,NULL,10);
	if(id1!=0){
	  obj_dest=SelectpObj(lhead,id1,player);
	}
	break;
    }

    if(obj_dest!=NULL){
      if(obj_dest->type==PLANET){
	obj_dest=SelectpObjInObj(lhead,obj_dest->id,player);
      }
      if(obj_dest!=NULL){
	if(obj!=NULL)obj->selected=FALSE;
	obj_dest->selected=TRUE;
	ret=obj_dest;
	printf("(%c %d) selected.\n",Type(obj_dest),obj_dest->pid);
      }
    }
    
    break;

  case STOP:
    obj_dest=obj;
    if(obj_dest!=NULL){


      ord.priority=1;
      ord.id=STOP;

      ord.time=0;
      ord.g_time=time;
      
      ord.a=ord.b=ord.c=ord.d=0;
      ord.e=ord.f=ord.g=ord.h=0;

      /* if ship is in a planet change order to goto this planet */
/*       if(obj_dest->in!=NULL){ */
/* 	ord.id=GOTO; */
/* 	ord.a=obj_dest->in->x; */
/* 	ord.b=obj_dest->in->y; */
/* 	ord.c=obj_dest->in->id; */
/* 	ord.d=obj_dest->in->type; */
/* 	ord.e=ord.f=ord.g=ord.h=0; */
/*       } */


      DelAllOrder(obj_dest);
      AddOrder(obj_dest,&ord);
      printf("(%c %d) stopping.\n",Type(obj),obj->pid);
    }
    break;
  case TAKEOFF:
    obj_dest=obj;
    if(obj_dest!=NULL){
      ord.priority=1;
      ord.id=TAKEOFF;
      ord.time=40;
      ord.g_time=time;
      
      ord.a=ord.b=ord.c=ord.d=0;
      ord.e=ord.f=ord.g=ord.h=0;
      DelAllOrder(obj_dest);
      AddOrder(obj_dest,&ord);
      printf("(%c %d) taking off.\n",Type(obj),obj->pid);
    }
    break;
    
  case BUY:
    if(obj!=NULL){
      id1=strtol(arg1,NULL,10);
      if(id1>=1 && id1<=3){
	switch(id1){
	case 1:
	  if(BuyShip(players[obj->player],obj,EXPLORER)==SZ_OK){
	    printf("Explorer buyed.\n");
	  }
	  break;
	case 2:
	  if(BuyShip(players[obj->player],obj,FIGHTER)==SZ_OK){
	    printf("Fighter buyed.\n");
	  }
	  break;
	case 3:
	  if(BuyShip(players[obj->player],obj,TOWER)==SZ_OK){
	    printf("Tower buyed.\n");
	  }
	  break;
	default:
	  break;
	}
      }
    }
    break;
  case SELL:
    printf("Selling ship with id: %d\n",obj->pid);
    price=.5*GetPrice(obj,0,0,0);
    if(price>0){
      printf("Price: %d eng: %d weapon: %d\n",
	     price, obj->engine.type, obj->weapon->type);
      AddGold(players,obj->player,price);
      obj->state=-1;
    }
    break;

  case UPGRADE:
    if(obj->level+1 < players[obj->player].maxlevel){
      price=GetPrice(obj,0,0,0);
      if(price>0){
	if(players[obj->player].gold>price){
	  //      obj->level++;
	  players[obj->player].gold-=price;
	  Experience(obj,(int)(100*pow(2,obj->level) - obj->experience+1));
	  printf("(%c %d) upgrade to level %d.\n",Type(obj),obj->pid,obj->level);
	}
	else{
	  printf("You have not enough gold\n");
	  ret=NULL;
	}
      }
    }
    else{
      printf("You can upgrade until level %d\n",players[obj->player].maxlevel-1);
      ret=NULL;
    }
    break;
    
  case WRITE:
    printf("=============\n");
    Keystrokes(LOAD,text);
    printf("%s\n",text);
    printf("=============\n");
    if(GameParametres(GET,GNET,0)==TRUE){
      SendTextMessage(text);   
    }
    SetDefaultKeyValues(&keys,0);
    break;
  default:
    break;
  }
  return(ret);
}


void SelectionBox(Object **pcv,int reset){
  /* version 01*/
  static int sw=0;
  static Region region;
  static Object *cv0=NULL;
  int x0,y0;
  int x1,y1;
  Object *cv;
  int n;  
  cv=*pcv;

  if(reset){ /* reset */
    region.rect.width=region.rect.height=0;
    UnmarkObjs(&listheadobjs);
    if(cv!=NULL){
      habitat.type=cv->habitat;
      habitat.obj=cv->in;
      (*pcv)->selected=TRUE;
    }
    sw=0;
    return;
  }

  if(region.habitat<0)return;
  //  printf("habitat: %d \n",habitat.type);
  if((habitat.type==H_SPACE && gdraw.map==TRUE) || habitat.type==H_PLANET){
    if(keys.mleft==FALSE){
      if(sw){ /* mouse release */
	/* region 0: space & map, >0 planet*/
#if DEBUG
	printf("BOX0: %d %d w:%d h:%d\n",region.rect.x,region.rect.y, 
	       region.rect.width,region.rect.height); 
#endif
	if(region.rect.width==0 || region.rect.height==0){
	  /*reset*/
	  region.rect.width=region.rect.height=0;
	  UnmarkObjs(&listheadobjs);
	  if(cv!=NULL){
	    habitat.type=cv->habitat;
	    habitat.obj=cv->in;
	    cv->selected=TRUE;
	  }
	  sw=0;
	}
	else{
	  if(region.rect.width<0){
	    region.rect.x+=region.rect.width;
	    region.rect.width*=-1;
	  }
	  if(region.rect.height<0){
	    region.rect.y+=region.rect.height;
	    region.rect.height*=-1;
	  }
	  
	  if(region.habitat==0){ /* free space */
	    /* window to real coordinates */
	  }
	  if(region.habitat>0){
	    region.rect.y=GameParametres(GET,GHEIGHT,0)-region.rect.y;
	  }

	  Window2Real(cv,region.habitat,region.rect.x,region.rect.y,&x0,&y0); 
	  Window2Real(cv,region.habitat,region.rect.x+region.rect.width, 
		      region.rect.y+region.rect.height, 
		      &x1,&y1); 
	  
	  region.rect.x=x0; 
	  region.rect.y=y0; 
	  region.rect.width=x1-x0; 
	  region.rect.height=y1-y0; 
	  
#if DEBUG	    
	  printf("BOX1: %d %d w:%d h:%d\n",region.rect.x,region.rect.y, 
		 region.rect.width,region.rect.height);  /* window coordinates */
#endif	    
	  if(region.habitat>=0){
	    cv0=MarkObjs(&listheadobjs,region);
	    
	    if(cv0!=NULL){
	      *pcv=cv0;
	      habitat.type=(*pcv)->habitat;
	      habitat.obj=(*pcv)->in;
	      (*pcv)->selected=TRUE;
	      ///		SelectionBox(cv,1); /* reset selection box*/
	    }
	  }
	  n=CountSelected(&listheadobjs);
	  if(n<6){
	    printf("Selected ships:\n");
	    PrintSelected(&listheadobjs); 
	  }
	  else printf("Selected %d ships.\n",n);
	}
	sw=0;
      }
    }
    
    if(keys.mleft==TRUE){
      /* printf("\t mouse pos: %d %d\n",mouse_pos.x,mouse_pos.y);  */
      if(sw==0){
	region.rect.x=mouse_pos.x;
	region.rect.y=mouse_pos.y;
	region.rect.width=region.rect.height=0;
	
	region.habitat=-1;
	if(gdraw.map==TRUE){
	  region.habitat=0;
	}
	else{
	  if(habitat.type==H_PLANET){
	    if(cv!=NULL)region.habitat=cv->in->id;
	  }
	}
	sw=1;
      }
      region.rect.width=mouse_pos.x-region.rect.x;
      region.rect.height=mouse_pos.y-region.rect.y;
    }
    
    if(region.rect.width!=0){
      if(region.habitat>0 && gdraw.map==FALSE){
	DrawSelectionBox(pixmap,penGreen,region,cv);
      }
      else{
	if(region.habitat==0 && gdraw.map==TRUE){
	  DrawSelectionBox(pixmap,penGreen,region,cv);
	}
      }
    }
  }
}


int Keystrokes(int action,char *c){
  /* 
     grab all the keystrokes in the cad text
     returns the number of chars manipulated.
   */
  static int n=0;
  static char text[MAXTEXTLEN];
  int m=0;
  char endln='\0';

  switch(action){
  case RESET: /*  RESET to 0 */
    n=0;
    break;
  case ADD: /*  ADD */
    memcpy(&text[n],c,sizeof(char));
    n++;
    if(n>MAXTEXTLEN-1)n=MAXTEXTLEN-1;
    m=1;
    break;
  case DELETELAST: /*  DELETE the last char */
    if(n>=1){
      n--;
      m=1;
    }
    else m=0;
    break;
  case RETURNLAST: /*  RETURNS the last character */
    if(n>=1){
      memcpy(c,&text[n-1],sizeof(char));
      m=1; 
    }
    else m=0;
    break;
  case LOAD: /*  LOAD */
    if(c!=NULL){
      memcpy(c,text,n*sizeof(char));
      memcpy(&c[n*sizeof(char)],&endln,sizeof(char));
      m=n;
    }
    break;
  default:
    break;
  }
  return (m);
}

int Get2Args(char *cad,char *arg1,char *arg2){
  /*
    copy the first in arg1.
    copy the 2nd in arg2
    returns the number of arguments 0,1,2
  */
  int nargs=0; /* number of arguments */
  int n=0;
  int m=0;
  char a;
  int inw=0;
  int cont=0;
  int endw=0;
  int endl=0;

  strncpy(&arg1[0],"\0",1);
  strncpy(&arg2[0],"\0",1);


  strncpy(&a,&cad[n],1);


  /* first arg */

  while(!endw){
    switch(a){
    case'\0':
      strncpy(&arg1[m],"\0",1);
      endw=1;
      endl=1;
      break;
    case ' ':
    case ',':
    case '\t':
      if(inw){
	strncpy(&arg1[m],"\0",1);
	endw=1;
      }
      break;
    default:
      inw=1;
      strncpy(&arg1[m],&a,1);
      m++;
      break;
    }
    n++;
    strncpy(&a,&cad[n],1);
  }
  if(inw)nargs=1;

  if(endl){
    return(nargs);
  }
 
  /* second args, the rest*/
  cont=0;
  inw=0;
  endw=0;
  m=0;  
  strncpy(&a,&cad[n],1);
  while(!endl){
    switch(a){
    case '\0':
      strncpy(&arg2[m],"\0",1);
      endw=1;
      endl=1;
      break;
    case ' ':
    case ',':
    case '\t':
      if(inw){
	strncpy(&arg2[m],&a,1);
	nargs++;
	m++;
      }
      break;
    default:
      inw=1;
      if(nargs==1)nargs=2;
      strncpy(&arg2[m],&a,1);
      m++;
      break;
    }
    n++;
    strncpy(&a,&cad[n],1);
  }
  
  //  printf("GA final(%s) (%s) (%s)\n",cad,arg1,arg2);
  return(nargs);
}
