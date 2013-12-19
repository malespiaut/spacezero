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
		version 0.86 December 2013
**************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "menu.h"
#include "shell.h" 
#include "sound.h" 
#include "locales.h" 
#include "save.h" 

#define MAXARGLEN 32

extern char version[64];
struct Parametres param;
struct Keys keys;

struct Validargum validarg[]={{"h",ARG_h},{"g",ARG_g},{"n",ARG_n},
			      {"p",ARG_p},{"t",ARG_t},{"l",ARG_l},
			      {"s",ARG_s},{"c",ARG_c},{"ip",ARG_ip},
			      {"port",ARG_port},{"name",ARG_name},
			      {"nosound",ARG_sound},{"nomusic",ARG_music},
			      {"soundvol",ARG_soundvol},{"musicvol",ARG_musicvol},
			      {"k",ARG_k},{"font",ARG_font},{"geom",ARG_geom},
			      {"cooperative",ARG_cooperative},
			      {"compcooperative",ARG_compcooperative},
			      {"queen",ARG_queen},
			      {"pirates",ARG_pirates},
			      {"nopirates",ARG_nopirates},
			      {"enemyknown",ARG_enemyknown},
			      {"noenemyknown",ARG_noenemyknown},
			      {"nomenu",ARG_nomenu},
			      {"fontlist",ARG_fontlist},			
			      {"lang",ARG_lang},
			      {"",ARG_0}};


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


