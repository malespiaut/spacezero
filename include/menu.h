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

#ifndef _MENU_
#define _MENU_
#define MAXMENULEN 16

/* menu item types */
#define MENUITEMTEXT 0
#define MENUITEMCHECKBOX 1
#define MENUITEMENTRY 2

struct MenuItem{
  int type;
  char text[MAXMENULEN];
};

struct MenuList{
  struct MenuItem item;
  struct MenuList *next;
};

struct HeadMenuList{
  int level;
  int n;
};

struct MenuList *Add2MenuList(struct MenuList *list0,struct MenuItem *item0);
void PrintMenuList(struct MenuList *list0);
struct MenuItem *GetMenuItem(struct MenuList *list0,int n);

#endif
