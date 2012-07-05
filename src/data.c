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

#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "general.h"
#include "objects.h"
#include "functions.h"


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
    MemUsed(MADD,+sizeof(struct IntList));
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
  MemUsed(MADD,+sizeof(struct IntList));
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
    MemUsed(MADD,-sizeof(struct IntList));
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
    MemUsed(MADD,-sizeof(struct IntList));
    item0=NULL;
    n++;
  }
  
  free(list);
  MemUsed(MADD,-sizeof(struct IntList));
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
    MemUsed(MADD,+sizeof(struct IntList));
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
      MemUsed(MADD,+sizeof(struct IntList));
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
  MemUsed(MADD,+sizeof(struct IntList));
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


/* char list with scroll  */

int Add2CharList(struct CharListHead *hlist,char *cad,int mode){
  /* 
     version 00
     add the char cad to the end of the list
     returns:
     0 if success, item added.
     1 success,list full, item added and first item deleted.
     2 if there are some error.
     3 if is already added.
   */

  struct CharList *list;
  int swfull=0;
  int m=0;
  int n; 


  if(hlist==NULL){
    fprintf(stderr,"ERROR in Add2CharList(): NULL\n");
    exit(-1);
  }
  if(cad==NULL){
    fprintf(stderr,"ERROR in Add2CharList(): cad NULL\n");
    return(2);
  }
  n=strlen(cad);
  if(n>32)n=32; /* max of log messages */

  if(mode==1){ /* check if is recently added => dont add*/
    m=0;
    list=hlist->next;
    while(list!=NULL){
      if(strncmp(cad,list->cad,32)==0){
	if(m > hlist->n-20){
	  return(2);
	}
      }
      m++;
      list=list->next;
    }
  }

  list=hlist->next;

  if(list==NULL){ /* first item */
    list=malloc(sizeof(struct CharList));
    MemUsed(MADD,+sizeof(struct CharList));
    if(list==NULL){ 
      fprintf(stderr,"ERROR in malloc (Add2Charlist)\n"); 
      exit(-1); 
    } 

    list->cad=malloc((n+1)*sizeof(char));
    MemUsed(MADD,+sizeof((n+1)*sizeof(char)));
    if(list->cad==NULL){ 
      fprintf(stderr,"ERROR in malloc (Add2Charlist)\n"); 
      exit(-1); 
    } 

    snprintf(list->cad,n+1,"%s",cad);
    //    strncpy(list->cad,cad,n+1);
    list->next=NULL;
    hlist->next=list;
    hlist->n++;
    return(0);
  }

  /* checking max number of elements */
  /* if reach its limit delete the first item */

  if(hlist->n==hlist->max){
    struct CharList *freeitem;
    int n;
    freeitem=hlist->next;
    if(freeitem!=NULL){
      hlist->next=hlist->next->next;
      n=strlen(freeitem->cad);
      free(freeitem->cad);
      free(freeitem);
      MemUsed(MADD,-(sizeof((n+1)*sizeof(char))+sizeof(struct CharList)));
      hlist->n--;
      swfull++;
    }
    else{
      fprintf(stderr,"ERROR in List (Add2Charlist) not added.\n"); 
      return(2);
    }
  }


  /* adding to the end of the list if there are room */

  while(list->next!=NULL)list=list->next;

    list->next=malloc(sizeof(struct CharList));
    MemUsed(MADD,+sizeof(struct CharList));
    if(list->next==NULL){ 
      fprintf(stderr,"ERROR in malloc (Add2Charlist)\n"); 
      exit(-1); 
    } 

    list->next->cad=malloc((n+1)*sizeof(char));
    MemUsed(MADD,+sizeof((n+1)*sizeof(char)));
    if(list->next->cad==NULL){ 
      fprintf(stderr,"ERROR in malloc (Add2Charlist)\n"); 
      exit(-1); 
    } 
    snprintf(list->next->cad,n+1,"%s",cad);
    //    strncpy(list->next->cad,cad,n+1);
    list->next->next=NULL;
    hlist->n++;

    if(swfull)return(1);
    return(0);
}

int Add2CharListWindow(struct CharListHead *hlist,char *cad,int mode,struct Window *w){
  /* 
     version 00
     change the scroll bar if a new item is added to hlist.
     returns:
     0 if success
     1 if there are some error
     2 if is already added
   */

  int status;

  if(hlist==NULL){
    fprintf(stderr,"ERROR in Add2CharListWindow(): NULL\n");
    exit(-1);
  }
  if(cad==NULL){
    fprintf(stderr,"ERROR in Add2CharListwindow(): cad NULL\n");
    return(1);
  }

  if(w==NULL)return(1);

  status= Add2CharList(hlist,cad,mode);

  switch(status){
  case 0:
  case 1:
    if(w->scrollbar.n!=0){
      w->scrollbar.n++;
    }
    break;
  case 2:
    break;
  default:
    break;
  }

  return(status);
}



int PrintCharList(struct CharListHead *hlist){
  /* 
     version 00
     print the list
     returns:
     the number of item printed.
   */

  struct CharList *list;
  int n=0; 


  if(hlist==NULL){
    fprintf(stderr,"ERROR in Add2CharList(): NULL\n");
    exit(-1);
  }
  
  list=hlist->next;
  
  while(list!=NULL){
    printf("%d:  %s\n",n,list->cad);
    list=list->next;
    n++;
  }
  if(n!=hlist->n){
    printf("ERROR in list n: %d  hlist.n:%d\n",n,hlist->n);
  }
  return(n);
}


int DestroyCharList(struct CharListHead *hlist){
  /* 
     version 00
     delete all the list
     returns:
     the number of item deleted.
   */

  struct CharList *freeitem;
  int n=0; 
  int cont=0;

  if(hlist==NULL){
    fprintf(stderr,"ERROR in Add2CharList(): NULL\n");
    exit(-1);
  }
  
  while(hlist->next!=NULL){
    freeitem=hlist->next;
    hlist->next=hlist->next->next;
    n=strlen(freeitem->cad);
    free(freeitem->cad);
    free(freeitem);
    MemUsed(MADD,-(sizeof(n*sizeof(char))+sizeof(struct CharList)));
    hlist->n--;
    cont++;
  }
  return(cont);
}



/******** 
   Tree 
********/

struct IntTree *Add2IntTree(struct IntTree *head,int id){
  /* add the integer id to the tree head
     if is not already added.
     returns:
     a pointer to the head of the list.
   */

  if(!head){
    head=malloc(sizeof(struct IntTree));
    MemUsed(MADD,+sizeof(struct IntTree));
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
    MemUsed(MADD,-sizeof(struct IntTree));
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



