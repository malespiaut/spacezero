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
#include <stdio.h>
#include <string.h>
#include "menu.h"


extern int g_memused;
extern struct Parametres param;


struct MenuHead *MenuHeadNew(char *title){
  struct MenuHead *mh;

  mh=malloc(sizeof(struct MenuHead));
  if(mh==NULL){
    fprintf(stderr,"ERROR in malloc MenuHeadNew()\n");
    exit(-1);
  }
  strcpy(mh->title,"");
  strncat(mh->title,title,MAXTEXTLEN);
  strncpy(&mh->title[MAXTEXTLEN-1],"\0",1);
  mh->n=0;
  mh->nactive=0;
  mh->active=ITEM_ST_FALSE;
  mh->firstitem=NULL;
  return(mh);
}

int Add2MenuHead(struct MenuHead *mhead,struct MenuItem *item0,char *title){
  /* 
     add the cad to the list.
  */
  struct MenuItem *item;
  struct MenuItem *itemnew;

  if(mhead->firstitem==NULL){ /* first item */
    mhead->firstitem=malloc(sizeof(struct MenuItem));
    if(mhead->firstitem==NULL){
      fprintf(stderr,"ERROR in malloc Add2MenuHead()\n");
      exit(-1);
    }
    itemnew=mhead->firstitem;
  }
  else{   /* Add to the end of the list */
    item=mhead->firstitem;
    while(item->next!=NULL){
      item=item->next;
    }
    item->next=malloc(sizeof(struct MenuItem));
    if(item->next==NULL){
      fprintf(stderr,"ERROR in malloc Add2MenuHead()\n");
      exit(-1);
    }
    itemnew=item->next;
  }

  g_memused+=sizeof(struct MenuItem);
  itemnew->id=item0->id;
  itemnew->type=item0->type;
  itemnew->active=item0->active;
  strcpy(itemnew->text,"");
  strncpy(itemnew->text,title,MAXTEXTLEN-1);
  strcpy(itemnew->value,"");
  itemnew->next=NULL;
  itemnew->nexthead=item0->nexthead;
  return (0);
}



void PrintMenuHead(struct MenuHead *mh){

  struct MenuItem *item;

  printf("--------------\nprintfmenuhead\n");
  if(mh==NULL)return;

  item=mh->firstitem;
  while (item != NULL){
    printf("%s\n",item->text);
    item=item->next;
  }
  printf("printmenuhead end\n--------------\n");
}


void PrintAllMenu(struct MenuHead *mh){

  struct MenuItem *item;

  printf("--------------\nprintfallmenuhead\n");
  if(mh==NULL)return;

  item=mh->firstitem;
  while (item != NULL){
    printf("%s\n",item->text);
    if(item->nexthead!=NULL){
      printf("LINK\n");
      PrintMenuHead(item->nexthead);
    }
    item=item->next;
  }
  printf("printallmenuhead end\n--------------\n");
}


char *GetOptionValue(int id){


  static char point[MAXTEXTLEN];
  strcpy(point,"");
  switch(id){
  case ITEM_sound:
    if(param.sound==TRUE)
      sprintf(point,"YES");
    else
      sprintf(point,"NO");
    break;
  case ITEM_music:
    if(param.music==TRUE)
      sprintf(point,"YES");
    else
      sprintf(point,"NO");
    break;
  case ITEM_k:
    if(param.kplanets==TRUE)
      sprintf(point,"YES");
    else
      sprintf(point,"NO");
    break;
  case ITEM_cooperative:
    if(param.cooperative==TRUE)
      sprintf(point,"YES");
    else
      sprintf(point,"NO");
    break;
  case ITEM_compcooperative:
    if(param.compcooperative==TRUE)
      sprintf(point,"YES");
    else
      sprintf(point,"NO");
    break;
  case ITEM_queen:
    if(param.queen==TRUE)
      sprintf(point,"YES");
    else
      sprintf(point,"NO");
    break;
  case ITEM_pirates:
    if(param.pirates==TRUE)
      sprintf(point,"YES");
    else
      sprintf(point,"NO");
    break;
  case ITEM_ip:
    /* printf("IP:%s\n",param.IP); */
    sprintf(point,"%s",param.IP);
    break;
  case ITEM_port:
    /* printf("port:%d\n",param.port); */
    sprintf(point,"%d",param.port);
    break;
  case ITEM_name:
    sprintf(point,"%s",param.playername);
    break;
  case ITEM_p:
    sprintf(point,"%d",param.nplayers);
    break;
  case ITEM_n:
    sprintf(point,"%d",param.nplanets);
    break;
  case ITEM_g:
    sprintf(point,"%d",param.ngalaxies);
    break;
  case ITEM_l:
    sprintf(point,"%d",param.ul);
    break;
  case ITEM_geom:
    sprintf(point,"%s",param.geom);
    break;

  case ITEM_start:
  case ITEM_quit:
  case ITEM_0:
    break;

  default:
    fprintf(stderr,"WARNING: GetOptionValue() id: %d unknown.\n",id);
    break;
  }
  return(point);
}


