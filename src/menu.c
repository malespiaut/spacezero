 /*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyright(C) 2006-2013  MRevenga
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "menu.h"
#include "help.h"
#include "shell.h"
#include "sound.h"
#include "functions.h"
#include "locales.h"

extern struct Parametres param;
struct Keys keys;

struct MenuHead *MenuHeadNew(char *title){
  struct MenuHead *mh;
  int ret;

  mh=malloc(sizeof(struct MenuHead));
  if(mh==NULL){
    fprintf(stderr,"ERROR in malloc MenuHeadNew()\n");
    exit(-1);
  }

  ret=snprintf(mh->title,MAXTEXTLEN,"%s",title);
  if(ret>=MAXTEXTLEN){
    fprintf(stderr,"string too long. Truncated to:\"%s\"",title);
  }

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

  MemUsed(MADD,+sizeof(struct MenuItem));
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
    snprintf(point,MAXTEXTLEN,"%d",param.soundvol);
    break;
  case ITEM_music:
    snprintf(point,MAXTEXTLEN,"%d",param.musicvol);
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
  case ITEM_enemyknown:
    if(param.enemyknown==TRUE)
      sprintf(point,"YES");
    else
      sprintf(point,"NO");
    break;
  case ITEM_ip:
    /* printf("IP:%s\n",param.IP); */
    snprintf(point,MAXTEXTLEN,"%s",param.IP);
    break;
  case ITEM_port:
    /* printf("port:%d\n",param.port); */
    snprintf(point,MAXTEXTLEN,"%d",param.port);
    break;
  case ITEM_name:
    snprintf(point,MAXTEXTLEN,"%s",param.playername);
    break;
  case ITEM_p:
    snprintf(point,MAXTEXTLEN,"%d",param.nplayers);
    break;
  case ITEM_n:
    snprintf(point,MAXTEXTLEN,"%d",param.nplanets);
    break;
  case ITEM_g:
    snprintf(point,MAXTEXTLEN,"%d",param.ngalaxies);
    break;
  case ITEM_l:
    snprintf(point,MAXTEXTLEN,"%d",param.ul);
    break;
  case ITEM_geom:
    snprintf(point,MAXTEXTLEN,"%s",param.geom);
    break;

  case ITEM_start:
  case ITEM_quit:
  case ITEM_0:
    break;
  case ITEM_fire:
    snprintf(point,MAXTEXTLEN,"%s",gdk_keyval_name(keys.fire.value));
    break;
  case ITEM_turnleft:
    snprintf(point,MAXTEXTLEN,"%s",gdk_keyval_name(keys.turnleft.value));
    break;
  case ITEM_turnright:
    snprintf(point,MAXTEXTLEN,"%s",gdk_keyval_name(keys.turnright.value));
    break;
  case ITEM_accel:
    snprintf(point,MAXTEXTLEN,"%s",gdk_keyval_name(keys.accel.value));
    break;
  case ITEM_automode:
    snprintf(point,MAXTEXTLEN,"%s",gdk_keyval_name(keys.automode.value));
    break;
  case ITEM_manualmode:
    snprintf(point,MAXTEXTLEN,"%s",gdk_keyval_name(keys.manualmode.value));
    break;
  case ITEM_map:
    snprintf(point,MAXTEXTLEN,"%s",gdk_keyval_name(keys.map.value));
    break;
  case ITEM_order:
    snprintf(point,MAXTEXTLEN,"%s",gdk_keyval_name(keys.order.value));
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
    Keystrokes(RESET,NULL,NULL);
  }
  id=item->id;
  /* strcpy(par,""); */
  Keystrokes(LOAD,NULL,textentry);
  strcpy(text,"");
  strncpy(text,textentry,MAXTEXTLEN);
  return(text);
}

struct MenuHead *CreateMenu(void){
  
  struct MenuHead *mhead;
  struct MenuHead *moptions;
  struct MenuHead *mmultiplayeroptions;
  struct MenuHead *mgeneraloptions;
  struct MenuHead *mgameoptions;
  struct MenuHead *mkeyboard;


