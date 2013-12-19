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


#include <stdio.h>
#include "general.h"
#include "functions.h"
#include "statistics.h"
#include "graphics.h"

int lasttime;
int inctime;
int stats_n;
int stats_np;
int stats_cont;

struct Stats stats_tmp;
struct Stats stats[NSTATS];

extern GdkGC *penGreen;
extern GdkGC *penBlack;
extern GdkGC *gcolors[];


/****************************/


void InitStatistics(void){
  int i,j;

  lasttime=GetTime();
  inctime=24;
  stats_n=0;
  stats_cont=0;
  stats_np=GameParametres(GET,GNPLAYERS,0)+2;

  stats_tmp.time=lasttime;

  for(i=0;i<stats_np;i++){
    stats_tmp.level[i]=0;
    stats_tmp.nplanets[i]=0;
    for(j=0;j<NSTATS;j++){
      stats[j].level[i]=0;
      stats[j].nplanets[i]=0;
    }
  }
}


void UpdateStatistics(void){
  int i,j,k;
  struct Player *players;

  stats_tmp.time=GetTime();
  stats_cont++;
  
  players=GetPlayers();
  for(i=0;i<stats_np;i++){
    stats_tmp.level[i]+=players[i].level+players[i].nships;
    stats_tmp.nplanets[i]+=players[i].nplanets;
  }

  if(stats_tmp.time>lasttime+inctime){
    /* add to logger */

    for(i=0;i<stats_np;i++){
      stats[stats_n].level[i]=stats_tmp.level[i]/stats_cont;
      stats[stats_n].nplanets[i]=stats_tmp.nplanets[i]/stats_cont;

      stats_tmp.level[i]=0;
      stats_tmp.nplanets[i]=0;
    }

    stats_n++;
    lasttime=stats_tmp.time;
    stats_cont=0;

    if(stats_n==NSTATS){
      /* renormalize */
      k=0;
      for(j=0;j<stats_n-1;j+=2){ /* FIXED IN  0.84.01 */
	for(i=0;i<stats_np;i++){
	  stats[k].level[i]=(stats[j].level[i]+stats[j+1].level[i])/2;
	  stats[k].nplanets[i]=(stats[j].nplanets[i]+stats[j+1].nplanets[i])/2;
	}
	k++;
      }
      stats_n/=2;
      if(inctime<43200){ /* max 30 min */
	inctime*=2;
      }
    }
  }
}


void PrintStatistics(void){
  int i;


  printf("stats %d %d\n",lasttime, inctime);
  for(i=1;i<stats_np;i++){
    printf("%d: %.1f %d \n",i,stats[NSTATS-1].level[i],stats[NSTATS-1].nplanets[i]);
  }
  printf("\n");
}


void fprintStatistics(FILE *fp){
  int i,j;
  
  fprintf(fp,"%d %d %d %d %d ",lasttime,inctime,stats_n,stats_np,stats_cont);

  for(i=0;i<NSTATS;i++){
    for(j=0;j<stats_np;j++){
      fprintf(fp,"%.1f %d ",stats[i].level[j],stats[i].nplanets[j]);
    }
  }

  fprintf(fp,"%d ",stats_tmp.time);
  for(i=0;i<stats_np;i++){
    fprintf(fp,"%.1f %d ",stats_tmp.level[i],stats_tmp.nplanets[i]);
  }

  fprintf(fp,"\n");
}


void fscanfStatistics(FILE *fp){
  int i,j;

  InitStatistics();

  if(fscanf(fp,"%d%d%d%d%d",&lasttime,&inctime,&stats_n,&stats_np,&stats_cont)!=5){
    perror("fscanf");
    exit(-1);
  }

  for(i=0;i<NSTATS;i++){
    for(j=0;j<stats_np;j++){
      if(fscanf(fp,"%f",&stats[i].level[j])!=1){
	perror("fscanf");
	exit(-1);
      }
      if(stats[i].level[j]<0){
	fprintf(stderr,"ERROR fscanfStatistics(): level <0. Setting to zero\n");
	stats[i].level[j]=0;
      }
      if(fscanf(fp,"%d",&stats[i].nplanets[j])!=1){
	perror("fscanf");
	exit(-1);
      }
      if(stats[i].nplanets[j]<0){
	fprintf(stderr,"ERROR fscanfStatistics(): nplanets <0. Setting to zero\n");
	stats[i].nplanets[j]=0;
      }
    }
  }
  if(fscanf(fp,"%d",&stats_tmp.time)!=1){
    perror("fscanf");
    exit(-1);
  }
  for(i=0;i<stats_np;i++){
    if(fscanf(fp,"%f%d",&stats_tmp.level[i],&stats_tmp.nplanets[i])!=2){
	perror("fscanf");
	exit(-1);
    }
  }
}

void Statistics_Draw(GdkPixmap *pixmap,Rectangle *r){
  DrawStatistics(pixmap,r,stats,stats_n,stats_np);
}