char *GetTextEntry(struct MenuItem *item,char *text){

  char textentry[MAXTEXTLEN];
  static int id =0;
  if(id!=item->id){
    printf("reset ks %d %d \n",item->id,id);
    Keystrokes(RESET,NULL);
  }
  id=item->id;
  //  strcpy(par,"");
  Keystrokes(LOAD,textentry);
  strcpy(text,"");
  strcpy(text,textentry);
  return(text);
}

struct MenuHead *CreateMenu(void){
  
  struct MenuHead *mhead;
  struct MenuHead *moptions;
  struct MenuHead *mmultiplayeroptions;
  struct MenuHead *mgeneraloptions;
  struct MenuHead *mgameoptions;

  struct MenuItem item;

  /****** create all menu headers *******/

  mhead=MenuHeadNew("");
  moptions=MenuHeadNew("OPTIONS");
  mmultiplayeroptions=MenuHeadNew("MULTIPLAYER OPTIONS");
  mgeneraloptions=MenuHeadNew("GENERAL OPTIONS");
  mgameoptions=MenuHeadNew("GAME OPTIONS");

  
  /******* main menu *********/

  item.id=0;
  item.type=MENUITEMTEXT;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=moptions;   /* linking with options menu */
  Add2MenuHead(mhead,&item,"OPTIONS");

  item.id=ITEM_start;
  item.type=MENUITEMACTION;
  item.active=ITEM_ST_SHOW;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mhead,&item,"START GAME");

  item.id=ITEM_quit;
  item.type=MENUITEMACTION;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mhead,&item,"QUIT GAME");


  /******* options menu *********/

  item.id=0;
  item.type=MENUITEMTEXT;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=mgeneraloptions;/* link with general options menu */
  Add2MenuHead(moptions,&item,"General Options");


  item.id=0;
  item.type=MENUITEMTEXT;
  strcpy(item.text,"");
  strncat(item.text,"Game Options",MAXTEXTLEN);
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=mgameoptions;/* link with game options menu */
  Add2MenuHead(moptions,&item,"Game Options");


  item.id=0;
  item.type=MENUITEMTEXT;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=mmultiplayeroptions;/* link with multiplayer options menu */;
  Add2MenuHead(moptions,&item,"Multiplayer Options");

  item.id=ITEM_default;
  item.type=MENUITEMACTION;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(moptions,&item,"Set Default Options");

  /***** menu general options *********/

  item.id=ITEM_name;
  item.type=MENUITEMTEXTENTRY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgeneraloptions,&item,"Name:");

  item.id=ITEM_sound;
  item.type=MENUITEMBOOL;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgeneraloptions,&item,"Sound");

  item.id=ITEM_music;
  item.type=MENUITEMBOOL;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgeneraloptions,&item,"Music");

  item.id=ITEM_geom;
  item.type=MENUITEMTEXTENTRY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgeneraloptions,&item,"Window geometry: ");


  /****** menu game options ******/

  item.id=ITEM_p;
  item.type=MENUITEMTEXTENTRY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,"Number of players: ");

  item.id=ITEM_n;
  item.type=MENUITEMTEXTENTRY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,"Number of planets: ");

  item.id=ITEM_g;
  item.type=MENUITEMTEXTENTRY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,"Number of galaxies: ");

  item.id=ITEM_l;
  item.type=MENUITEMTEXTENTRY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,"Size of Universe: ");

  item.id=ITEM_k;
  item.type=MENUITEMBOOL;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,"Planets are known: ");

  item.id=ITEM_pirates;
  item.type=MENUITEMBOOL;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,"Pirates: ");

  item.id=ITEM_cooperative;
  item.type=MENUITEMBOOL;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,"Cooperative mode: ");

  item.id=ITEM_compcooperative;
  item.type=MENUITEMBOOL;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,"Computer cooperative mode: ");

  item.id=ITEM_queen;
  item.type=MENUITEMBOOL;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,"Queen mode: ");

  /***** multiplayer menu options *****/
  item.id=ITEM_ip;
  item.type=MENUITEMTEXTENTRY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mmultiplayeroptions,&item,"IP address:");

  item.id=ITEM_port;
  item.type=MENUITEMTEXTENTRY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mmultiplayeroptions,&item,"port:");

  item.id=ITEM_server;
  item.type=MENUITEMACTION;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mmultiplayeroptions,&item,"Start Server.");

  item.id=ITEM_client;
  item.type=MENUITEMACTION;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mmultiplayeroptions,&item,"Connect to Server.");


  /*  PrintMenuHead(moptions); */
  return(mhead);
}


