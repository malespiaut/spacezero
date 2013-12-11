#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#include "objects.h"


struct Flake *flakes;

struct Flake{
  float x,y;
  float vx,vy;
  float s;
};

void UpdateFlakes(struct Planet *planet);
void DrawFlakes(GdkPixmap *pxmap,GdkGC *pen,int x,int y);
void CreateSnow(int h,int w);
void DrawFlakes(GdkPixmap *pixmap,GdkGC *pen,int h,int w);

