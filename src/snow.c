 /*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyright(C) 2006-2013  MRevenga
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


#include <stdlib.h>
#include <stdio.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <math.h>

#include "snow.h"
#include "general.h"
#include "functions.h"
#include "objects.h"


#define NFLAKES 1000


void CreateSnow(int w,int h){
  int i;
  flakes=malloc(NFLAKES*sizeof(struct Flake));
  if(flakes==NULL){
    fprintf(stderr,"ERROR in malloc:CreateSnow()\n"); 
    exit(-1); 
  }

  for(i=0;i<NFLAKES;i++){
    flakes[i].x= ((float)w*rand())/RAND_MAX;
    flakes[i].y= ((float)h*rand())/RAND_MAX;
    flakes[i].vx= (1.0*rand())/RAND_MAX-0.5;
    flakes[i].vy= (1.0*rand())/RAND_MAX;
    flakes[i].s= 3.+(3.*rand())/RAND_MAX;
  }
}


void UpdateFlakes(struct Planet *planet){
  int i;
  float v,fy;
  static float wind=0;
  float bDT;
  static int cont=0;
  float mxwind=5;
  Segment s;

  bDT=.01*DT;
  cont++;
  if(cont >150){
    cont =0;
    wind=(mxwind*rand())/RAND_MAX-mxwind/2;
  }

  fy=0.5*5*G*DT;

  for(i=0;i<NFLAKES;i++){
    v=sqrt(flakes[i].vx*flakes[i].vx+flakes[i].vy*flakes[i].vy);

    flakes[i].x+=flakes[i].vx*DT - bDT*v*flakes[i].vx*flakes[i].s*DT + wind*DT*DT;
    flakes[i].y+=flakes[i].vy*DT - fy*DT - bDT*v*flakes[i].vy*flakes[i].s*DT;

    flakes[i].vx+=-bDT*v*flakes[i].vx*flakes[i].s +wind*DT;
    flakes[i].vy+=- fy - bDT*v*flakes[i].vy*flakes[i].s; 

    if(flakes[i].x<0){
      flakes[i].x+=LXFACTOR;
    }
    if(flakes[i].x>LXFACTOR){
      flakes[i].x-=LXFACTOR;
    }

    /* Collision with planet terrain */
    if(flakes[i].y<LYFACTOR/2){
      if(!GetSegment(&s,planet,flakes[i].x,flakes[i].y)){
	
	if(flakes[i].y <= s.y0 || flakes[i].y <= s.y1){
	  flakes[i].y+=LYFACTOR;
	  flakes[i].vy*=.5;
	}
      }
    }
  }
}



void DrawFlakes(GdkPixmap *pixmap,GdkGC *pen,int w,int h){
  int i;
  float gwidth,gheight,sx,sy;

  gwidth=GameParametres(GET,GWIDTH,0);
  gheight=GameParametres(GET,GHEIGHT,0);
  sx=(float)gwidth/LXFACTOR;
  sy=(float)gheight/LYFACTOR;

  for(i=0;i<NFLAKES;i++){
    gdk_draw_point(pixmap,pen,flakes[i].x*sx,gheight-flakes[i].y*sy); 
  }
}
