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


struct MenuList *Add2MenuList(struct MenuList *list0,struct MenuItem *item0){
  /* 
     add the cad to the list.
  */
  struct MenuList *list;
  struct MenuList *lh;

  /* Add text at the end of the list */
  lh=list0;
  while(lh->next!=NULL){
    lh=lh->next;
  }

  list=malloc(sizeof(struct MenuList));
  if(list==NULL){
    fprintf(stderr,"ERROR in malloc Add2MenuList()\n");
    exit(-1);
  }
  //  g_memused+=sizeof(struct MenuList);

  strncpy(list->item.text,item0->text,MAXMENULEN);
  list->item.type=item0->type;
  list->next=NULL; 
  lh->next=list;
  return (0);
}

void PrintMenuList(struct MenuList *list0){


  struct MenuList *list;

  printf("printfmenulist\n");
  if(list0==NULL)return;

  list=list0->next;
  while (list != NULL){
    printf("%s\n",list->item.text);
    list=list->next;
  }

  printf("printmenulist end\n");
}

struct MenuItem *GetMenuItem(struct MenuList *list0,int n){

  return (NULL);
}
