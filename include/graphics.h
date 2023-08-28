/*****************************************************************************
**  This is part of the SpaceZero program
**  Copyright (C) 2006-2022  MRevenga
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

/*************  SpaceZero  M.R.H. 2006-2022 ******************
                Author: MRevenga
                E-mail: mrevenga at users.sourceforge.net
                version 0.86 November 2022
**************************************************************/

#ifndef _GRAPHICS_
#define _GRAPHICS_

#include "objects.h"
#include "statistics.h"

extern GdkFont* gfont;

extern GtkWidget* d_a;
extern GtkWidget* win_main;

extern struct Draw gdraw;
extern int gdrawmenu;

/* constants used by DrawMessageBox() */
#define MBOXDEFAULT 0
#define MBOXBORDER 1

#define VIEW_NONE 0
#define VIEW_SPACE 1
#define VIEW_MAP 2
#define VIEW_PLANET 3
#define VIEW_SHIP 4

#define DOT 1
#define LINE 2
#define CIRCLE 3

/* colors */

#define BLACK 0
#define WHITE 1
#define RED 2
#define LIGHTGREEN 3
#define GREEN 4
#define BLUE 5
#define YELLOW 6
#define ORANGE 7
#define VIOLET 8
#define PINK 9
#define CYAN 10
#define SOFTRED 11
#define GREY 12

struct Stars
{
  int* color;
  int *x, *y;
};

struct Draw
{
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

GtkWidget* InitGraphics(char* title, char* optfile, int, int, struct Parametres param);
void CreatePixmap();
GdkFont* InitFonts(char* fontname);
GdkFont* InitFontsMenu(char* fname);
gint QuitGraphics(GtkWidget* widget, gpointer gdata);
gint configure_event(GtkWidget* widget, GdkEventConfigure* event);
gint SizeRequest(GtkWidget* widget, GdkEventConfigure* event);
gint expose_event(GtkWidget* widget, GdkEventExpose* event);
gint GotFocus(GtkWidget* widget, gpointer data);
gint LostFocus(GtkWidget* widget, gpointer data);
gint button_press(GtkWidget* widget, GdkEventButton* event);
gint button_release(GtkWidget* widget, GdkEventButton* event);
gint motion_notify(GtkWidget* widget, GdkEventMotion* event);
void key_press(GtkWidget* widget, GdkEventKey* event, gpointer data);
void key_release(GtkWidget* widget, GdkEventKey* event, gpointer data);
GtkWidget* CreateSubMenu(GtkWidget* menu, char* szName);
gint ShowWindow(GtkWidget* widget, gpointer gdata);
gint QuitWindow(GtkWidget* widget, gpointer gdata);

gint ShowWindowOptions(GtkWidget* widget, gpointer gdata);
gint SaveWindowOptions(GtkWidget* widget, gpointer gdata);

gint SetDefaultOptions(GtkWidget* widget, gpointer gdata);
gint PrintMessage(GtkWidget* widget, gpointer gdata);
GtkWidget* CreateMenuItem(GtkWidget* menu,
                          char* Name,
                          char* Accel,
                          char* Tip,
                          GtkSignalFunc func,
                          gpointer data);
GtkWidget* CreateBarSubMenu(GtkWidget* menu, char* szName);

int CountKey(int mode);

GdkColor* NewColor(int red, int green, int blue);
GdkGC* GetPen(GdkColor* c, GdkPixmap* pixmap);

void DrawPoint(GdkGC* gc, int x, int y);
void DrawLine(GdkGC* gc, int x1, int y1, int x2, int y2);
void DrawRectangle(GdkGC* gc, int fill, int x, int y, int w, int h);
void DrawText(GdkFont* font, GdkGC* gc, gint x, gint y, const gchar* string);

int DrawObjs(struct HeadObjList*, struct Habitat habitat, Object* cv, Vector r_rel);
void DrawShip(GdkGC* gc, int x, int y, Object* obj);
void DrawPlanet(int x, int y, int r);
void DrawStars(int, float, float);
void DrawPlanetSurface(Object*);
void DrawAsteroid(int x, int y, Object* obj);
int DrawRadar(Object*, struct HeadObjList*, int crash);
void DrawMap(int player, struct HeadObjList, Object* cv, int ulx);
void DrawSpaceShip(Object* obj, struct HeadObjList* lhc);

int DrawGameStatistics(struct Player* pl);

void DrawInfo(Object*, struct Draw*, struct HeadObjList*, struct TextMessageList*);
int DrawPlayerInfo(GdkFont* font, GdkGC* color, struct Player* player, int x0, int y0);
int DrawShipInfo(GdkFont* font, GdkGC* color, Object* obj, int x0, int y0);
int DrawEnemyShipInfo(GdkFont* font, GdkGC* color, Object*, int, int);
int DrawPlanetInfo(GdkFont* font, GdkGC* color, Object* planet, int player, int x0, int y0);
void DrawPlayerList(int player, struct HeadObjList*, Object*, int);

int DrawTextList(GdkFont* font, char* title, struct HeadTextList* head, int x0, int y0, int width, int height);
void DrawString(GdkFont* font, GdkGC* gc, gint x, gint y, const gchar* string);
void DrawMessageBox(GdkFont* font, char* cad, int x0, int y0, int type);

void DrawCharList(GdkFont* font, GdkGC* color, struct CharListHead* hlist, int x0, int y0);
void DrawWindow(GdkFont* font, GdkGC* color, int x0, int y0, int type, struct Window* W);
void DrawBarBox(GdkGC* border, GdkGC* color, int x0, int y0, int w, int h, float value);
void DrawSelectionBox(GdkGC* color, int view, Space reg, Object* cv);
int DrawMenuHead(GdkFont* font, struct MenuHead* head, int x0, int y0);

int WindowFocus(struct Window* w);
int ActWindow(struct Window* w);

gint Save(GtkWidget* widget, gpointer gdata);
gint Load(GtkWidget* widget, gpointer gdata);
gint Options(GtkWidget* widget, gpointer gdata);

void Shift(int action, int ulx, int cvid, float* z, float* x, float* y);

void MousePos(int order, int* x, int* y);

void PrintFontNames(int n);
void PrintFontName(char* fname, int n);

void DrawStatistics(Rectangle* r, struct Stats* stats, int, int);

void InitColors(void);
GdkGC* GetColor(int);

void Real2Window(Object*, int view, int rx, int ry, int* wx, int* wy);
void Real2Sector(int x, int y, int* a, int* b);
void Window2Real(Object*, int view, int wx, int wy, int* rx, int* ry);
void Window2Sector(Object* cv, int* x, int* y);
void W2R(Object* cv, int* x, int* y);

#endif
