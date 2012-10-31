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

#include <string.h>
#include "general.h"
#include "shell.h"
#include "ai.h"
#include "objects.h"
#include "spacecomm.h"
#include "functions.h"
#include "graphics.h"

extern struct Player *players;
extern struct Habitat habitat;
extern struct Keys keys;
extern struct HeadObjList listheadobjs;       /* list of all objects */
extern int fobj[4];
extern struct Draw gdraw;
#if DEBUG
int debugshell=FALSE;
#endif


int GetView(void);

int Shell(int command,GdkPixmap *pixmap,GdkGC *color,GdkFont *font,struct HeadObjList *lhead,struct Player *ps,struct Keys *key,Object **pcv){
  /*
    version 04
  */
  static char cad[MAXTEXTLEN]="";
  static char ord[16]="";
  static char par[MAXTEXTLEN]="";
  static char pargoto[MAXTEXTLEN]="";
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
  int textw;

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
    key->order.state=FALSE;
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
    key->order.state=FALSE;
    level=0;
    key->esc=FALSE;
    return(0);
  }

  if(ps->control!=HUMAN){
    fprintf(stderr,"ERROR: computer controlled ship\n");
    key->order.state=FALSE;
    level=0;
    key->esc=FALSE;
    return(0);
  }
  if(key->esc|key->tab|key->Pagedown|key->Pageup|key->home){
    key->order.state=FALSE;
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


      strncpy(cad,"G: GOTO   X: EXPLORE   S: SELECT   P: STOP   T: TAKEOFF   R: REPEAT   B: BUY   U: UPGRADE   W: WRITE   E: SELL   D:RETREAT",MAXTEXTLEN); 
      
      if(font!=NULL){
	textw=gdk_text_width(font,cad,strlen(cad));
      }
      else{
	textw=12*strlen(cad);
      }
      if(textw>GameParametres(GET,GWIDTH,0)){
	strncpy(cad,"",1);
	strncpy(cad,"G:GT  X:EXP  S:SLC  P:STP  T:TOFF  R:RPT  B:BUY  U:UPG  W:WRT  E:SELL  D:RTRT",MAXTEXTLEN); 
      }
      
      if((*pcv)->type==SHIP && (*pcv)->subtype==PILOT && CountNSelected(lhead,player)==1){
	strncpy(cad,"S: SELECT   B: BUY   W: WRITE",MAXTEXTLEN); 
	key->g=key->x=key->p=key->t=key->r=key->e=key->u=FALSE;
      }

      if(0&&CountNSelected(lhead,player)>1){
	strncpy(cad,"G: GOTO   X: EXPLORE   P: STOP   T: TAKEOFF   R: REPEAT   B: BUY   U: UPGRADE   W: WRITE   E: SELL   D:RETREAT",MAXTEXTLEN); 

	if(font!=NULL){
	  textw=gdk_text_width(font,cad,strlen(cad));
	}
	else{
	  textw=12*strlen(cad);
	}
	if(textw>GameParametres(GET,GWIDTH,0)){
	  strncpy(cad,"",1);
	  strncpy(cad,"G:GT  X:EXP  P:STP  T:TOFF  R:RPT  B:BUY  U:UPG  W:WRT  E:SELL  D:RTRT",MAXTEXTLEN); 
	}
	key->s=FALSE;
      }
      
    }
    else{
      strncpy(cad,"S: SELECT   W: WRITE",MAXTEXTLEN); 
      key->g=key->x=key->p=key->t=key->r=key->b=key->u=key->e=FALSE;
    }

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
      order=REPEAT;
      strcpy(ord,"REPEAT");
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
    if(key->d==TRUE){
      level=2;
      order=RETREAT;
      strcpy(ord,"RETREAT : ");
      key->d=FALSE;
    }
    
    Keystrokes(RESET,NULL,NULL);
    
    /*    if(order!=REPEAT)lastorder=order; */
  }
  /*  printf("last:%d order: %d\n",lastorder,order); */
  if(level==2){

    switch(order){
      
    case SELECT:
    case GOTO:
    case RETREAT:
      /* keyb or mouse order */
      if(key->mright==TRUE  && *pcv!=NULL){
	int view=GetView();

	switch(view){
	  int x,y,a,b,pid;
	  Object *nearobj;

	case VIEW_MAP:
	  MousePos(GET,&x,&y);
	  strcpy(pargoto,"");
	  strcpy(par,"");
	  a=x;
	  b=y;
	  //	W2R(*pcv,&a,&b);
	  Window2Real(*pcv,VIEW_MAP, x,y,&a,&b);
	  
	  nearobj=ObjNearThan(lhead,player,a,b,500000); /* 0.5 sectors */
	  if(nearobj!=NULL)pid=nearobj->pid;
	  else pid=0;
	  
	  if(pid!=0){ /* is a planet or a spaceship */
	    snprintf(pargoto,12,"%d",pid);
	    snprintf(par,12,"%d",pid);
	  }
	  else{ /* universe coordinates */
	    Window2Sector(*pcv,&x,&y);
	    sprintf(pargoto,"%d %d",x,y);
	    sprintf(par,"%d %d",a,b);
	    //	    printf("first: %d %d (%s)\n",a,b,pargoto);
	  }
	  key->enter=TRUE;
	  break;

	case VIEW_SPACE:
	  break;

	case VIEW_PLANET:
	  break;

	default:
	  break;
	}
	key->mright=FALSE;
      }
      else{
	/* keyb order */
	int nargs,id1,id2;
	char arg1[MAXTEXTLEN],arg2[MAXTEXTLEN];
	strcpy(par,"");
	Keystrokes(LOAD,NULL,par);
	strcpy(pargoto,"");
	Keystrokes(LOAD,NULL,pargoto);
	
	if(1){
	  
	  // cambiar texto por coordenadas reales
	  nargs=Get2Args(pargoto,&arg1[0],&arg2[0]);
	  switch(nargs){
	  case 0:
	  case 1:
	    break;
	  case 2:
	    id1=strtol(arg1,NULL,10);
	    id2=strtol(arg2,NULL,10);
	    /* pasar de sectores a coordenadas reales */
	    sprintf(par,"%d %d",id1*SECTORSIZE+SECTORSIZE/2,id2*SECTORSIZE+SECTORSIZE/2);
	    break;
	  default:
	    printf("ERROR (%d)\n",nargs);
	    break;
	  }
	}
      }
      strcpy(cad,"");
      strncat(cad,ord,MAXTEXTLEN-strlen(cad));
      
      DelCharFromCad(pargoto,"1234567890,- fnFN");
      
      strncat(cad,pargoto,MAXTEXTLEN-strlen(cad));
      
      break;
    case BUY:
      strcpy(cad,"");
      snprintf(pr1,12,"%d",GetPrice(NULL,EXPLORER,ENGINE3,CANNON3));
      snprintf(pr2,12,"%d",GetPrice(NULL,FIGHTER,ENGINE4,CANNON4));
      snprintf(pr3,12,"%d",GetPrice(NULL,TOWER,ENGINE1,CANNON4));
      snprintf(cad,MAXTEXTLEN,"1: EXPLORER(%s)   2: FIGTHER(%s)   3: TOWER(%s)",pr1,pr2,pr3);
      if((*pcv)->type==SHIP && (*pcv)->subtype==PILOT){
	snprintf(cad,MAXTEXTLEN,"                  2: FIGTHER(%s)",pr2);
      }
      level=3;
      break;
    case UPGRADE:
      strcpy(cad,"");
      snprintf(cad,MAXTEXTLEN,"%s %s  %d %s",ord,par,
	      (int)(GetPrice(cv,0,0,0)),"   (Esc to cancel)");
      break;
    case EXPLORE:
    case STOP:
    case TAKEOFF:
    case REPEAT:
      //    case RETREAT:
      strcpy(cad,"");
      snprintf(cad,MAXTEXTLEN,"%s %s",ord,par);
      //      printf("RETREAT: %s\n",cad);
      break;
    case SELL:
      strcpy(cad,"");
      snprintf(cad,MAXTEXTLEN,"%s %s  %d %s",ord,par,
	      (int)(0.5*GetPrice(cv,0,0,0)),"   (Esc to cancel)");
      break;
    case WRITE:
      strcpy(par,"");
      Keystrokes(LOAD,NULL,par);

      strcpy(cad,"");
      strncat(cad,ord,MAXTEXTLEN-strlen(cad));
      strncat(cad,par,MAXTEXTLEN-strlen(cad));
      break;
      
    default:
      break;
    }
  }

  if(level==3){
    switch(order){
    case BUY:
      Keystrokes(LOAD,NULL,par);
      DelCharFromCad(par,"123");
      if((*pcv)->type==SHIP && (*pcv)->subtype==PILOT){
	DelCharFromCad(par,"2");
      }
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


  /***** Canceling the order *****/

  if(key->back==TRUE){
    nn--;
    if(nn<0)nn=0;
    strcpy(par0,"");
    strncpy(par0,par,nn);
    strncpy(&par0[nn],"\0",1);
    strncpy(par,par0,MAXTEXTLEN);

    Keystrokes(DELETELAST,NULL,NULL);
    key->back=FALSE;
    key->enter=FALSE;
  }

  /**** executing the order *****/

  if(key->enter==TRUE){
    if(order==REPEAT && order !=0){ 
       order=lastorder;
       strcpy(par,"");
       strncpy(par,lastpar,16);
    } 

    if(order==GOTO||order==TAKEOFF||order==EXPLORE||order==STOP||order==RETREAT){
      key->automode.state=TRUE;
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
      
      /* first selected goes to cv if cv is not selected*/
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
	  if(obj0==cv)firstselobj=cv;
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
	  SelectionBox(pixmap,color,pcv,2);
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
    key->order.state=FALSE;
    for(i=0;i<10;i++){
      key->number[i]=FALSE;
    }

    /* delete selectionbox*/
    SelectionBox(pixmap,color,pcv,1);
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
  int status;
  int price;
  int time;
  float d2,d2b;
  int retreatsw=0;
  int nargs;
  char arg1[MAXTEXTLEN],arg2[MAXTEXTLEN];
  char stmess[MAXTEXTLEN];

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

  case TAKEOFF:
    if(obj->habitat!=H_PLANET)return(NULL);
    if(obj->type==SHIP && obj->subtype==PILOT)return(NULL);
    if(obj->type==SHIP && obj->subtype==TOWER && obj->ai==0){
      obj->ai=1;
      return(NULL);
    }
    if(obj->engine.type<=ENGINE1)return(NULL);

    break;
  case RETREAT:
  case EXPLORE:
  case STOP:
  case GOTO:
    if(obj->type==SHIP && obj->subtype==PILOT)return(NULL);
    if(obj->type==SHIP && obj->subtype==TOWER && obj->ai==0){
      obj->ai=1;
      return(NULL);
    }
    if(obj->engine.type<=ENGINE1)return(NULL);
    break;
  case SELL:
    if(obj->type==SHIP && obj->subtype==PILOT){
      printf("You can't sell pilots\n");
      snprintf(stmess,MAXTEXTLEN,"You can't sell pilots");
      ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
      return(NULL);
    }
  case BUY:
  case UPGRADE:
    if(obj->mode!=LANDED){
      printf("You must be landed.\n");
      snprintf(stmess,MAXTEXTLEN,"You must be landed.");
      ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
      return(NULL);
    }
    break;
  default:
    break;
  }

  /* --forbidden orders */

  /** change manual to auto mode **/

  switch(order){

  case TAKEOFF:
  case RETREAT:
  case EXPLORE:
  case STOP:
  case GOTO:
    obj->ai=1;
    break;
  case SELL:
  case BUY:
  case UPGRADE:
  default:
    break;
  }

  
  nargs=Get2Args(par,&arg1[0],&arg2[0]);
  time=GetTime();

  ord.id=NOTHING;
  ord.time=0;
  retreatsw=0;

  if(order==RETREAT && nargs==0){
    nargs=1;
    strcpy(&arg1[0],"n");
  }

  switch(order){

  case RETREAT:

    retreatsw=1;
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
	    printf("Not Allowed. Planet or spaceship %d unknown.\n",obj_dest->pid);
	    snprintf(stmess,MAXTEXTLEN,"Not Allowed. Planet or spaceship %d unknown.",obj_dest->pid);
	    ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
	    obj_dest=NULL;
	  }
	  else{
	    printf("(%c %d) going to planet %d.\n",Type(obj),obj->pid,obj_dest->pid);
	    snprintf(stmess,MAXTEXTLEN,"(%c %d) going to planet %d.",Type(obj),obj->pid,obj_dest->pid);
	    ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
	  }
	  break;
	case SHIP:/* ship belongs to another player*/
	  if(obj_dest->player!=obj->player){
	    obj_dest=NULL;
	    printf("Not Allowed. Destiny is an enemy spaceship.\n");
	    snprintf(stmess,MAXTEXTLEN,"Not Allowed. Destiny is an enemy spaceship.");
	    ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
	  }
	  else{
	    if(obj_dest==obj){
	      printf("(%c %d) Not Allowed. Destiny %d is equal than origin.\n",
		     Type(obj),obj->pid,obj_dest->pid);
	    snprintf(stmess,MAXTEXTLEN,"(%c %d) Not Allowed. Destiny %d is equal than origin.",
		     Type(obj),obj->pid,obj_dest->pid);
	    ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
	      obj_dest=NULL;
	      }	
	    else{
	      printf("(%c %d) going to spaceship %d.\n",
		     Type(obj),obj->pid,obj_dest->pid);
	      snprintf(stmess,MAXTEXTLEN,"(%c %d) going to spaceship %d.",
		       Type(obj),obj->pid,obj_dest->pid);
	      ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
	    }
	  }
	  break;
	default:
	  break;
	}
      }
      else{
	printf("Not Allowed. Planet or spaceship unknown.\n");
	snprintf(stmess,MAXTEXTLEN,"Not Allowed. Planet or spaceship unknown.");
	ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
      }
      
      if(obj_dest!=NULL){
	ord.priority=1;
	if(!retreatsw)ord.id=GOTO;
	else ord.id=RETREAT;
	ord.time=0;
	ord.g_time=time;
	ord.a=obj_dest->x;
	ord.b=obj_dest->y;
	ord.c=obj_dest->id;
	ord.d=obj_dest->type;
	ord.e=obj_dest->pid;
	ord.f=ord.g=ord.h=0;
	ord.i=ord.j=ord.k=ord.l=0;
	if(obj_dest->type==SHIP && obj_dest->subtype==PILOT){
	  ord.i=PILOT;
	  //	  printf("ord_i 1st: %f\n",ord.i);
	}

	DelAllOrder(obj);
	AddOrder(obj,&ord);

      }
      break;
    case 2: // AQUI
      id1=strtol(arg1,NULL,10);
      id2=strtol(arg2,NULL,10);

      ord.priority=1;
      ord.id=GOTO;

      if(retreatsw)ord.id=RETREAT;
      ord.time=0;
      ord.g_time=time;
      ord.a=id1*SECTORSIZE+SECTORSIZE/2;
      ord.b=id2*SECTORSIZE+SECTORSIZE/2;
      ord.a=id1;
      ord.b=id2;
      ord.c=-1;
      ord.d=0;
      ord.e=ord.f=ord.g=ord.h=0;
      ord.i=ord.j=ord.k=ord.l=0;
      DelAllOrder(obj);
      AddOrder(obj,&ord);

      printf("(%c %d) going to sector %d %d.\n",
	     Type(obj),
	     obj->pid,
	       (id1-SECTORSIZE/2)/SECTORSIZE,
	       (id2-SECTORSIZE/2)/SECTORSIZE);
      snprintf(stmess,MAXTEXTLEN,"(%c %d) going to sector %d %d.",Type(obj),obj->pid,
	       (id1-SECTORSIZE/2)/SECTORSIZE,
	       (id2-SECTORSIZE/2)/SECTORSIZE);
      ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
      break;
    default:
      printf("shell() invalid entry\n");
      snprintf(stmess,MAXTEXTLEN,"shell() invalid entry.");
      ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
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
    ord.i=ord.j=ord.k=ord.l=0;
    DelAllOrder(obj);
    AddOrder(obj,&ord);
    printf("(%c %d) going to explore.\n",Type(obj),obj->pid);
    snprintf(stmess,MAXTEXTLEN,"(%c %d) going to explore.",Type(obj),obj->pid);
    ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
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
	snprintf(stmess,MAXTEXTLEN,"(%c %d) selected.",Type(obj_dest),obj_dest->pid);
	ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
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
      ord.i=ord.j=ord.k=ord.l=0;
      /* if ship is in a planet change order to goto this planet */
/*       if(obj_dest->habitat==H_PLANET){ */
/* 	ord.id=GOTO; */
/* 	ord.a=obj_dest->in->x; */
/* 	ord.b=obj_dest->in->y; */
/* 	ord.c=obj_dest->in->id; */
/* 	ord.d=obj_dest->in->type; */
/* 	ord.e=ord.f=ord.g=ord.h=0; */
/*       } */


      DelAllOrder(obj_dest);
      AddOrder(obj_dest,&ord);
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
      ord.i=ord.j=ord.k=ord.l=0;
      DelAllOrder(obj_dest);
      AddOrder(obj_dest,&ord);
      printf("(%c %d) taking off.\n",Type(obj),obj->pid);
      snprintf(stmess,MAXTEXTLEN,"(%c %d) taking off.",Type(obj),obj->pid);
      ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
    }
    break;
    
  case BUY:
    if(obj!=NULL){
      id1=strtol(arg1,NULL,10);
      if(id1>=1 && id1<=3){
	status=SZ_OK;
	switch(id1){
	case 1:
	  status=BuyShip(players[obj->player],obj,EXPLORER);
	  if(status==SZ_OK){
	    printf("Explorer buyed.\n");
	    snprintf(stmess,MAXTEXTLEN,"Explorer buyed.");
	    ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
	  }
	  break;
	case 2:
	  status=BuyShip(players[obj->player],obj,FIGHTER);
	  if(status==SZ_OK){
	    printf("Fighter buyed.\n");
	    snprintf(stmess,MAXTEXTLEN,"Fighter buyed.");
	    ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
	  }
	  break;
	case 3:
	  status=BuyShip(players[obj->player],obj,TOWER);
	  if(status==SZ_OK){
	    printf("Tower buyed.\n");
	    snprintf(stmess,MAXTEXTLEN,"Tower buyed.");
	    ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
	  }
	  break;
	default:
	  break;
	}
	switch(status){
	case SZ_OK:
	  break;
	case SZ_OBJNULL:
	  break;
	case SZ_UNKNOWNERROR:
	  break;
	case SZ_OBJNOTLANDED:
	  printf("Spaceship must be landed.\n");
	  snprintf(stmess,MAXTEXTLEN,"Spaceship must be landed.");
	  ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
	  break;
	case SZ_NOTOWNPLANET:
	  printf("You don't own this planet\n");
	  snprintf(stmess,MAXTEXTLEN,"You don't own this planet.");
	  ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
	  break;
	case SZ_NOTENOUGHGOLD:
	  printf("You have not enough gold\n");
	  snprintf(stmess,MAXTEXTLEN,"You have not enough gold.");
	  ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
	  break;
	case SZ_NOTALLOWED:
	  printf("Not allowed\n");
	  snprintf(stmess,MAXTEXTLEN,"Not allowed.");
	  ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
	  break;
	case SZ_NOTIMPLEMENTED:
	default:
	  fprintf(stderr, "Warning ExecOrder(): (%d) Not implemented\n",status);
	  break;
	}
      }
    }
    break;
  case SELL:
    printf("Selling spaceship with id: %d\n",obj->pid);
    snprintf(stmess,MAXTEXTLEN,"Selling spaceship with id: %d",obj->pid);
    ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
    price=.5*GetPrice(obj,0,0,0);
    if(price>0){
      printf("Price: %d eng: %d weapon: %d\n",
	     price, obj->engine.type, obj->weapon->type);
      AddGold(players,obj->player,price);
      obj->mode=SOLD;
      obj->items=0;
      obj->state=-1;
    }
    break;

  case UPGRADE:
    if(obj->level+1 < players[obj->player].maxlevel){
      price=GetPrice(obj,0,0,0);
      if(price>0){
	if(players[obj->player].gold>price){
	  players[obj->player].gold-=price;
	  Experience(obj,(int)(100*pow(2,obj->level) - obj->experience+1));
	  printf("(%c %d) upgraded to level %d.\n",Type(obj),obj->pid,obj->level);
	  snprintf(stmess,MAXTEXTLEN,"(%c %d) upgraded to level %d.",Type(obj),obj->pid,obj->level);
	  ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
	}
	else{
	  printf("You have not enough gold\n");
	  snprintf(stmess,MAXTEXTLEN,"You have not enough gold.");
	  ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
	  ret=NULL;
	}
      }
    }
    else{
      printf("You can upgrade until level %d\n",players[obj->player].maxlevel-1);
      snprintf(stmess,MAXTEXTLEN,"You can upgrade until level %d",players[obj->player].maxlevel-1);
      ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
      ret=NULL;
    }
    break;
    
  case WRITE:
    {
      char text[MAXTEXTLEN];
      char cad[MAXTEXTLEN];

      Keystrokes(LOAD,NULL,text);
      snprintf(cad,MAXTEXTLEN,"%s: %s",players[obj->player].playername,text);      

      printf("=============\n");
      //      printf("%s\n",cad);
      printf("=============\n");
      if(GameParametres(GET,GNET,0)==TRUE){
	SendTextMessage(cad);
      }
      SetDefaultKeyValues(&keys,0);
      
    }    break;
  default:
    break;
  }
  return(ret);
}