int UpdateMenu(struct MenuHead *mhead,struct MenuHead *mactual,struct Keys *keys){
  int ret=0;

  if(keys->down){
    /* printf("kdown\n"); */
    MenuDown(mactual);
    keys->down=FALSE;
  }

  if(keys->up){
    MenuUp(mactual);
    keys->up=FALSE;
  }

  if(keys->enter){
    ret=MenuEnter(mactual);
    keys->enter=FALSE;
    return(ret);
  }

  if(keys->esc){
    if(mactual!=mhead){
      MenuEsc(mactual);
    }
    keys->esc=FALSE;
    return(MENUESC);
  }

  if(keys->back){
    Keystrokes(DELETELAST,NULL);
    keys->back=FALSE;

  }
  return(ret);
}

void MenuDown(struct MenuHead *mhead){
  struct MenuItem *item;
  int sw=0;

  /* printf("menudown\n"); */

  item=mhead->firstitem;
  while(item!=NULL){
    if(sw){
      item->active=ITEM_ST_SHOW;
      return;
    }
    if(item->active){
      if(item->next!=NULL){
	item->active=ITEM_ST_FALSE;
	sw++;
      }
    }
    item=item->next;
  }
  return;
}


void MenuUp(struct MenuHead *mhead){
  struct MenuItem *item,*last;

  /* printf("menuup\n"); */

  item=mhead->firstitem;
  last=item;
  while(item!=NULL){
    if(item->active){
      item->active=ITEM_ST_FALSE;
      last->active=ITEM_ST_SHOW;

    }
    last=item;
    item=item->next;
  }
  return;
}

int MenuEnter(struct MenuHead *mhead){
  struct MenuItem *item;
  int sw=0;
  char text[MAXTEXTLEN];

  /* printf("menuenter %s\n",mhead->title); */

  item=mhead->firstitem;
  while(item!=NULL){
    if(sw){
      item->active=ITEM_ST_SHOW;
      return(0);
    }
    if(item->active){

      if(item->nexthead!=NULL){
	if(item->nexthead->firstitem!=NULL){
	  item->nexthead->active=ITEM_ST_SHOW;
	  MenuItemActive(item->nexthead,FALSE);
	  item->nexthead->firstitem->active=ITEM_ST_SHOW;
	}
      }
      else{
	switch(item->type){
	case MENUITEMBOOL:
	case MENUITEMTEXTENTRY:
	case MENUITEMTEXT:
	  strcpy(text,"");
	  Keystrokes(LOAD,text);
	  Funct01(item,text);
	  break;
	default:
	  break;
	}
      }
      if(item->type==MENUITEMACTION){
	if(item->id==ITEM_server){
	  strcpy(item->text,"Waiting for player...");
	}
	if(item->id==ITEM_start){
	  if(param.server==TRUE){
	    strcpy(item->text,"Waiting for player...");
	  }
	  if(param.client==TRUE){
	    strcpy(item->text,"Waiting for server...");
	  }
	}
	return(item->id);
      }
    }
    item=item->next;
  }
  return(0);
}


void MenuEsc(struct MenuHead *mhead){
  struct MenuItem *item;
  int sw=0;
  /* printf("menuesc\n"); */
  Keystrokes(RESET,NULL);	

  item=mhead->firstitem;
  while(item!=NULL){
    if(item->active>=ITEM_ST_EDIT)sw=1;
    if(item->active>ITEM_ST_SHOW)
      item->active--;
    
    item=item->next;
  }
  if(sw==0)
    mhead->active=FALSE;
  return;
}


struct MenuHead *SelectMenu(struct MenuHead *mh){

  struct MenuItem *item;
  struct MenuHead *mhret;
 
  mhret=mh;
  item=mh->firstitem;
  while (item != NULL){
    if(item->active==ITEM_ST_SHOW){
      if(item->nexthead!=NULL){
	if(item->nexthead->active){
	  return(SelectMenu(item->nexthead));
	}
      }
    }
    item=item->next;
  }

  return(mhret);
}


void MenuItemActive(struct MenuHead *mhead,int value){
  /*
    set active to value in all items of mhead.
   */
  struct MenuItem *item;

  item=mhead->firstitem;
  while(item!=NULL){
    item->active=value;
    item=item->next;
  }
  return;
}

