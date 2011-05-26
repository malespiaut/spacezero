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

#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "general.h"
#include "objects.h"

extern int g_memused;

/* 
 *   data structures and functions
 *
 */



/* 
   Unordered list 
*/

struct IntList* Add2IntList(struct IntList *list,int id){
  /* 
     version 01
     add the integer id to the end of the list
     if is not already added.
     returns:
     a pointer to the beginning of the list. 
   */

  struct IntList *kps;

  if(list==NULL){ /* first item */
    list=malloc(sizeof(struct IntList));
    g_memused+=sizeof(struct IntList);
    if(list==NULL){ 
      fprintf(stderr,"ERROR in malloc (Add2IntList)\n"); 
      exit(-1); 
    } 
    list->id=id;
    list->next=NULL;
    return(list);
  }

  /* look if it its already added */
  kps=list;

  if(kps->id==id)
    return(list); /* first item is known */

  while(kps->next!=NULL){
    if(kps->next->id==id)
      return(list); /* item was known */
    kps=kps->next;
  }
  /* 
     item is not in the list 
     adding to the end
  */

  kps->next=malloc(sizeof(struct IntList));
  if(kps->next==NULL){ 
    fprintf(stderr,"ERROR in malloc (Add2IntList)\n"); 
    exit(-1); 
  } 
  g_memused+=sizeof(struct IntList);
  kps->next->id=id;
  kps->next->next=NULL;
  return(list);
}


int IsInIntList(struct IntList *list,int id){
  /* 
     Secuential search (must be improve)
     return:
     TRUE if id is in the list
     FALSE if not.
  */

  /* look if it its already added */

  while(list!=NULL){
    if(list->id==id){
      return(TRUE); /* item is known */
    }
    list=list->next;
  }

  /* item is not in the list */
  return(FALSE);
}

int CountIntList(struct IntList *list){
  /* return the number of item of the list */

  int n=0;

  while(list!=NULL){
    n++;
    list=list->next;
  }
  return(n);
}

int DelFirstIntList(struct IntList *list){
  /* Delete the first item of the list */
  struct IntList *item0;

  if(list!=NULL){
    item0=list;
    list=list->next;
    free(item0);
    g_memused-=sizeof(struct IntList);
    item0=NULL;
    return(1);
  }
  return(0);
}


int DelIntList(struct IntList *list){
  /* 
     version 01
     Delete all the list 
     returns the number of items deleted
  */

  struct IntList *item0;
  int n=0;

  if(list==NULL)return(0);

  while(list->next!=NULL){
    item0=list->next;
    list->next=list->next->next;
    free(item0);
    g_memused-=sizeof(struct IntList);
    item0=NULL;
    n++;
  }
  
  free(list);
  g_memused-=sizeof(struct IntList);
  list=NULL;
  n++;
  return (n);
}


int FPrintIntList(FILE *fp,struct IntList *list){
  /* return the number of item of the list */

  int n=0;

  while(list!=NULL){
    fprintf(fp,"%d ",list->id);
    n++;
    list=list->next;
  }
  return(n);
}


/* 
   Ordered list 
*/

int IsInIntOList(struct IntList *list,int id){
  /* return:
     TRUE if id is in the list
     FALSE if not.
  */

  while(list!=NULL){
    if(list->id==id)
      return(TRUE); /* item is known */
    if(list->id>id)
      return(FALSE);
    list=list->next;
  }

  /* item is not in the list */
  return(FALSE);
}


/* 
   Indexed list 
*/

int Add2IntIList(struct HeadIntIList *head,int id){
  /* add the integer id to the list head
     if is not already added.
     returns:
     0 if item is added
     1 if is not added, the item exists
   */
  struct IntList *item;
  struct IntList *last=NULL;
  int i;

  /* is the first */
  if(head->list==NULL){
    head->list=malloc(sizeof(struct IntList));
    g_memused+=sizeof(struct IntList);
    if(head->list==NULL){ 
      fprintf(stderr,"ERROR in malloc (Add2IntIList)\n"); 
      exit(-1); 
    } 
    head->n++;
    head->n0++;
    head->list->id=id;
    head->list->next=NULL;
    for(i=0;i<NINDEXILIST;i++){
      head->index[i]=head->list;
    }
    /*    printf("added (0): %d\n",id); */
    return(0);
  }

  if(head->n0 > NINDEXCALC){
    int i=0;
    int n=0;
    int m;

    item=head->list;
    head->n0=0;
    m=head->n/NINDEXILIST;
    head->index[i++]=head->list;
    while(item!=NULL){
      n++;
      if(n>m){
	head->index[i++]=item;
	n=0;
      }
      item=item->next;
    }
  }

  item=head->list;
  for(i=0;i<NINDEXILIST-1;i++){
    if( head->index[i]->id > id)break;
    item=head->index[i];
  }

  /* look if it its already added */
  while(item!=NULL){

    if(item->id==id){
      return(1); /* item is in the list */
    }
    if(item->id>id){ /*item is not in the list, adding in the middle */
      struct IntList *itemn;
      itemn=malloc(sizeof(struct IntList));
      g_memused+=sizeof(struct IntList);
      if(itemn==NULL){ 
	fprintf(stderr,"ERROR in malloc (Add2IntIList)\n"); 
	exit(-1); 
      } 
      head->n++;
      head->n0++;

      if(last==NULL){ /* first item */
	itemn->id=id;
	itemn->next=head->list;
	head->list=itemn;
	head->index[0]=head->list;
      
	/*	printf("added (1): %d\n",id); */
      }
      else{
	itemn->id=id;
	last->next=itemn;
	itemn->next=item;
	/*	printf("added (2): %d\n",id); */
      }
      return(0);
    }
    last=item;
    item=item->next;
  }

  /* item is not in the list, adding to the end */
  head->n++;
  head->n0++;
  
  item=malloc(sizeof(struct IntList));
  g_memused+=sizeof(struct IntList);
  if(item==NULL){ 
    fprintf(stderr,"ERROR in malloc (Add2IntIList)\n"); 
    exit(-1); 
  } 

  last->next=item;
  item->next=NULL;
  item->id=id;
  
  return(0);
}