  struct MenuItem item;

  /****** create all menu headers *******/

  mhead=MenuHeadNew("");
  moptions=MenuHeadNew(GetLocale(L_HOPTIONS));
  mmultiplayeroptions=MenuHeadNew(GetLocale(L_HMULTIPLAYEROPTIONS));
  mgeneraloptions=MenuHeadNew(GetLocale(L_HGENERALOPTIONS));
  mgameoptions=MenuHeadNew(GetLocale(L_HGAMEOPTIONS));
  mkeyboard=MenuHeadNew(GetLocale(L_HKEYBOARD));

  
  /******* main menu *********/

  item.id=0;
  item.type=MENUITEMTEXT;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=moptions;   /* linking with options menu */
  Add2MenuHead(mhead,&item,GetLocale(L_OPTIONS));

  item.id=ITEM_start;
  item.type=MENUITEMACTION;
  item.active=ITEM_ST_SHOW;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mhead,&item,GetLocale(L_STARTGAME));

  item.id=ITEM_quit;
  item.type=MENUITEMACTION;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mhead,&item,GetLocale(L_QUITGAME));


  /******* options menu *********/

  item.id=0;
  item.type=MENUITEMTEXT;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=mgeneraloptions;/* link with general options menu */
  Add2MenuHead(moptions,&item,GetLocale(L_GENERALOPTIONS));


  item.id=0;
  item.type=MENUITEMTEXT;
  strcpy(item.text,"");
  strncat(item.text,"Game Options",MAXTEXTLEN-strlen(item.text));
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=mgameoptions;/* link with game options menu */
  Add2MenuHead(moptions,&item,GetLocale(L_GAMEOPTIONS));

  item.id=0;
  item.type=MENUITEMTEXT;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=mkeyboard;
  Add2MenuHead(moptions,&item,GetLocale(L_KEYBOARD));

  item.id=0;
  item.type=MENUITEMTEXT;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=mmultiplayeroptions;/* link with multiplayer options menu */;
  Add2MenuHead(moptions,&item,GetLocale(L_MULTIPLAYEROPTIONS));

  item.id=ITEM_default;
  item.type=MENUITEMACTION;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(moptions,&item,GetLocale(L_DEFAULTOPTIONS));

  /***** menu general options *********/

  item.id=ITEM_name;
  item.type=MENUITEMTEXTENTRY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgeneraloptions,&item,GetLocale(L_NAME));

  item.id=ITEM_sound;
  item.type=MENUITEMTEXTENTRY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgeneraloptions,&item,GetLocale(L_SVOL));

  item.id=ITEM_music;
  item.type=MENUITEMTEXTENTRY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgeneraloptions,&item,GetLocale(L_MVOL));

  item.id=ITEM_geom;
  item.type=MENUITEMTEXTENTRY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgeneraloptions,&item,GetLocale(L_WINDOWGEOM));


  /****** menu game options ******/

  item.id=ITEM_p;
  item.type=MENUITEMTEXTENTRY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,GetLocale(L_NUMPLAYERS));

  item.id=ITEM_n;
  item.type=MENUITEMTEXTENTRY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,GetLocale(L_NUMPLANETS));

  item.id=ITEM_g;
  item.type=MENUITEMTEXTENTRY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,GetLocale(L_NUMGALAXIES));

  item.id=ITEM_l;
  item.type=MENUITEMTEXTENTRY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,GetLocale(L_SIZEUNIVERSE));

  item.id=ITEM_k;
  item.type=MENUITEMBOOL;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,GetLocale(L_PLANETKNOWN));

  item.id=ITEM_pirates;
  item.type=MENUITEMBOOL;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,GetLocale(L_PIRATES));

  item.id=ITEM_enemyknown;
  item.type=MENUITEMBOOL;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,GetLocale(L_ENEMIESKNOWN));

  item.id=ITEM_cooperative;
  item.type=MENUITEMBOOL;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,GetLocale(L_COOPMODE));

  item.id=ITEM_compcooperative;
  item.type=MENUITEMBOOL;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,GetLocale(L_COMPMODE));

  item.id=ITEM_queen;
  item.type=MENUITEMBOOL;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mgameoptions,&item,GetLocale(L_QUEENMODE));

  /***** Keyboard Options *****/
  item.id=ITEM_fire;
  item.type=MENUITEMGRABKEY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mkeyboard,&item,GetLocale(L_SHOOT));

  item.id=ITEM_turnleft;
  item.type=MENUITEMGRABKEY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mkeyboard,&item,GetLocale(L_TURNLEFT));

  item.id=ITEM_turnright;
  item.type=MENUITEMGRABKEY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mkeyboard,&item,GetLocale(L_TURNRIGHT));

  item.id=ITEM_accel;
  item.type=MENUITEMGRABKEY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mkeyboard,&item,GetLocale(L_ACCEL));

  item.id=ITEM_manualmode;
  item.type=MENUITEMGRABKEY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mkeyboard,&item,GetLocale(L_MANUALMODE));

  item.id=ITEM_automode;
  item.type=MENUITEMGRABKEY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mkeyboard,&item,GetLocale(L_AUTOMODE));

  item.id=ITEM_map;
  item.type=MENUITEMGRABKEY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mkeyboard,&item,GetLocale(L_MAP));

  item.id=ITEM_order;
  item.type=MENUITEMGRABKEY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mkeyboard,&item,GetLocale(L_MORDER));


  /***** multiplayer menu options *****/
  item.id=ITEM_ip;
  item.type=MENUITEMTEXTENTRY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mmultiplayeroptions,&item,GetLocale(L_IPADDRESS));

  item.id=ITEM_port;
  item.type=MENUITEMTEXTENTRY;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mmultiplayeroptions,&item,GetLocale(L_PORT));

  item.id=ITEM_server;
  item.type=MENUITEMACTION;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mmultiplayeroptions,&item,GetLocale(L_STARTSERVER));

  item.id=ITEM_client;
  item.type=MENUITEMACTION;
  item.active=ITEM_ST_FALSE;
  strcpy(item.value,"");
  item.nexthead=NULL;
  Add2MenuHead(mmultiplayeroptions,&item,GetLocale(L_CONNECTSERVER));


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
    Keystrokes(DELETELAST,NULL,NULL);
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
    if(item->active==ITEM_ST_SHOW){
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
    if(item->active==ITEM_ST_SHOW){
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
  guint keyval;

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
	  Keystrokes(LOAD,NULL,text);
	  Funct01(item,text);
	  break;
	case MENUITEMACTION:
	  break;
	case MENUITEMGRABKEY:

	  strcpy(text,"");
	  Keystrokes(RETURNLAST,&keyval,text);
	  Funct01(item,text);
	  break;
	default:
	  fprintf(stderr,"ERROR MenuEnter(): type %d not implemented\n",item->type);
	  exit(-1);
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
  Keystrokes(RESET,NULL,NULL);	

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
  char a;
  guint keyval;

  switch(item->type){
  case MENUITEMBOOL:
    switch(item->id){
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
    case ITEM_enemyknown:
      param.enemyknown=param.enemyknown==TRUE?FALSE:TRUE;
      break;

    default:
      break;
    }
    break;

  case MENUITEMTEXT:
    break;
  case MENUITEMTEXTENTRY:
    item->active++;
    if(item->active==ITEM_ST_EDIT)Keystrokes(RESET,NULL,NULL);

    switch(item->id){
    case ITEM_sound:
      if(item->active==ITEM_ST_UPDATE){
	tmparg=param.soundvol;
	param.soundvol=atoi(value);
	if(CheckArgs(param)){
	  fprintf(stderr,"WARNING: Invalid value\n");
	  param.soundvol=tmparg;
	}
	else{
	  Keystrokes(RESET,NULL,NULL);
	}
	item->active=ITEM_ST_SHOW;
      }
      break;
    case ITEM_music:
      if(item->active==ITEM_ST_UPDATE){
	int master;
	tmparg=param.musicvol;
	param.musicvol=atoi(value);
	if(CheckArgs(param)){
	  fprintf(stderr,"WARNING: Invalid value\n");
	  param.musicvol=tmparg;
	}

	Keystrokes(RESET,NULL,NULL);

	master=SetMasterVolume(0,VOLGET);
	if(master*100<param.musicvol){
	  SetMasterVolume(param.musicvol/100,VOLSET);
	}

	SetMusicVolume((float)param.musicvol/100,VOLSET);
	item->active=ITEM_ST_SHOW;
      }
      break;

    case ITEM_name:
      if(item->active==ITEM_ST_UPDATE){
	strncpy(param.playername,value,MAXTEXTLEN);
	Keystrokes(RESET,NULL,NULL);
	item->active=ITEM_ST_SHOW;
      }
      break;

    case ITEM_geom:
      if(item->active==ITEM_ST_UPDATE){
	strncpy(param.geom,value,MAXTEXTLEN);
	Keystrokes(RESET,NULL,NULL);
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
	  Keystrokes(RESET,NULL,NULL);
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
	  Keystrokes(RESET,NULL,NULL);
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
	  Keystrokes(RESET,NULL,NULL);
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
	  Keystrokes(RESET,NULL,NULL);
	}
	item->active=ITEM_ST_SHOW;
      }
      break;

    case ITEM_ip:
      if(item->active==ITEM_ST_UPDATE){
	strncpy(param.IP,value,MAXTEXTLEN);
	Keystrokes(RESET,NULL,NULL);
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
	  Keystrokes(RESET,NULL,NULL);
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
      fprintf(stderr,"Funct01()id: %d not defined\n",item->id);
      exit(-1);
      break;
    }
    break;

  case MENUITEMGRABKEY:
    item->active++;
    if(item->active==ITEM_ST_EDIT)Keystrokes(RESET,NULL,NULL);

    if(item->active==ITEM_ST_UPDATE){
      Keystrokes(RETURNLAST,&keyval,&a);
      Keystrokes(RESET,NULL,NULL);

      if(keyval>64 && keyval<91){
	keyval+=32;
      }
    }

    switch(item->id){
    case ITEM_fire:
      if(item->active==ITEM_ST_UPDATE){
	item->active=ITEM_ST_SHOW;
	keys.fire.value=keyval;
      }
      break;
    case ITEM_turnleft:
      if(item->active==ITEM_ST_UPDATE){
	item->active=ITEM_ST_SHOW;
	keys.turnleft.value=keyval;
      }
      break;
    case ITEM_turnright:
      if(item->active==ITEM_ST_UPDATE){
	item->active=ITEM_ST_SHOW;
	keys.turnright.value=keyval;
      }
      break;
    case ITEM_accel:
      if(item->active==ITEM_ST_UPDATE){
	item->active=ITEM_ST_SHOW;
	keys.accel.value=keyval;
      }
      break;
    case ITEM_automode:
      if(item->active==ITEM_ST_UPDATE){
	item->active=ITEM_ST_SHOW;
	keys.automode.value=keyval;
      }
      break;
    case ITEM_manualmode:
      if(item->active==ITEM_ST_UPDATE){
	item->active=ITEM_ST_SHOW;
	keys.manualmode.value=keyval;
      }
      break;
    case ITEM_map:
      if(item->active==ITEM_ST_UPDATE){
	item->active=ITEM_ST_SHOW;
	keys.map.value=keyval;
      }
      break;
    case ITEM_order:
      if(item->active==ITEM_ST_UPDATE){
	item->active=ITEM_ST_SHOW;
	keys.order.value=keyval;
      }
      break;
    default:
      break;
    }
    break;
  default:
    fprintf(stderr,"Funct01()type: %d not defined\n",item->type);
    exit(-1);
    break;
    
  }
}
