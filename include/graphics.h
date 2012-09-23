 /*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyright (C) 2006-2012  MRevenga
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

#ifndef _GRAPHICS_
#define _GRAPHICS_

#include "menu.h"
#include "help.h"

/* constants used by DrawMessageBox() */
#define MBOXDEFAULT 0
#define MBOXBORDER 1

#define VIEW_NONE 0
#define VIEW_SPACE 1
#define VIEW_MAP 2
#define VIEW_PLANET 3
#define VIEW_SHIP 4

struct Stars{
  int *color;
  int *x,*y;
};

struct Draw{
  int main;
  int menu;
  int map;
  int shiplist;
  int stats;
  int gamelog;
  int order;
  int info;
  int crash;
  int volume;
};



GtkWidget *InitGraphics(char *title,char *optfile,int,int,struct Parametres param);
GdkFont *InitFonts(char *fontname);
GdkFont *InitFontsMenu(char *fname);
gint QuitGraphics(GtkWidget *widget,gpointer gdata);
gint configure_event(GtkWidget *widget, GdkEventConfigure *event);
gint SizeRequest(GtkWidget *widget, GdkEventConfigure *event);
gint expose_event(GtkWidget *widget, GdkEventExpose *event);
gint GotFocus(GtkWidget *widget,gpointer data);
gint LostFocus(GtkWidget *widget,gpointer data);
gint button_press(GtkWidget *widget,GdkEventButton *event);
gint button_release(GtkWidget *widget,GdkEventButton *event);
gint motion_notify(GtkWidget *widget,GdkEventMotion *event);
void key_press(GtkWidget *widget,GdkEventKey *event,gpointer data);
void key_release(GtkWidget *widget,GdkEventKey *event,gpointer data);
GtkWidget *CreateSubMenu(GtkWidget *menu,char *szName);
gint ShowWindow(GtkWidget *widget,gpointer gdata);
gint QuitWindow(GtkWidget *widget,gpointer gdata);

gint ShowWindowOptions(GtkWidget *widget,gpointer gdata);
gint SaveWindowOptions(GtkWidget *widget,gpointer gdata);

gint SetDefaultOptions(GtkWidget *widget,gpointer gdata);
gint PrintMessage(GtkWidget *widget,gpointer gdata);
GtkWidget *CreateMenuItem(GtkWidget *menu,
			  char *Name,char *Accel,char *Tip,
			  GtkSignalFunc func,
			  gpointer data);
GtkWidget *CreateBarSubMenu(GtkWidget *menu,char *szName);

int CountKey(int mode);

GdkColor *NewColor(int red,int green,int blue);
GdkGC *GetPen(GdkColor *c,GdkPixmap *pixmap);
void WriteCad(GdkPixmap *pixmap,char *cad,int x,int y,GdkGC *color);

int DrawObjs_00(GdkPixmap *pixmap,struct HeadObjList ,struct Habitat habitat,Object *cv,Vector r_rel);
int DrawObjs(GdkPixmap *pixmap,struct HeadObjList *,struct Habitat habitat,Object *cv,Vector r_rel);
void DrawShip(GdkPixmap *pixmap,GdkGC *gc,int x,int y,Object *obj);
void DrawPlanet(GdkPixmap *pixmap,int x,int y, int r);
void DrawStars(GdkPixmap *pixmap,int,float,float);
void DrawPlanetSurface(GdkPixmap *pixmap,struct Planet *planet,  GdkGC *color);
void DrawAsteroid(GdkPixmap *pixmap,int x,int y,Object *obj);
int DrawRadar(GdkPixmap *pixmap,Object *,struct HeadObjList *,int crash);
void DrawMap(GdkPixmap *pixmap,int player,struct HeadObjList,Object *cv,int ulx);
void DrawSpaceShip(GdkPixmap *pixmap,Object *obj,struct HeadObjList *lhc);

int DrawGameStatistics(GdkPixmap *pixmap,struct Player *pl);

int DrawPlayerInfo(GdkPixmap *pixmap,GdkFont *font,GdkGC *color,struct Player *player,int x0,int y0);
int DrawShipInfo(GdkPixmap *pixmap,GdkFont *font,GdkGC *color,Object *obj,int x0,int y0);
int DrawEnemyShipInfo(GdkPixmap *pixmap,GdkFont *font,GdkGC *color,Object *,int,int);
int DrawPlanetInfo(GdkPixmap *pixmap,GdkFont *font,GdkGC *color,Object *planet,int player,int x0,int y0);
void DrawPlayerList(GdkPixmap *pixmap,int player,struct HeadObjList *,Object *,int);

int XPrintTextList(GdkPixmap *pixmap,GdkFont *font,char *title,struct TextList *head,int x0,int y0,int width,int height);
void DrawString(GdkDrawable *pixmap,GdkFont *font,GdkGC *gc,gint x,gint y,const gchar *string);
void DrawMessageBox(GdkPixmap *pixmap,GdkFont *font,char *cad,int x0,int y0,int type);

void DrawCharList (GdkPixmap *pixmap,GdkFont *font,GdkGC *color,struct CharListHead *hlist,int x0,int y0);
void DrawWindow (GdkPixmap *pixmap,GdkFont *font,GdkGC *color,int x0,int y0,int type,struct Window *W);
void DrawBarBox (GdkPixmap *pixmap,GdkGC *color,int x0,int y0,int w,int h,float value);
int WindowFocus(struct Window *w);
int ActWindow(struct Window *w);


gint Save(GtkWidget *widget,gpointer gdata);
gint Load(GtkWidget *widget,gpointer gdata);
gint Options(GtkWidget *widget,gpointer gdata);

void SetDefaultKeyValues(struct Keys *key,int action);

void Shift(int action,int ulx,int cvid,float *z,float *x,float *y);

void MousePos(int order,int *x,int *y);

void DrawSelectionBox(GdkPixmap *pixmap,GdkGC *color,int view,Space reg,Object *cv);
void Real2Window(Object *,int view,int rx,int ry,int *wx,int *wy);
void Real2Sector(int x,int y,int *a,int *b);
void Window2Real(Object *,int view,int wx,int wy,int *rx,int *ry);
void Window2Sector(Object *cv,int *x,int *y);
void W2R(Object *cv,int *x,int *y);

int XPrintMenuHead(GdkPixmap *pixmap,GdkFont *font,struct MenuHead *head,int x0,int y0);

void PrintFontNames(int n);
void PrintFontName(char *fname,int n);
#endif
