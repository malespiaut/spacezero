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

struct MenuHead *MenuHeadNew(char *title){

  struct MenuHead *mh;

  mh=malloc(sizeof(struct MenuHead));
  if(mh==NULL){
    fprintf(stderr,"ERROR in malloc MenuHeadNew()\n");
    exit(-1);
  }
  strcpy(mh->title,"");
  strncat(mh->title,title,MAXMENULEN);
  strncpy(&mh->title[MAXMENULEN-1],"\0",1);
  mh->n=0;
  mh->nactive=0;
  mh->active=FALSE;
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
  strncpy(itemnew->text,title,MAXMENULEN-1);
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

  printf("--------------\nprintfmenuhead\n");
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

  printf("printmenuhead end\n--------------\n");
}


char *GetOptionValue(int id){
  static char point[MAXMENULEN];
  /*
    #define ITEM_0 0
    #define ITEM_h 1
    #define ITEM_g 2
    #define ITEM_n 3
    #define ITEM_p 4
    #define ITEM_t 5
    #define ITEM_l 6
    #define ITEM_s 7
    #define ITEM_c 8
    #define ITEM_ip 9
    #define ITEM_port 10
    #define ITEM_name 11
    #define ITEM_sound 12
    #define ITEM_music 13
    #define ITEM_k 14
    #define ITEM_cooperative 15
    #define ITEM_compcooperative 16
    #define ITEM_queen 17
    #define ITEM_pirates 18
    #define ITEM_font 19
    #define ITEM_geom 20
    #define ITEM_start 21
    #define ITEM_quit 22 
  */

  strcpy(point,"");
  switch(id){
  case ITEM_sound:
    if(GameParametres(GET,GSOUND,0)==TRUE)
      sprintf(point,"YES");
    else
      sprintf(point,"NO");
    break;

  case ITEM_music:
    if(GameParametres(GET,GMUSIC,0)==TRUE)
      sprintf(point,"YES");
    else
      sprintf(point,"NO");
    break;
  case ITEM_k:
    if(GameParametres(GET,GKPLANETS,0)==TRUE)
      sprintf(point,"YES");
    else
      sprintf(point,"NO");
    break;
  case ITEM_cooperative:
    if(GameParametres(GET,GCOOPERATIVE,0)==TRUE)
      sprintf(point,"YES");
    else
      sprintf(point,"NO");
    break;
  case ITEM_compcooperative:
    if(GameParametres(GET,GCOMPCOOPERATIVE,0)==TRUE)
      sprintf(point,"YES");
    else
      sprintf(point,"NO");
    break;
  case ITEM_queen:
    if(GameParametres(GET,GQUEEN,0)==TRUE)
      sprintf(point,"YES");
    else
      sprintf(point,"NO");
    break;
  case ITEM_pirates:
    if(GameParametres(GET,GPIRATES,0)==TRUE)
      sprintf(point,"YES");
    else
      sprintf(point,"NO");
    break;
  default:
    break;
  }
  return(point);
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
  item.active=0;
  strcpy(item.value,"");
  item.nexthead=moptions;   /* linking with options menu */
#if TEST
  Add2MenuHead(mhead,&item,"OPTIONS");
#endif

  item.id=ITEM_start;
  item.type=MENUITEMTEXT;
  item.active=1;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mhead,&item,"START GAME");

  item.id=ITEM_quit;
  item.type=MENUITEMTEXT;
  item.active=0;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mhead,&item,"QUIT GAME");


  /******* options menu *********/

  item.id=0;
  item.type=MENUITEMTEXT;
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=mgeneraloptions;/* link with general options menu */
  Add2MenuHead(moptions,&item,"General Options");


  item.id=0;
  item.type=MENUITEMTEXT;
  strcpy(item.text,"");
  strncat(item.text,"Game Options",MAXMENULEN);
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=mgameoptions;/* link with game options menu */
  Add2MenuHead(moptions,&item,"Game Options");


  item.id=0;
  item.type=MENUITEMTEXT;
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=mmultiplayeroptions;/* link with multiplayer options menu */;
  Add2MenuHead(moptions,&item,"Multiplayer Options");

  /***** menu general options *********/

  item.id=ITEM_name;
  item.type=MENUITEMTEXT;
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgeneraloptions,&item,"Name");

  item.id=ITEM_sound;
  item.type=MENUITEMBOOL;
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgeneraloptions,&item,"Sound");

  item.id=ITEM_music;
  item.type=MENUITEMBOOL;
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgeneraloptions,&item,"Music");

  item.id=0;
  item.type=MENUITEMTEXT;
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgeneraloptions,&item,"Window geometry: ");


  /****** menu game options ******/

  item.id=0;
  item.type=MENUITEMTEXT;
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,"Number of planets: ");

  item.id=0;
  item.type=MENUITEMTEXT;
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,"Number of galaxies: ");

  item.id=0;
  item.type=MENUITEMTEXT;
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,"Size of Universe: ");

  item.id=ITEM_k;
  item.type=MENUITEMBOOL;
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,"Planets are known: ");

  item.id=ITEM_pirates;
  item.type=MENUITEMBOOL;
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,"Pirates: ");

  item.id=ITEM_cooperative;
  item.type=MENUITEMBOOL;
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,"Cooperative mode: ");

  item.id=ITEM_compcooperative;
  item.type=MENUITEMBOOL;
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,"Computer cooperative mode: ");

  item.id=ITEM_queen;
  item.type=MENUITEMBOOL;
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,"Queen mode: ");

  /***** multiplayer menu options *****/
  item.id=0;
  item.type=MENUITEMTEXT;
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mmultiplayeroptions,&item,"IP address:");

  item.id=0;
  item.type=MENUITEMTEXT;
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mmultiplayeroptions,&item,"port:");

  item.id=0;
  item.type=MENUITEMTEXT;
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mmultiplayeroptions,&item,"Start Server.");

  item.id=0;
  item.type=MENUITEMTEXT;
  item.active=FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mmultiplayeroptions,&item,"Connect to Server.");


  PrintMenuHead(moptions);
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

  return(ret);
}

