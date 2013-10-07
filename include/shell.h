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
		version 0.84 April 2013
****/

#ifndef _SHELL_
#define _SHELL_

#include <gtk/gtk.h>
#include "objects.h"

/* keystrokes */
#define RESET 0
#define ADD 1
#define DELETELAST 2
#define RETURNLAST 3
#define LOAD 4



void initshell(void);
void ShellTitle(int order,char *mess,GdkPixmap *pixmap,GdkGC *color,GdkFont *font,int x,int y);
int Shell(int command,GdkPixmap *pixmap,GdkGC *color,GdkFont *,struct HeadObjList *hl,struct Player *players,struct Keys *,Object **cv,char *cad);
Object *ExecOrder(struct HeadObjList *hl,Object *obj,int player,int order,char *par);
void SelectionBox(GdkPixmap *pixmap,GdkGC *color,Object **,int);
int Keystrokes(int mode,guint *val,char *c);
char Keyval2Char(guint keyval);

int Get2Args(char *cad,char *arg1,char *arg2);

#endif