void Funct01(struct MenuItem *item,char *value){
  /*
    Applied the changes.
   */
  int tmparg;

  switch(item->type){
  case MENUITEMBOOL:
    switch(item->id){
    case ITEM_sound:
      param.sound=param.sound==TRUE?FALSE:TRUE;
      break;
    case ITEM_music:
      param.music=param.music==TRUE?FALSE:TRUE;
      break;
    case ITEM_k:
      param.kplanets=param.kplanets==TRUE?FALSE:TRUE;
      break;
    case ITEM_cooperative:
      param.cooperative=param.cooperative==TRUE?FALSE:TRUE;
      break;
    case ITEM_compcooperative:
      param.compcooperative=param.compcooperative==TRUE?FALSE:TRUE;
      break;
    case ITEM_queen:
      param.queen=param.queen==TRUE?FALSE:TRUE;
      break;
    case ITEM_pirates:
      param.pirates=param.pirates==TRUE?FALSE:TRUE;
      break;

    default:
      break;
    }
    break;

  case MENUITEMTEXT:
    break;
  case MENUITEMTEXTENTRY:
    item->active++;
    if(item->active==ITEM_ST_EDIT)Keystrokes(RESET,NULL);

    switch(item->id){
    case ITEM_name:
      if(item->active==ITEM_ST_UPDATE){
	strncpy(param.playername,value,MAXTEXTLEN);
	Keystrokes(RESET,NULL);
	item->active=ITEM_ST_SHOW;
      }
      break;

    case ITEM_geom:
      if(item->active==ITEM_ST_UPDATE){
	strncpy(param.geom,value,MAXTEXTLEN);
	Keystrokes(RESET,NULL);
	item->active=ITEM_ST_SHOW;
      }
      break;

    case ITEM_p:
      if(item->active==ITEM_ST_UPDATE){
	tmparg=param.nplayers;
	param.nplayers=atoi(value);
	if(CheckArgs(param)){
	  fprintf(stderr,"WARNING: Invalid value\n");
	  param.nplayers=tmparg;
	}
	else{
	  Keystrokes(RESET,NULL);
	}
	item->active=ITEM_ST_SHOW;
      }
      break;
    case ITEM_n:
      if(item->active==ITEM_ST_UPDATE){
	tmparg=param.nplanets;
	param.nplanets=atoi(value);
	if(CheckArgs(param)){
	  fprintf(stderr,"WARNING: Invalid value\n");
	  param.nplanets=tmparg;
	}
	else{
	  Keystrokes(RESET,NULL);
	}
	item->active=ITEM_ST_SHOW;
      }
      break;

    case ITEM_g:
      if(item->active==ITEM_ST_UPDATE){
	tmparg=param.ngalaxies;
	param.ngalaxies=atoi(value);
	if(CheckArgs(param)){
	  fprintf(stderr,"WARNING: Invalid value\n");
	  param.ngalaxies=tmparg;
	}
	else{
	  Keystrokes(RESET,NULL);
	}
	item->active=ITEM_ST_SHOW;
      }
      break;

    case ITEM_l:
      if(item->active==ITEM_ST_UPDATE){
	tmparg=param.ul;
	param.ul=atoi(value);
	if(CheckArgs(param)){
	  fprintf(stderr,"WARNING: Invalid value\n");
	  param.ul=tmparg;
	}
	else{
	  Keystrokes(RESET,NULL);
	}
	item->active=ITEM_ST_SHOW;
      }
      break;

    case ITEM_ip:
      if(item->active==ITEM_ST_UPDATE){
	strncpy(param.IP,value,MAXTEXTLEN);
	Keystrokes(RESET,NULL);
	item->active=ITEM_ST_SHOW;
      }
      break;
    
    case ITEM_port:
      if(item->active==ITEM_ST_UPDATE){
	tmparg=param.port;
	param.port=atoi(value);
	if(CheckArgs(param)){
	  fprintf(stderr,"WARNING: Invalid value\n");
	  param.port=tmparg;
	}
	else{
	  Keystrokes(RESET,NULL);
	}
	item->active=ITEM_ST_SHOW;
      }
      break;
    
    case ITEM_server:
      printf("Starting server\n");
      break;
    
    case ITEM_client:
      printf("Starting client\n");
      break;
      
    default:
      printf("Funct01()id: %d not defined\n",item->id);
      break;
    }

    break;
  default:
    printf("Funct01()type: %d not defined\n",item->type);
    break;
    
  }
}