void MenuDown(struct MenuHead *mhead){
  struct MenuItem *item;
  int sw=0;

  /* printf("menudown\n"); */

  item=mhead->firstitem;
  while(item!=NULL){
    if(sw){
      item->active=TRUE;
      return;
    }
    if(item->active){
      if(item->next!=NULL){
	item->active=FALSE;
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
      item->active=FALSE;
      last->active=TRUE;

    }
    last=item;
    item=item->next;
  }
  return;
}

int MenuEnter(struct MenuHead *mhead){
  struct MenuItem *item;
  int sw=0;

  /* printf("menuenter %s\n",mhead->title); */

  item=mhead->firstitem;
  while(item!=NULL){
    if(sw){
      item->active=TRUE;
      return(0);
    }
    if(item->active){

      if(item->nexthead!=NULL){
	if(item->nexthead->firstitem!=NULL){
	  item->nexthead->active=TRUE;
	  MenuItemActive(item->nexthead,FALSE);
	  item->nexthead->firstitem->active=TRUE;
	}
      }
      else{
	switch(item->type){
	case MENUITEMBOOL:
	  Funct01(item);
	  /* printf("BOOL\n"); */
	  break;
	default:
	  break;
	}
      }
      if(item->id==ITEM_start)return(ITEM_start);
      if(item->id==ITEM_quit)return(ITEM_quit);
    }
    item=item->next;
  }
  return(0);
}

void MenuEsc(struct MenuHead *mhead){
  struct MenuItem *item;

  /* printf("menuesc\n"); */

  item=mhead->firstitem;
  while(item!=NULL){
    item->active=FALSE;
    item=item->next;
  }
  mhead->active=FALSE;
  return;
}


struct MenuHead *SelectMenu(struct MenuHead *mh){

  struct MenuItem *item;
  struct MenuHead *mhret;
  

  mhret=mh;

  item=mh->firstitem;
  while (item != NULL){
    if(item->active==TRUE){
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

void Funct01(struct MenuItem *item){


  switch(item->type){
  case MENUITEMBOOL:
    switch(item->id){
    case ITEM_sound:
      if(GameParametres(GET,GSOUND,0)==TRUE)
	GameParametres(SET,GSOUND,FALSE);
      else
	GameParametres(SET,GSOUND,TRUE);
      break;
      
    case ITEM_music:
      if(GameParametres(GET,GMUSIC,0)==TRUE)
	GameParametres(SET,GMUSIC,FALSE);
      else
	GameParametres(SET,GMUSIC,TRUE);
      break;
    case ITEM_k:
      if(GameParametres(GET,GKPLANETS,0)==TRUE)
	GameParametres(SET,GKPLANETS,FALSE);
      else
	GameParametres(SET,GKPLANETS,TRUE);
      break;
    case ITEM_cooperative:
      if(GameParametres(GET,GCOOPERATIVE,0)==TRUE)
	GameParametres(SET,GCOOPERATIVE,FALSE);
      else
	GameParametres(SET,GCOOPERATIVE,TRUE);
      break;
    case ITEM_compcooperative:
      if(GameParametres(GET,GCOMPCOOPERATIVE,0)==TRUE)
	GameParametres(SET,GCOMPCOOPERATIVE,FALSE);
      else
	GameParametres(SET,GCOMPCOOPERATIVE,TRUE);
      break;
    case ITEM_queen:
      if(GameParametres(GET,GQUEEN,0)==TRUE)
	GameParametres(SET,GQUEEN,FALSE);
      else
	GameParametres(SET,GQUEEN,TRUE);
      break;
    case ITEM_pirates:
      if(GameParametres(GET,GPIRATES,0)==TRUE)
	GameParametres(SET,GPIRATES,FALSE);
      else
	GameParametres(SET,GPIRATES,TRUE);
      break;
    default:
      break;
    }

    break;
  }
}