int IsInIntIList(struct HeadIntIList *head,int id){
  /* return:
     TRUE if id is in the list
     FALSE if not.
  */

  /* look if it its already added */

  int i;
  struct IntList *list;


  list=head->index[0];

  if(list==NULL){
    /*    printf("no en lista id: %d\n",id); */
    return(FALSE);

  }
  for(i=0;i<NINDEXILIST-1;i++){
    if( head->index[i]->id > id)break;
    list=head->index[i];
  }

  while(list!=NULL){
    if(list->id==id)
      return(TRUE); /* item is known */
    if(list->id>id)
      return(FALSE);
    list=list->next;
  }

  /* item is not in the list */
  return(FALSE);
}

int DelIntIList(struct HeadIntIList *head){
  /* Delete all the list */

  int n=0;
  int i;

  DelIntList(head->list);

  head->n=head->n0=0;

  head->list=NULL;
  for(i=0;i<NINDEXILIST;i++){
    head->index[i]=NULL;
  }
  return (n);
}

int PrintIntIList(struct HeadIntIList head){
  /* print the list */
  int n=0;
  int i;
  struct IntList *list;
  
  list=head.list;

  while(list!=NULL){
    printf("%d ",list->id);
    n++;
    list=list->next;
  }
  printf("\n");
  for(i=0;i<NINDEXILIST;i++){
    if(head.index[i]!=NULL)
      printf("%p %d\n",head.index[i],head.index[i]->id);
    else
      printf("%p\n",head.index[i]);
  }
  return(n);
}



/* 
   Tree 
*/

struct IntTree *Add2IntTree(struct IntTree *head,int id){
  /* add the integer id to the tree head
     if is not already added.
     returns:
     a pointer to the head of the list.
   */

  if(!head){
    head=malloc(sizeof(struct IntTree));
    g_memused+=sizeof(struct IntTree);
    if(head==NULL){ 
      fprintf(stderr,"ERROR in malloc (Add2IntTree)\n"); 
      exit(-1); 
    } 
    head->id=id;
    head->l=NULL;
    head->r=NULL;
    printf("added: %d\n",head->id);
    return(head);
  }

  if(id<head->id){
    head->l=Add2IntTree(head->l,id);
  }
  else{
    if(id>head->id){
      head->r=Add2IntTree(head->r,id);
    }
  }
  return(head); 
}

void PrintTree (struct IntTree *head){
  static int level=0;
  level++;
  if(head!=NULL){
    PrintTree(head->l);
    printf("l %d:  %d\n",level,head->id);
    PrintTree(head->r);
  }
  level--;
}

int SizeIntTree (struct IntTree *head,int reset){
  /*
    Return:
    the size of the tree.
   */


  static int level=0;
  static int max=0;
  if(reset){
    max=0;
    return(0);
  }
  if(level>max)max=level;
  level++;

  if(head!=NULL){
    SizeIntTree(head->l,0);
    SizeIntTree(head->r,0);
  }
  level--;
  return(max);
}


int IsInIntTree(struct IntTree *head,int id){
  /*
    returns:
    1 if the element id is in the tree
    0 if not.
   */
  if(head!=NULL){
    if(id==head->id){
      return(1);
    }
    if(id < head->id){
      return(IsInIntTree(head->l,id));
    }
    else{
      return(IsInIntTree(head->r,id));
    }
  }
  return(0);
}

void DelIntTree(struct IntTree *head){
  /*
    Delete the tree freeing the memory.
  */

  if(head!=NULL){
    DelIntTree(head->l);
    DelIntTree(head->r);
    head->l=NULL;
    head->r=NULL;
    free(head);
    g_memused-=sizeof(struct IntTree);
    head=NULL;
  }
}


int CountIntTree (struct IntTree *head){
  /*
    Returns:
    the number of elemnets of the tree.
  */
 int cont=0;

  if(head!=NULL){
    cont+=CountIntTree(head->l);
    cont++;
    cont+=CountIntTree(head->r);
  }
  return(cont);
}


/* objects */