void SelectionBox(GdkPixmap *pixmap,GdkGC *color,Object **pcv,int reset){
  /* version 03*/

  static int mouserelease=FALSE;
  static Space region;
  static int sw=0;
  static Object *cv0=NULL;
  char stmess[MAXTEXTLEN];
  int x,y;
  int x0,y0;
  int x1,y1;
  Object *cv;
  int n;
  int view;

  if(0){
    if(*pcv==NULL){
      printf("SOS: %p\n",cv0);
      if(keys.mleft==TRUE){ /* mouse release */
	cv0=SelectOneShip(&listheadobjs,region,NULL,keys.ctrl);
	printf("SOS: %p\n",cv0);
	*pcv=cv0;
      }
    }
  }

  if(*pcv==NULL)return;
  cv=*pcv;

  if(sw==0){
    region.rect.width=region.rect.height=0;
    region.habitat=-1;
    sw++;
  }

  if(cv0!=cv){
    cv0=cv;
    if(reset==0)reset=1;
  }

  switch(reset){
  case 0:
    break;
  case 1:
    region.rect.width=region.rect.height=0;
    region.habitat=-1;
    (*pcv)->selected=TRUE;
    mouserelease=FALSE;
    return;
    break;
  case 2:
    region.rect.width=region.rect.height=0;
    region.habitat=-1;
    UnmarkObjs(&listheadobjs);
    (*pcv)->selected=TRUE;
    mouserelease=FALSE;
    return;
    break;
  default:
    break;
  }

  /* Define views. Four types of view: SPACE,MAP,PLANET,SHIP*/
  view=GetView();

  if(view==VIEW_NONE){
    //    printf("VIEW: NONE\n");
    return;
  }
  
  if(view==VIEW_SPACE||view==VIEW_PLANET||view==VIEW_MAP){
    
    if(keys.mleft==FALSE){
      if(mouserelease){ /* mouse release */
	/* region 0: space & map, >0 planet*/
	if(region.rect.width==0 || region.rect.height==0){
	  /*reset*/
	  region.rect.width=region.rect.height=0;
	  if(keys.ctrl==FALSE){
	    UnmarkObjs(&listheadobjs);
	  }
	  cv->selected=TRUE;

	  mouserelease=FALSE;

	  /***** mouse selection, one click *****/

	  if(view==VIEW_PLANET){
	    region.rect.y=GameParametres(GET,GHEIGHT,0)-region.rect.y;
	  }
	  
	  Window2Real(cv,view,region.rect.x,region.rect.y,&x0,&y0); 
	  Window2Real(cv,view,region.rect.x+region.rect.width, 
		      region.rect.y+region.rect.height, 
		      &x1,&y1); 
	  
	  region.rect.x=x0; 
	  region.rect.y=y0; 
	  region.rect.width=0; 
	  region.rect.height=0;

	  switch(view){
	  case VIEW_MAP:
	  case VIEW_SPACE:
	    region.habitat=0;
	    break;
	  case VIEW_PLANET:
	    if(cv!=NULL)region.habitat=cv->in->id;
	    break;
	  default:
	    break;
	  }

	  if(region.habitat>=0){
	    cv0=SelectOneShip(&listheadobjs,region,cv,keys.ctrl);
	    if(cv0==NULL)cv0=cv;
	    if(cv0!=NULL){
	      if(keys.ctrl==TRUE){
		if(cv0->selected==TRUE){
		  if(CountSelected(&listheadobjs,cv->player)>1){
		    cv0->selected=cv0->selected==TRUE?FALSE:TRUE;
		    if(cv==cv0 && cv0->selected==FALSE){
		      *pcv=FirstSelected(&listheadobjs,cv->player);
		      if(*pcv!=NULL){
			(*pcv)->selected=TRUE;
			habitat.type=(*pcv)->habitat;
			habitat.obj=(*pcv)->in;
		      }
		    }
		  }
		}
		else{
		  *pcv=cv0;
		  (*pcv)->selected=TRUE;
		  habitat.type=(*pcv)->habitat;
		  habitat.obj=(*pcv)->in;
		}
	      }
	      else{
		cv->selected=FALSE;
		*pcv=cv0;
		(*pcv)->selected=TRUE;
		habitat.type=(*pcv)->habitat;
		habitat.obj=(*pcv)->in;
	      }
	    }
	    else{
	      fprintf(stderr,"WARNING SelectionBox() cv0=NULL\n");
	      //		exit(-1); // HERE PRODUCTION this never must happen
	    }
	  }
	  
	  /***** --mouse selection, one click *****/

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

	  switch(view){
	  case VIEW_MAP:
	  case VIEW_SPACE:
	    region.habitat=0;
	    break;
	  case VIEW_PLANET:
	    if(cv!=NULL)region.habitat=cv->in->id;
	    break;
	  default:
	    break;
	  }

	  
	  if(region.habitat==0){ /* free space */
	    /* window to real coordinates */
	  }
	  if(region.habitat>0){
	    region.rect.y=GameParametres(GET,GHEIGHT,0)-region.rect.y;
	  }

	  Window2Real(cv,view,region.rect.x,region.rect.y,&x0,&y0); 
	  Window2Real(cv,view,region.rect.x+region.rect.width, 
		      region.rect.y+region.rect.height, 
		      &x1,&y1); 
	  
	  region.rect.x=x0;
	  region.rect.y=y0;
	  region.rect.width=x1-x0;
	  region.rect.height=y1-y0;

	  if(region.habitat>=0){
	    cv0=MarkObjs(&listheadobjs,region,*pcv,keys.ctrl);
	    if(cv0!=NULL){
	      *pcv=cv0;
	      (*pcv)->selected=TRUE;
	      habitat.type=(*pcv)->habitat;
	      habitat.obj=(*pcv)->in;
	    }
	  }
	  n=CountSelected(&listheadobjs,cv->player);
	  if(n<6){
	    printf("Selected spaceships:\n");
	    PrintSelected(&listheadobjs); 
	    if(n==0)printf("NONE\n");
	  }
	  else{
	    printf("Selected %d spaceships.\n",n);
	  }
	  if(n>0){
	    snprintf(stmess,MAXTEXTLEN,"Selected %d spaceships.",n);
	    ShellTitle(2,NULL,NULL,NULL,NULL,0,0);
	    ShellTitle(1,stmess,NULL,NULL,NULL,0,0);
	  }
	}
	mouserelease=FALSE;
      }
    }
    
    if(keys.mleft==TRUE){
      if(mouserelease==0){
	MousePos(GET,&x,&y);
	region.rect.x=x;
	region.rect.y=y;
	region.rect.width=region.rect.height=0;
	region.habitat=-1; /* nothing selected */

	mouserelease=TRUE;
      }
      MousePos(GET,&x,&y);
      region.rect.width=x-region.rect.x;
      region.rect.height=y-region.rect.y;

      switch(view){
      case VIEW_SPACE:
      case VIEW_MAP:
	region.habitat=0;
	break;
      case VIEW_PLANET:
	if(cv!=NULL)region.habitat=cv->in->id;
	break;
      default:
	break;
      }
    }
    
    
    if(region.rect.width!=0){
      switch(view){
      case VIEW_PLANET:
	if(region.habitat>0){
	  DrawSelectionBox(pixmap,color,view,region,cv);
	}
	break;
      case VIEW_SPACE:
      case VIEW_MAP:
	if(region.habitat==0){
	  DrawSelectionBox(pixmap,color,view,region,cv);
	}
	break;

      default:
	break;
      }
      /*
	check if cv in inside the region 
	if not : delete region
      */
      if(keys.mleft==FALSE && *pcv!=NULL){
	if((*pcv)->habitat==H_SPACE){
	  if(IsInRegion(*pcv,region)){
	    region.rect.width=region.rect.height=0;
	    region.habitat=-1;
	  }
	  return;
	}
      }
    }
  }
}



