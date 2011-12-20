 /*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyright(C) 2006-2011  M.Revenga
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


#include "sectors.h"


int SelectSector(struct HeadIntIList *hlist,Object *obj,int *a,int *b){
  /*
    Returns a sector in a,b
    Depending of the level of obj:
    level 0: a random sector
    level 1: possibly an unexplored sector.
    level 2: more possibly an unexplored sector.

   */

  int n,level,i,j,cont;
  float x,y;

  if(obj==NULL){
    fprintf(stderr,"ERROR 1 in SelectSector()\n");
    exit(-1); 
  }
  if(hlist==NULL){
    fprintf(stderr,"ERROR 2 in SelectSector()\n");
    exit(-1); 
  }
  if(a==NULL||b==NULL){
    fprintf(stderr,"ERROR 3 in SelectSector()\n");
    exit(-1); 
  }

  n=0;

  if(obj->in==NULL){
    x=obj->x;
    y=obj->y;
  }
  else{
    x=obj->in->x;
    y=obj->in->y;
  }


  *a=x;
  *b=y;

  level=obj->level;
  if(obj->subtype==EXPLORER)level++;

  switch(level){
  case 0: /* simplest: a random walk */
    cont=0;
    n=0;
    n=NearRandomSector(hlist,x,y,5);
    break;
  case 1: /* less simple, try to find an inexplore near sector  */
    cont=0;
    n=0;
    while(cont<3 && n==0){
      n=NearRandomSector(hlist,x,y,5);
      cont++;
    }
    if(n==0){
      n=NearestSector(hlist,x,y,5);
    }
    
    break;
  case 2: /* as case 1 but the zone is greater */
  default:
    cont=n=0;
    while(cont<10 && n==0){
      n=NearRandomSector(hlist,x,y,5);
      cont++;
    }
    
    if(n==0){
      n=NearestSector(hlist,x,y,8);
    }
    break;
  }

  if(n==0){ /* if no sector found, a near random one inside Universe */
    i=j=0;
    if(IsInUniverse(x,y)){
      i=(int)(6*(Random(-1))-3);
      j=(int)(6*(Random(-1))-3);
    }
    else{
      float maxx,maxy;
      maxx=0.55*GameParametres(GET,GULX,0);
      maxy=0.55*GameParametres(GET,GULY,0);
      if(x>maxx){
	i=-3;
      }
      else if(x<-maxx){
	i=3;
      }
      if(y>maxy){
	j=-3;
      }
      else if(y<-maxy){
	j=3;
      }
    }
    
    n=Quadrant(x+i*SECTORSIZE,y+j*SECTORSIZE);
    
  }
  
  InvQuadrant(n,a,b);
  
  return(0);  
}


int NearestSector(struct HeadIntIList *hlist,float a,float b,int n){
  /*
    version 03
    a,b ship coordinates.
    returns the nearest unknown id sector to the sector (a,b)
    looks far n sectors 
    returns 0 if all are known.
  */
  int i,j,k;
  int id;
  int rid=0;
  int d=n*n;
  int cont=0;
  float x,y;

  /* printf("================\nN:%d\n",n);  */

  for(k=1;k<=n;k++){
    /* horizontal borders */

    for(i=-k;i<=k;i++){
      for(j=-k;j<=k;j+=2*k){
	/* printf("%d %d\n",i,j);  */
	cont++;

	x=a+i*SECTORSIZE;
	y=b+j*SECTORSIZE;
	
	if(IsInUniverse(x,y)){
	  id=Quadrant(x,y);
	  if(!IsInIntIList(hlist,id)){
	    if(i*i+j*j<d){
	      rid=id;
	      d=i*i+j*j;
	    }
	  }
	}
      }
    }
    /* vertical borders */

    for(j=-k+1;j<k;j++){
      for(i=-k;i<=k;i+=2*k){
	cont++;
	/* printf("%d %d\n",i,j);  */
	x=a+i*SECTORSIZE;
	y=b+j*SECTORSIZE;

	if(IsInUniverse(x,y)){
	  id=Quadrant(a+i*SECTORSIZE,b+j*SECTORSIZE);
	  if(!IsInIntIList(hlist,id)){
	    if(i*i+j*j<d){
	      rid=id;
	      d=i*i+j*j;
	    }
	  }
	}
      }
    }
    if(rid!=0)break;
  }
  /* printf("===== %d ======\n",cont);  */
  return(rid);
}


