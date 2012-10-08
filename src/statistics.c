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
#include "general.h"
#include "functions.h"
#include "statistics.h"

#define NSTATS 64

int lasttime;
int inctime;
int n;
int cont;
int np;
struct Stats stats_tmp;
struct Stats stats[NSTATS];


extern GdkGC *penGreen;
extern GdkGC *penBlack;
extern GdkGC *gcolors[];
extern struct Player *players;

/****************************/


void InitStatistics(void){
  int i,j;

  lasttime=GetTime();
  inctime=24;
  n=0;
  cont=0;
  np=GameParametres(GET,GNPLAYERS,0)+2;
  //  printf("InitStats: %d \n",np);

  stats_tmp.time=lasttime;

  for(i=0;i<np;i++){
    stats_tmp.level[i]=0;
    stats_tmp.nplanets[i]=0;
    for(j=0;j<NSTATS;j++){
      stats[j].level[i]=0;
      stats[j].nplanets[i]=0;
    }
  }
}


void AddStatistics(struct Stats *s){
  int i,j,k;

  stats_tmp.time=GetTime();

  cont++;
  for(i=0;i<np;i++){
    stats_tmp.level[i]+=s->level[i];
    stats_tmp.nplanets[i]+=s->nplanets[i];
    //    printf("%d %d\n",i,stats_tmp.nplanets[i]);
  }
  /*   */
  if(stats_tmp.time>lasttime+inctime){
    /* add to logger */
    if(0)printf("Stats: logging %d %d %d\n",cont,n,np);

    for(i=0;i<np;i++){
      stats[n].level[i]=stats_tmp.level[i]/cont;
      stats[n].nplanets[i]=stats_tmp.nplanets[i]/cont;

      stats_tmp.level[i]=0;
      stats_tmp.nplanets[i]=0;
    }

    n++;
    lasttime=stats_tmp.time;
    cont=0;

    if(n==NSTATS){
      /* TODO renormalize */
      //      printf("Stats: renormalize\n");
      k=0;
      for(j=0;j<n;j+=2){
	for(i=0;i<np;i++){
	  stats[k].level[i]=(stats[j].level[i]+stats[j+1].level[i])/2;
	  stats[k].nplanets[i]=(stats[j].nplanets[i]+stats[j+1].nplanets[i])/2;
	}
	k++;
      }
      n/=2;
      inctime*=2;
      //      PrintStatistics();
    }
  }
}


void PrintStatistics(void){
  int i;

  printf("stats %d %d\n",lasttime, inctime);
  for(i=1;i<np;i++){
    printf("%d: %.1f %d \n",i,stats[NSTATS-1].level[i],stats[NSTATS-1].nplanets[i]);
  }
  printf("\n");
}


void fprintStatistics(FILE *fp){
  int i,j;
  
  fprintf(fp,"%d %d %d %d %d ",lasttime,inctime,n,np,cont);

  for(i=0;i<NSTATS;i++){
    for(j=0;j<np;j++){
      fprintf(fp,"%.1f %d ",stats[i].level[j],stats[i].nplanets[j]);
    }
  }

  fprintf(fp,"%d ",stats_tmp.time);
  for(i=0;i<np;i++){
    fprintf(fp,"%.1f %d ",stats_tmp.level[i],stats_tmp.nplanets[i]);
  }

  fprintf(fp,"\n");
}


void fscanfStatistics(FILE *fp){
  int i,j;

  InitStatistics();

  if(fscanf(fp,"%d%d%d%d%d",&lasttime,&inctime,&n,&np,&cont)!=5){
    perror("fscanf");
    exit(-1);
  }

  for(i=0;i<NSTATS;i++){
    for(j=0;j<np;j++){
      if(fscanf(fp,"%f",&stats[i].level[j])!=1){
	perror("fscanf");
	exit(-1);
      }
      if(fscanf(fp,"%d",&stats[i].nplanets[j])!=1){
	perror("fscanf");
	exit(-1);
      }
    }
  }
  if(fscanf(fp,"%d",&stats_tmp.time)!=1){
    perror("fscanf");
    exit(-1);
  }
  for(i=0;i<np;i++){
    if(fscanf(fp,"%f%d",&stats_tmp.level[i],&stats_tmp.nplanets[i])!=2){
	perror("fscanf");
	exit(-1);
    }
  }
}


void DrawStatistics(GdkPixmap *pixmap,Rectangle *r){

  int i,j;
  int maxnplanets;
  float maxlevel;
  float dx,dy;
  int x0,x1,y0,y1;


  gdk_draw_rectangle(pixmap,    
		     penBlack,
		     TRUE,   
		     r->x,
		     r->y,
		     r->width,r->height);


  gdk_draw_rectangle(pixmap,    
		     penGreen,
		     FALSE,   
		     r->x,
		     r->y,
		     r->width,r->height);


  /* max value */
  maxnplanets=0;
  maxlevel=0;


  for(i=0;i<np;i++){
    for(j=0;j<NSTATS;j++){
      if(stats[j].level[i]>maxlevel)maxlevel=stats[j].level[i];
      if(stats[j].nplanets[i]>maxnplanets)maxnplanets=stats[j].nplanets[i];
    }
    if(players[i].level>maxlevel)maxlevel=players[i].level;
    if(players[i].nplanets>maxnplanets)maxnplanets=players[i].nplanets;
  }


      
  dx=(float)r->width/n;
  dy=(float)r->height/maxlevel;
  dy*=0.9;
  
  for(i=1;i<np;i++){
    x0=r->x;
    y0=r->y+r->height-stats[0].level[i]*dy;

    for(j=0;j<n-1;j++){
      x1=r->x+(j+1)*dx;
      y1=r->y+r->height-stats[j+1].level[i]*dy;
      gdk_draw_line(pixmap,    
		    gcolors[players[i].color],
		    x0,y0,
		    x1,y1);
      x0=x1;
      y0=y1;
      
    }
    x1=r->x+(j+1)*dx;
    y1=r->y+r->height-players[i].level*dy;
    gdk_draw_line(pixmap,    
		  gcolors[players[i].color],
		  x0,y0,
		  x1,y1);
  }
  
}