char Keyval2Char(guint keyval){
  char kname[24];
  char c=0;

  if(keyval>31 && keyval < 256  ){
    return((char)keyval);
  }

  snprintf(kname,24,"%s",gdk_keyval_name(keyval));
  //  printf("kv2c: (%s)%d\n",kname,keyval);

  if(strlen(kname)==1)return(kname[0]);
  if(strlen(kname)==0)return((char)0);


  switch (keyval){
  case 65456: // KP_0:
    c='0';
    break;
  case 65457: // KP_1:
    c='1';
    break;
  case 65458: // KP_2:
    c='2';
    break;
  case 65459: // KP_3:
    c='3';
    break;
  case 65460: // KP_4:
    c='4';
    break;
  case 65461: // KP_5:
    c='5';
    break;
  case 65462: // KP_6:
    c='6';
    break;
  case 65463: // KP_7:
    c='7';
    break;
  case 65464: // KP_8:
    c='8';
    break;
  case 65465: // KP_8:
    c='9';
    break;
   }
  return(c);
}

int Keystrokes(int action,guint *kval,char *c){
  /* 
     grab all the keystrokes in the cad text
     returns the number of chars manipulated.
   */
  int i,j;
  static int n=0;
  static guint val[MAXTEXTLEN];
  char kname[24];
  int m=0;
  char endln='\0';
  char d;

  switch(action){
  case RESET: /*  RESET to 0 */
    n=0;
    break;
  case ADD: /*  ADD */
    if(kval!=NULL){
      val[n]=*kval;
      n++;
      if(n>MAXTEXTLEN-1)n=MAXTEXTLEN-1;
      m=1;
    }
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
      if(kval!=NULL)*kval=val[n-1];
      if(c!=NULL){
	snprintf(kname,24,"%s",gdk_keyval_name(val[n-1]));
	*c=Keyval2Char(val[n-1]);
	//	memcpy(c,kname,sizeof(char));
      }

      m=1; 
    }
    else m=0;
    break;
  case LOAD: /* returns a cad of all keys pressed */
    if(c!=NULL){
      for(i=0,j=0;i<n;i++){
	d=Keyval2Char(val[i]);
	if(d!=0){
	  memcpy(&c[j],&d,sizeof(char));
	  j++;
	}
      }
      memcpy(&c[j],&endln,sizeof(char));
      m=j;
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
    if(n==MAXTEXTLEN-1){
      endw=1;
      endl=1;
      strncpy(&arg1[m],"\0",1);
    }
  }

  if(inw)nargs=1;

  if(endl){
    return(nargs);
  }
 
  /* second arg */

  inw=0;
  m=0;  
  strncpy(&a,&cad[n],1);
  while(!endl){
    switch(a){
    case '\0':
      strncpy(&arg2[m],"\0",1);
      endl=1;
      break;
    case ' ':
    case ',':
    case '\t':
      if(inw){
	strncpy(&arg1[m],"\0",1);
	endl=1;
      }
      break;
    default:
      inw=1;
      if(nargs<2)nargs++;
      strncpy(&arg2[m],&a,1);
      m++;
      break;
    }
    n++;
    strncpy(&a,&cad[n],1);
    if(n==MAXTEXTLEN-1){
      endl=1;
      strncpy(&arg1[m],"\0",1);
    }
  }
  return(nargs);
}