int NearRandomSector(struct HeadIntIList *hlist,float a,float b,int n){
  /*
    a,b: sector coordinates.
    returns a random sector near position a,b
    looks far n sectors.
    if sector is known return 0.
    if sector is out if Universe returns 0 (try 2 times).
  */
  int i,j;
  int id;
  float x,y;
  int cont=0;
  int inside=0;
  int ret=0;

  while(cont<1 && inside==0){ /* avoiding stay on universe border */
    i=(int)((2*n)*(Random(-1))-n);
    j=(int)((2*n)*(Random(-1))-n);
    
    /* check if is out universe border */
    x=a+i*SECTORSIZE;
    y=b+j*SECTORSIZE;
    
    inside=IsInUniverse(x,y);
    cont++;
  }
  
  if(inside){
    id=Quadrant(x,y);
    if(!IsInIntIList(hlist,id)){
      ret=id;
    }
  }
  return(ret);
}


int Quadrant(float x, float y){
  /* 
     x,y real position of an object
     return the quadrant id
  */

  int i,j;
  int r,r0;
  int d,s;
  int n;


  i=x/SECTORSIZE-(x<0);
  j=y/SECTORSIZE-(y<0);

  r=MODI(i) > MODI(j) ? MODI(i) : MODI(j);

  r0=0;
  if(r!=0)r0=4*r*(r-1)+1; /* (2*r-1)*(2*r-1); */
  d=2*r-i-j;

  if(d!=0){
    s=0;
    if(i<=j)s=-1;
    
    n=r0+2*d+s;
  }
  else{
    n=r0;
  }
  return(n);
}

void InvQuadrant(int n,int *x, int *y){
  /* 
     version 00
     input n: quadrant id
     output: the real position in x, y
     HEREOPT use a sqrt table.
  */

  int i,j;
  int r;
  int d;
  /*
#if DEBUG
  static int max=0;
  static int min =0;
  if(n>max){max=n;printf("MAX=%d\n",max);}
  if(n<min){min=n;printf("MIN=%d\n",min);}
#endif
  */
  r=(int)((sqrt(n)+1.0)*0.5);

  if(r==0){
    /* r0=0; */
    d=(int)((n+1)*0.5);
  }
  else{
    /*    r0=4*r*(r-1)+1;  (2*r-1)*(2*r-1); */
    d=(int)(0.5*(n-4*r*(r-1)));
  }

  if(!(n%2)){ /*   n es par */
    i=r-d;
    if(MODI(i)>r){
      j=r-(MODI(i)-r);
      i=-r;
    }
    else{
      j=2*r-i-d;
    }
  }
  else{ /*  n es impar */
    j=r-d;
    if(MODI(j)>r){
      i=r-(MODI(j)-r);
      j=-r;
    }
    else{
      i=2*r-j-d;
    }
  }
  /*   if(i<0)i++; */
  /*   if(j<0)j++; */
  *x=i;
  *y=j;
  return;
}

int  IsInUniverse(float a,float b){
  /*
    check if the point a,b is inside the Universe.
    a,b: ship coordinates.
    returns:
    0 if not
    1 its inside 
  */

  float maxx,maxy;
  
  maxx=0.55*GameParametres(GET,GULX,0);
  maxy=0.55*GameParametres(GET,GULY,0);
  
  if(a>maxx || a<-maxx || b>maxy || b<-maxy){
    return(0);
  }
  return(1);
}
