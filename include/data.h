 /*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyleft (C) 2006-2011  M.Revenga
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

#ifndef _DATA_
#define _DATA_ 

#include <stdio.h>

#define NINDEXILIST 20  // number of indexes
#define NINDEXCALC 100  // recalc indexes when this number of items are added

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




#endif