int UpdateMenu(struct MenuHead *mhead,struct MenuHead *mactual){
  int ret=0;

  if(keys.down){
    /* printf("kdown\n"); */
    MenuDown(mactual);
    keys.down=FALSE;
  }

  if(keys.up){
    MenuUp(mactual);
    keys.up=FALSE;
  }

  if(keys.enter){
    ret=MenuEnter(mactual);
    keys.enter=FALSE;
    return(ret);
  }

  if(keys.esc){
    if(mactual!=mhead){
      MenuEsc(mactual);
    }
    keys.esc=FALSE;
    return(MENUESC);
  }

  if(keys.back){
    Keystrokes(DELETELAST,NULL,NULL);
    keys.back=FALSE;

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
	if(CheckArgs()){
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
	if(CheckArgs()){
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
	if(CheckArgs()){
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
	if(CheckArgs()){
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
	if(CheckArgs()){
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
	if(CheckArgs()){
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
	if(CheckArgs()){
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


/***** Parametres   *****/


void PrintArguments(char *title){

  printf("%s\n",title);

  printf("\tNUM GALAXIES: %d\n",param.ngalaxies);
  printf("\tNUM PLANETS: %d\n",param.nplanets);
  printf("\tNUM PLAYERS: %d\n",param.nplayers);
  if(param.kplanets){
    printf("\tPlanets are known by all the players.\n");
  }
  printf("\tplayer name: %s\n",param.playername);
  printf("\tknown planets: %d\n",param.kplanets);
  printf("\tsound: %d\n",param.sound);
  printf("\tmusic: %d\n",param.music);
  printf("\tsound vol: %d\n",param.soundvol);
  printf("\tmusic vol: %d\n",param.musicvol);
  printf("\tcooperative mode: %d\n",param.cooperative);
  printf("\tcomputer cooperative mode: %d\n",param.compcooperative);
  printf("\tQueen mode: %d\n",param.queen);
  printf("\tpirates: %d\n",param.pirates);
  printf("\tenemy known: %d\n",param.enemyknown);

  printf("\tUniverse size: %d\n",param.ul);
  printf("\tSERVER: %d\n",param.server);
  printf("\tCLIENT: %d\n",param.client);
  printf("\tIP: %s\n",param.IP);
  printf("\tPORT: %d\n",param.port);
  printf("\tfont type: %s\n",param.font);
  printf("\twindow geometry: %s\n",param.geom);
  printf("\tlanguage: %s\n",param.lang);
}



/**** parameters *****/


struct Parametres param;

int Arguments(int argc,char *argv[],char *optfile){
  /*
    version 01
    funcion Arguments(). 
    Check the options file.
    Check the number of command line arguments.
  */
  

  int i;
  char arg[MAXARGLEN]="";

  int narg=0;
  FILE *fp;
  int fsw=0;
#if DEBUG
  int debug=0;
#endif

  /* default values */
  SetDefaultParamValues();
  
  /*******  options file values ******/
#if DEBUG
  if(debug)printf("reading options file\n");
#endif
  if((fp=fopen(optfile,"rt"))==NULL){

    if((fp=fopen(optfile,"wt"))==NULL){
      fprintf(stdout,"I can't create the file: %s\n", optfile);
      exit(-1);
    }
    /* file doesn't exists */
    /* default options */   /* check also SetDefaultOptions() in graphics.c */
    fclose(fp);
    SaveParamOptions(optfile,&param);

    if((fp=fopen(optfile,"rt"))==NULL){
      fprintf(stdout,"I can't open the file: %s", optfile);
      exit(-1);
    }
    fclose(fp);
  }

  if(LoadParamOptions(optfile,&param)){
    /* some error in options file. Setting default options */
    fprintf(stderr,
	    "ERROR in options file: incorrect version. Overwriting with default options.\n");
    SaveParamOptions(optfile,&param);
    if(LoadParamOptions(optfile,&param)){
      fprintf(stderr,
	      "ERROR in options file: unknown error. Exiting...\n");   
      exit(-1);
    }
  }

  /****** checking options *******/

  /* universe known */
  if(param.kplanets!=0 && param.kplanets!=1){
    fsw=2;
#if DEBUG
    if(debug)fprintf(stderr,"\tuk: ERROR\n"); 
#endif
    
  }

  /* sound */
  
  if(param.music<0 && param.music>1){
    fsw=3;
    param.music=0;
#if DEBUG
    if(debug)fprintf(stderr,"\tmusic OFF:ERROR\n");
#endif
  }

  if(param.sound<0 && param.sound>1){
    fsw=4;
    param.sound=0;
    param.music=0;
#if DEBUG
    if(debug)fprintf(stderr,"\tsound OFF:ERROR\n"); 
#endif
  }

  if(param.musicvol<0 || param.musicvol>100){
    fsw=3;
    param.musicvol=100;
#if DEBUG
    if(debug)fprintf(stderr,"\tmusic OFF:ERROR\n");
#endif
  }

  if(param.soundvol<0 || param.soundvol>100){
    fsw=4;
    param.soundvol=100;
    param.musicvol=100;
#if DEBUG
    if(debug)fprintf(stderr,"\tsound OFF:ERROR\n"); 
#endif
  }


  if(param.nplanets<MINNUMPLANETS || param.nplanets>MAXNUMPLANETS){
    fsw=5;
#if DEBUG
    if(debug)fprintf(stderr,"\tnplanets:ERROR\n");
#endif
  }

  if(param.nplayers<MINNUMPLAYERS || param.nplayers>MAXNUMPLAYERS){
    fsw=6;
#if DEBUG
    if(debug)fprintf(stderr,"\tnplayers:ERROR\n");
#endif
  }
  
  if(param.ul<MINULX || param.ul>MAXULX){
    fsw=7;
#if DEBUG
    if(debug)fprintf(stderr,"\tuniverse size:ERROR\n"); 
#endif
  }

  if(param.nplayers>param.nplanets)fsw=8;

  if(param.cooperative!=0 && param.cooperative!=1){
    fsw=9;
#if DEBUG
    if(debug)fprintf(stderr,"\tcooperative:ERROR\n");
#endif
  }

  if(param.compcooperative!=0 && param.compcooperative!=1){
    fsw=10;
#if DEBUG
    if(debug)fprintf(stderr,"\tcomputer cooperative:ERROR\n"); 
#endif
  }

  if(param.queen!=0 && param.queen!=1){
#if DEBUG
    if(debug)fprintf(stderr,"\tQueen mode:ERROR\n");
#endif
    fsw=11;
  }


  if(fsw){
    fprintf(stderr,"(%d)Warning: Incorrect values in options file %s\n",fsw,optfile);
    fprintf(stderr,"\t Ignoring file.\n");
    fprintf(stderr,"\t Setting default values.\n");

    if(fsw==3){
      param.music=0;
      param.musicvol=100;
    }
    if(fsw==4){
      param.sound=0;
      param.music=0;
      param.soundvol=100;
      param.musicvol=100;
    }
  }
  /*******  --options file values ******/


  /*************** command line values ******************/
  fprintf(stderr,"command line:..\n");
  for(i=1;i<argc;i++){
    if(*argv[i]=='-'){
      snprintf(arg,MAXARGLEN,"%s",&argv[i][1]);
      strncpy(&arg[MAXARGLEN-1],"\0",1); 
      narg=SearchArg(arg);
      if(narg<0){ 
 	printf("\ninvalid option -%s\n",arg); 
 	printf("\ntry 'spacezero -h' for help\n"); 
 	printf("\nExiting ...\n"); 
 	exit(-1); 
      } 

      switch(narg){
      case ARG_h:/*'h': show help */
	return(ARG_h);
	break;
      case ARG_g:/*'g': number of galaxies */
	if(i+1<argc){
	  param.ngalaxies=atoi(argv[i+1]);
	  i++;
	}
	else{
	  return(ARG_g);
	}
	break;
      case ARG_n:/*'n': number of planets */
	if(i+1<argc){
	  param.nplanets=atoi(argv[i+1]);
	  i++;
	}
	else{
	  return(ARG_n);
	}
	break;
      case ARG_p:/*'p': number of players */
	if(i+1<argc){
	  param.nplayers=atoi(argv[i+1]);
	  i++;
	}
	else{
	  return(ARG_p);
	}
	break;
      case ARG_t:/* 't': team, not used */
	break;
      case ARG_l:/*'l': size of universe */
	if(i+1<argc){
	  param.ul=atoi(argv[i+1]);
	  i++;
	}
	else{
	  return(ARG_l);
	}
	break;
      case ARG_s:/*'s': server */
	param.server=TRUE;
	param.client=FALSE;
	break;
      case ARG_c:/*'c': client */
	param.server=FALSE;
	param.client=TRUE;
	break;
      case ARG_ip:/*'ip': ip of the server */
	if(i+1<argc){
	  snprintf(param.IP,MAXTEXTLEN,"%s",argv[i+1]);
	  strncpy(&param.IP[MAXTEXTLEN-1],"\0",1);
	  i++;
	}
	else{
	  return(ARG_ip);
	}
	break;
      case ARG_port:/*'port': port used for communication */
	if(i+1<argc){
	  param.port=atoi(argv[i+1]);
	  param.port2=atoi(argv[i+1])+1;
	  i++;
	}
	else{
	  return(ARG_port);
	}
	break;
      case ARG_name: /* name:  player name */
 	if(i+1<argc){
	  snprintf(param.playername,MAXTEXTLEN,"%s",argv[i+1]);
	  strncpy(&param.playername[MAXTEXTLEN-1],"\0",1);
	  i++;
	}
	else{
	  return(ARG_name);
	}
	break;
      case ARG_sound: /* nosound */
	param.sound=0;
	param.music=0;
	break;

      case ARG_music: /* nomusic */
	param.music=0;
	break;

      case ARG_soundvol: /* sound volume */
	if(i+1<argc){
	  param.soundvol=atoi(argv[i+1]);
	  i++;
	}
	else{
	  return(ARG_soundvol);
	}
	break;

      case ARG_musicvol: /* music volume */
	if(i+1<argc){
	  param.musicvol=atoi(argv[i+1]);
	  i++;
	}
	else{
	  return(ARG_musicvol);
	}
	break;

      case ARG_k: /*k: known planets */
	param.kplanets=1;
	break;
      case ARG_font: /* font type  */
 	if(i+1<argc){
	  snprintf(param.font,MAXTEXTLEN,"%s",argv[i+1]);
	  strncpy(&param.font[MAXTEXTLEN-1],"\0",1);
	  i++;
	}
	else{
	  return(ARG_font);
	}
	break;
      case ARG_geom: /* window geometry */
 	if(i+1<argc){
	  snprintf(param.geom,MAXTEXTLEN,"%s",argv[i+1]);
	  strncpy(&param.geom[MAXTEXTLEN-1],"\0",1);
	  /* HERE check param values. */
	  i++;
	}
	else{
	  return(ARG_geom);
	}
	break;

      case ARG_cooperative: /* cooperative mode */
	param.cooperative=1;
	break;
      case ARG_compcooperative: /* computer cooperative mode */
	param.compcooperative=1;
	break;
      case ARG_queen: /* queen mode on */
	param.queen=1;
	break;
      case ARG_pirates:
	param.pirates=TRUE;
	break;
      case ARG_nopirates:
	param.pirates=FALSE;
	break;
      case ARG_enemyknown:
	param.enemyknown=TRUE;
	break;
      case ARG_noenemyknown:
	param.enemyknown=FALSE;
	break;
      case ARG_nomenu:
	param.menu=FALSE;
	break;
      case ARG_fontlist:
	param.fontlist=TRUE;
	break;
      case ARG_lang:
	snprintf(param.lang,MAXTEXTLEN,"%s",argv[i+1]);
	strncpy(&param.lang[MAXTEXTLEN-1],"\0",1);
	i++;
	break;
      default:
	printf("\ninvalid option -%s\n",arg);
	printf("try 'spacezero -h' for help\n");
	printf("Exiting ...\n");
	exit(-1);
	break;
      }
    }
    else{
      printf("invalid argument %d %s\n",i,argv[i]);
      printf("\ntry 'spacezero -h' for help\n"); 
      printf("\nExiting ...\n"); 
      exit(-1);
    }
  }
  fprintf(stderr,"..command line:\n");
  if(param.nplayers==-1){
    if(param.server==FALSE && param.client==FALSE){
      param.nplayers=NUMPLAYERS;
    }
    if(param.server==TRUE){
      param.nplayers=NUMPLAYERS;
    }
    if(param.client==TRUE){
      param.nplayers=1;
    }
  }

  return(0);
}  /* --Arguments()  */



int CheckArgs(void){
  /* 
     returns:
     1 if there are a non valid number
     0 if the the values are in range
   */
  int sw;

  if(param.nplanets<MINNUMPLANETS){
    printf("Number of planets must be > %d\n",MINNUMPLANETS);
    return(1);
  }
  if(param.nplanets>MAXNUMPLANETS){
    printf("Number of planets must be < %d\n",MAXNUMPLANETS);
    return(1);
  }
  if(param.ngalaxies<MINNUMGALAXIES){
    printf("Number of galaxies must be >= %d\n",MINNUMGALAXIES);
    return(1);
  }
  if(param.ngalaxies>MAXNUMGALAXIES){
    printf("Number of galaxies must be <= %d\n",MAXNUMGALAXIES);
    return(1);
  }
  if(param.nplayers<MINNUMPLAYERS){
    printf("Number of players must be > %d\n",MINNUMPLAYERS);
    return(1);
  }
  if(param.nplanets<param.nplayers){
    printf("Number of planets must be >= number of players\n");
    return(1);
  }

  if(param.nplayers>MAXNUMPLAYERS){
    printf("number of players must be less than %d\n",MAXNUMPLAYERS);
    return(1);
  }
  if(param.ul<MINULX){
    printf("Size of Universe must be > %d\n",MINULX);
    return(1);
  }
  if(param.ul>MAXULX){
    printf("Size of Universe must be < %d\n",MAXULX);
    return(1);
  }

  if(param.port<49152 || param.port>65535){
    printf("Invalid port: %d. Must be between (49152,65535).\n",param.port);
    return(1);
  }

  if(param.soundvol<0){
    printf("sound volume must be >= 0\n");
    return(1);
  }
  if(param.soundvol>100){
    printf("sound volume must be <= 100\n");
    return(1);
  }

  if(param.musicvol<0){
    printf("music volume must be >= 0\n");
    return(1);
  }
  if(param.musicvol>100){
    printf("music volume must be <= 100\n");
    return(1);
  }

  /* language */
  sw=1;
  if(sw&&strncmp(param.lang,"en",10)==0){
    sw=0;
  }
  if(sw&&strncmp(param.lang,"es",10)==0){
    sw=0;
  }
  if(sw&&strncmp(param.lang,"it",10)==0){
    sw=0;
  }
  if(sw){
    printf("language not suported: %s\n",param.lang);
    printf("supported languages:\n\t(en)glish\n\t(es)pañol\n");
    return(1);
  }
  /* --language */

  /* TODO */
  /***** ip *****/ 

  /***** geom *****/ 

 return(0);
}



int SearchArg(char *target){

  int i=0;

  while(validarg[i].id>0){
    if(strncmp(target,validarg[i].cad,24)==0){
#if TEST
      printf("%s\n",validarg[i].cad); 
#endif
      return(validarg[i].id);
    }
    i++;
  }
  return(-1);
}




void SetDefaultParamValues(void){
  printf("Setting default param values\n");  
  param.ngalaxies=NUMGALAXIES;
  param.nplanets=NUMPLANETS;
  param.nplayers=2;
  param.nteams=2;
  param.ul=ULX;
  param.kplanets=0;
  param.sound=1;
  param.music=1;
  param.soundvol=100;
  param.musicvol=100;

  param.cooperative=FALSE;
  param.compcooperative=FALSE;
  param.queen=FALSE;

  param.pirates=TRUE;
  param.enemyknown=FALSE;
  param.menu=TRUE;

  param.server=FALSE;
  param.client=FALSE;

  snprintf(param.IP,MAXTEXTLEN,"%s",DEFAULT_IP);
  strncpy(&param.IP[MAXTEXTLEN-1],"\0",1);

  param.port=DEFAULT_PORT;
  param.port2=DEFAULT_PORT+1;
  strcpy(param.playername,PLAYERNAME); /* set default */
  
  strcpy(param.font,"6x13");
  strcpy(param.geom,"1024x550");
  strcpy(param.lang,"en");
}


int GetGeom(char *geom,int *w,int *h){
  /*
    Gets the window geometry parametres from the cad geom.
    returns:
    In the pointers w and h the value of the window geometry from the cad geom.
    0 if there are no error
    >0 if therea are some error in the structure of the cad geom.
   */

  char str[24];
  int sw;
  int len;
  char *pointer;
  char *endptr=NULL;

  *w=DEFAULTWIDTH;
  *h=DEFAULTHEIGHT;
  len=strlen(geom);

  if(len==0){
    /* no option given */
    return(1);
  }

  if(len>9){
    fprintf(stderr,"WARNING: invalid option geom. Too high. Settting geometry to default: %dx%d.\n",DEFAULTWIDTH,DEFAULTHEIGHT);
    return(2);
  }
  sw=0;
  pointer=strchr(geom,'x');

  if(pointer==NULL){
    pointer=strchr(geom,':');
  }

  if(pointer){
    len=strlen(geom)-strlen(pointer);
    strncpy(str,geom,len);
    *w=(int)strtol(str,&endptr,10);
    if(*w==0 && str==endptr){
      sw++;
    }
    *h=strtol(pointer+1,&endptr,10);
    if(*h==0 && pointer+1==endptr){
      sw++;
    }
  }
  else{
    sw++;
  }

  if(sw){
    fprintf(stderr,"WARNING: -geom option bad formed. Using default values.\n");
    *w=DEFAULTWIDTH;
    *h=DEFAULTHEIGHT;
  }

  if(*w<640 || *w>1680 || *h<312 || *h>1050){
    if(*w<640)*w=640;
    if(*w>1680)*w=1680;
    if(*h<312)*h=312;
    if(*h>1050)*h=1050;
    fprintf(stderr,"WARNING: geom values reach their limit. Setting geometry to limit values:(640,1680)x(312,1050).\n");
      return(1);
  }
  return(0);  
}



/***** keys ******/

struct Keys* GetKeys(void){
  return &keys;
}


void SetDefaultUserKeys(void){
  keys.fire.value=32;
  keys.turnleft.value=65361;
  keys.turnright.value=65363;
  keys.accel.value=65362;
  keys.automode.value=65364;
  keys.manualmode.value=65362;
  keys.map.value=109;
  keys.order.value=111;
}


int LoadUserKeys(char *keyfile){
  FILE *fp;
  char cad[MAXTEXTLEN];
  
  if((fp=fopen(keyfile,"rt"))==NULL){
    printf("file does not exist\n");
    
    /* if doesn't exist, create with default values */
    SaveUserKeys(keyfile);
    
  }
  else{
    fclose(fp);
  }
  
  /* Read keys  */
  
  if((fp=fopen(keyfile,"rt"))==NULL){
    fprintf(stdout,"I can't open the file: %s", keyfile);
    exit(-1);
  }
  
  if(fscanf(fp,"%128s",cad)!=1){ /* HERE check version */
    perror("fscanf");
    exit(-1);
  }
  
  if(strcmp(cad,MINOROPTIONSVERSION)>=0){
    printf("Version:  game:(%s)  keymap file:(%s) >= %s  ... OK\n",
	   version,cad,MINOROPTIONSVERSION);
  }
  else if(strcmp(cad,MINOROPTIONSVERSION)<0){
    fprintf(stderr,"Error: incompatible versions.\n");
    printf("Version:  game:(%s)  keymap file:(%s) < %s\n",
	   version,cad,MINOROPTIONSVERSION);
    fclose(fp);
    return(1);
  }
  
  if(fscanf(fp,"%128s",cad)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%ud",&keys.fire.value)!=1){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%128s",cad)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%ud",&keys.turnleft.value)!=1){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%128s",cad)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%ud",&keys.turnright.value)!=1){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%128s",cad)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%ud",&keys.accel.value)!=1){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%128s",cad)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%ud",&keys.automode.value)!=1){
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%128s",cad)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%ud",&keys.manualmode.value)!=1){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%128s",cad)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%ud",&keys.map.value)!=1){
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%128s",cad)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%ud",&keys.order.value)!=1){
    perror("fscanf");
    exit(-1);
  }
  
  fclose(fp);
  return(0);
}

void SaveUserKeys(char *file){
  
  FILE *fp;
  
  if((fp=fopen(file,"wt"))==NULL){
    fprintf(stdout,"Can't open the file: %s",file);
    exit(-1);
  }
  printf("saving keymap to file: %s\n",file);
  fprintf(fp,"%s\n",version);
  fprintf(fp,"fire %ud\n",
	  keys.fire.value);
  fprintf(fp,"turnleft %ud\n",
	  keys.turnleft.value);
  fprintf(fp,"turnright %ud\n",
	  keys.turnright.value);
  fprintf(fp,"accel %ud\n",
	  keys.accel.value);
  fprintf(fp,"automode %ud\n",
	  keys.automode.value);
  fprintf(fp,"manualmode %ud\n",
	  keys.manualmode.value);
  fprintf(fp,"map %ud\n",
	  keys.map.value);
  fprintf(fp,"order %ud\n",
	  keys.order.value);
  
  fclose(fp);
} 


void SetDefaultKeyValues(int action){
  int i;
  
  keys.load=keys.save=FALSE;
  keys.up=keys.down=keys.right=keys.left=keys.back=FALSE;
  keys.centermap=keys.trace=keys.tab=keys.enter=FALSE;
  keys.s=keys.n=keys.l=FALSE;
  keys.e=keys.y=keys.u=FALSE;
  keys.f1=keys.f2=keys.f3=keys.f4=keys.f7=keys.f8=keys.f9=keys.f10=FALSE;
  keys.p=FALSE;
  keys.d=FALSE;
  keys.home=keys.Pagedown=keys.Pageup=keys.may=keys.ctrl=FALSE;
  for(i=0;i<10;i++)keys.number[i]=FALSE;
  keys.plus=keys.minus=FALSE;
  
  keys.fire.state=FALSE;
  keys.turnleft.state=FALSE;
  keys.turnright.state=FALSE;
  keys.accel.state=FALSE;
  keys.automode.state=FALSE;
  keys.manualmode.state=FALSE;
  keys.order.state=FALSE;
  keys.map.state=FALSE;
  
  /* don't reset this values when load a game */
  switch(action){
  case 1:
    keys.map.state=FALSE;
    keys.f5=keys.f6=FALSE;
    break;
  default:
    break;
  }
}

