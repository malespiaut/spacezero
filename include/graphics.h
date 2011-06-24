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

#ifndef _GRAPHICS_
#define _GRAPHICS_

/* constants used by DrawMessageBox() */
#define MBOXDEFAULT 0
#define MBOXBORDER 1
#include "menu.h"
#include "help.h"

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
  int order;
  int info;
  int crash;
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

gint SaveOptions(GtkWidget *widget,gpointer gdata);
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
int DrawRadar(GdkPixmap *pixmap,Object *,struct HeadObjList *);
void DrawMap(GdkPixmap *pixmap,int player,struct HeadObjList,Object *cv,int ulx);

void DrawGameStatistics(GdkPixmap *pixmap,struct Player *pl);

int DrawPlayerInfo(GdkPixmap *pixmap,GdkFont *font,GdkGC *color,struct Player *player,int x0,int y0);
int DrawShipInfo(GdkPixmap *pixmap,GdkFont *font,GdkGC *color,Object *obj,int x0,int y0);
int DrawEnemyShipInfo(GdkPixmap *pixmap,GdkFont *font,GdkGC *color,Object *,int,int);
int DrawPlanetInfo(GdkPixmap *pixmap,GdkFont *font,GdkGC *color,Object *planet,int x0,int y0);
void DrawPlayerList(GdkPixmap *pixmap,int player,struct HeadObjList *,Object *,int);

int XPrintTextList(GdkPixmap *pixmap,GdkFont *font,char *title,struct TextList *head,int x0,int y0,int width,int height);
void DrawString(GdkDrawable *pixmap,GdkFont *font,GdkGC *gc,gint x,gint y,const gchar *string);
void DrawMessageBox(GtkWidget *d_area,GdkPixmap *pixmap,GdkFont *font,char *cad,int x0,int y0,int type);


gint Save(GtkWidget *widget,gpointer gdata);
gint Load(GtkWidget *widget,gpointer gdata);
gint Options(GtkWidget *widget,gpointer gdata);

void SetDefaultKeyValues(struct Keys *key,int action);

void Shift(int ulx,Object *cv,float *z,float *x,float *y,int action);


void DrawSelectionBox(GdkPixmap *pixmap,GdkGC *color,Region reg,Object *cv);
void Real2Window(Object *,int habitat,int rx,int ry,int *wx,int *wy);
void Window2Real(Object *,int habitat,int wx,int wy,int *rx,int *ry);

int XPrintMenuHead(GdkPixmap *pixmap,GdkFont *font,struct MenuHead *head,int x0,int y0);

#endif