void ShellTitle(int order,char *mess,GdkPixmap *pixmap,GdkGC *color,GdkFont *font,int x,int y){
  static char cad0[64]="O: Introduce command";
  static char cad1[64]="";
  static int n_mess=0;
  char *cad;

  if(mess!=NULL)
  switch (order){
    case 0:
      break;
    case 1:
      if(mess!=NULL && n_mess<=0){
	strncpy(cad1,mess,64);
	n_mess=60;
	return;
      }
      break;
    case 2:
      n_mess=0;
      return;
      break;
    default:
      break;
    }

  cad=cad0;
  if(n_mess>0){
    cad=cad1;
    n_mess--;
  }
  else{
    n_mess=0;
  }

  DrawString(pixmap,font,color,x,y,cad);
  return;
}


int GetView(void){
  /* Define views. Four types of view: SPACE,MAP,PLANET,SHIP*/
  int view;

  view=VIEW_NONE;
  
  if(gdraw.map==TRUE){
    view=VIEW_MAP;
  }
  else{
    switch(habitat.type){
    case H_SPACE:
      view=VIEW_SPACE;
      break;
    case H_PLANET:
      view=VIEW_PLANET;
	break;
    case H_SHIP:
      view=VIEW_NONE;
      break;
    default:
      view=VIEW_NONE;
      break;
    }
  }
  return(view);
}
