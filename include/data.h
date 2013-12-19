 /*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyright (C) 2006-2013  MRevenga
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

#ifndef _DATA_
#define _DATA_ 

#include <stdio.h>
#include "general.h"

#define NINDEXILIST 20  /* number of indexes */
#define NINDEXCALC 100  /* recalc indexes when this number of items are added */

struct IntTree{
  int id;
  struct IntTree *l;
  struct IntTree *r;
};

struct IntList{
  int id;                /* item value */
  struct IntList *next;  /* pointer to the next item */
};

struct HeadIntIList{
  int n;
  int n0;
  struct IntList *list;
  struct IntList *index[NINDEXILIST];
};

struct CharListHead{
  int n;                    /* number of elements */
  int max;                  /* max. number of elements */
  int first;                /* first item to draw (scroll) */
  struct CharList *next;    /* first element of the list */
};

struct CharList{
  char *cad;
  struct CharList *next;
};

struct Scrollbar{
  int active;
  int width;
  int pos;      /* 0 rigth , 1 left */
  int n;        /* position of item 0..100*/
};


struct Window{ /* basic window */
  int type;
  int active;
  int x,y;
  int width,height;
  struct  Scrollbar scrollbar;
  void *data;
};

struct HeadTextList{
  int n;
  struct TextList *next;
};

struct TextList{
  int color;
  char text[MAXTEXTLEN];  /* text */
  struct TextList *next;
};


struct IntTree *Add2IntTree(struct IntTree *head,int id);
int IsInIntTree(struct IntTree *head,int id);
void DelIntTree(struct IntTree *head);
int SizeIntTree (struct IntTree *head,int);
int CountIntTree (struct IntTree *head);
void PrintTree(struct IntTree *head);

struct IntList* Add2IntList(struct IntList *list,int id);
int IsInIntList(struct IntList *list,int id);
int CountIntList(struct IntList *list);
int DelFirstIntList(struct IntList *list);
int DelIntList(struct IntList *list);
int FPrintIntList(FILE *fp,struct IntList *list);

int Add2IntOList(struct IntList *list,int id);
int IsInIntOList(struct IntList *list,int id);

int Add2IntIList(struct HeadIntIList *headlist,int id);
int IsInIntIList(struct HeadIntIList *headlist,int id);
int DelIntIList(struct HeadIntIList *head);
int PrintIntIList(struct HeadIntIList headlist);

int Add2CharList(struct CharListHead *list,char *cad,int mode);
int Add2CharListWindow(struct CharListHead *list,char *cad,int mode,struct Window *w);
int PrintCharList(struct CharListHead *hlist);
int DestroyCharList(struct CharListHead *hlist);

int Add2TextList(struct HeadTextList *head,char *cad,int color);
int DestroyTextList(struct HeadTextList *head);
int PrintTextList(struct HeadTextList *head);
int PosFirstTextList(struct HeadTextList *head,int m);
int PosLastTextList(struct HeadTextList *head,int m);
int CountTextList(struct HeadTextList *head);
int CountColorTextList(struct HeadTextList *head,int m);

#endif
