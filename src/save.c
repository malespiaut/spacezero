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
		version 0.84 april 2013
**************************************************************/

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "save.h"


extern int actual_player,actual_player0;
extern int record;
extern int nav_mode;
extern int g_objid;
extern int g_projid;
extern char version[64];
extern Object *ship_c; /* ship controled by keyboard */
extern struct HeadObjList listheadobjs;
extern struct HeadObjList *listheadcontainer; /* lists of objects that contain objects: free space and planets*/
extern struct HeadObjList *listheadkplanets; /* list of all planets */
extern struct Habitat habitat;

extern struct Global gclient;
extern int fobj[4];

extern Object *cv;     /* coordenates center */
extern struct Player *players;

extern struct CCDATA *ccdatap; /* HERE set to number of players */
extern int *cell;

struct Global gremote,glocal;
#if DEBUG
int debugsave=0;
#endif


int CreateDir(char *dir){
  /*
    Create the directory dir
    returns:
    0 if the directory has been succesfully created or if it exist
    -1 if some error occurs and is not created.
  */  
  
  errno=0;
  if(mkdir(dir,S_IFDIR|S_IRUSR|S_IWUSR|S_IXUSR)==-1){
    if(errno!=EEXIST){
      perror("mkdir");
      printf("Can't create directory: %s\n",dir);
      return(-1);
    }
  }
  else{ 
    printf("created directory: %s\n",dir); 
  } 
  return(0);
}

char *CreateSaveFile(int server,int client){
  
  char *file;
  int ret;
  
  file=malloc(MAXTEXTLEN*sizeof(char));
  if(file==NULL){
    fprintf(stderr,"ERROR in malloc ExecLoad()\n");
    exit(-1);
  }
  
  strcpy(file,"");
  if(server==TRUE){
    ret=snprintf(file,MAXTEXTLEN,"%s/%s/%s",getenv("HOME"),SAVEDIR,SAVEFILENET);
    if(ret>=MAXTEXTLEN){
      fprintf(stderr,"string too long. Truncated to:\"%s\"",file);
    }
  }
  else{
    ret=snprintf(file,MAXTEXTLEN,"%s/%s/%s",getenv("HOME"),SAVEDIR,SAVEFILE0);
    if(ret>=MAXTEXTLEN){
      fprintf(stderr,"string too long. Truncated to:\"%s\"",file);
    }
  }
  
  if(client==TRUE){
    ret=snprintf(file,MAXTEXTLEN,"%s/%s/%s",getenv("HOME"),SAVEDIR,SAVEFILE1);
    if(ret>=MAXTEXTLEN){
      fprintf(stderr,"string too long. Truncated to:\"%s\"",file);
    }
  }
  
  
  return(file);
}

char *CreateRecordFile(void){
  char *file;
  FILE *fprecord;
  int ret;

  file=malloc(MAXTEXTLEN*sizeof(char));
  if(file==NULL){
    fprintf(stderr,"ERROR in malloc ExecLoad()\n");
    exit(-1);
  }
  ret=snprintf(file,MAXTEXTLEN,"%s/%s/%s",getenv("HOME"),SAVEDIR,RECORDFILE);
  if(ret>=MAXTEXTLEN){
    fprintf(stderr,"string too long. Truncated to:\"%s\"",file);
  }  
  
  if((fprecord=fopen(file,"rt"))==NULL){
    
    if((fprecord=fopen(file,"wt"))==NULL){
      fprintf(stdout,"Can't open the file: %s",file);
      exit(-1);
    }
    fprintf(fprecord,"%d\n",0);
    fclose(fprecord);
    
    if((fprecord=fopen(file,"rt"))==NULL){
      fprintf(stdout,"Can't open the file: %s", file);
      exit(-1);
    }
  }
  
  if(fscanf(fprecord,"%d",&record)!=1){
    fprintf(stderr,"Setting record to 0\n");
    record=0;
  }
  
  fclose(fprecord);
  printf("Record: %d\n",record);
  
  return(file);
}



char *CreateOptionsFile(void){
  /*
    check if exists the option file.
    if no, creates it.
  */
  char *file;  
  int ret;  

  file=malloc(128*sizeof(char));
  if(file==NULL){
    fprintf(stderr,"ERROR in malloc ExecLoad()\n");
    exit(-1);
  }
  
  ret=snprintf(file,MAXTEXTLEN,"%s/%s/",getenv("HOME"),SAVEDIR);
  if(ret>=MAXTEXTLEN){
    fprintf(stderr,"string too long. Truncated to:\"%s\"",file);
  }
  
  /* Test if the configuration directory exists */
  if(CreateDir(file)!=0){
    printf("Can't create directory: %s\n",file);
    exit(-1);
  }
  /* --Test if the directory exists */
  
  ret=snprintf(file,MAXTEXTLEN,"%s/%s/%s",getenv("HOME"),SAVEDIR,OPTIONSFILE);
  if(ret>=MAXTEXTLEN){
    fprintf(stderr,"string too long. Truncated to:\"%s\"",file);
  }
  return(file);
}


char *CreateKeyboardFile(void){
  /*
    check if exists the keyborad file.
    if no, creates it.
  */
  char *file;  
  int ret;
  
  file=malloc(128*sizeof(char));
  if(file==NULL){
    fprintf(stderr,"ERROR in malloc ExecLoad()\n");
    exit(-1);
  }
  
  ret=snprintf(file,MAXTEXTLEN,"%s/%s/",getenv("HOME"),SAVEDIR);
  if(ret>=MAXTEXTLEN){
    fprintf(stderr,"string too long. Truncated to:\"%s\"",file);
  }
  
  /* Test if the configuration directory exists */
  if(CreateDir(file)!=0){
    printf("Can't create directory: %s\n",file);
    exit(-1);
  }
  /* --Test if the directory exists */
  
  ret=snprintf(file,MAXTEXTLEN,"%s/%s/%s",getenv("HOME"),SAVEDIR,KEYBOARDFILE);
  if(ret>=MAXTEXTLEN){
    fprintf(stderr,"string too long. Truncated to:\"%s\"",file);
  }

  return(file);
}



int ExecSave(struct HeadObjList lh,char *nom){
  /*
    Save all the game to a file
  */
  struct ObjList *ls;
  int sc,sv;
  FILE *fp;
  int i,n;
  int habitat_type,habitat_obj;
  int nplanets,nsectors;
  float control;
  struct IntList *ks,*kps;
  struct CCDATA *ccdata;
  
  if((fp=fopen(nom,"wt"))==NULL){
    fprintf(stdout,"ExecSave(): I can't open file %s\n",nom);
    return(1);
  }
  
  fprintf(stdout,"Save(): Saving the game to %s ...\n",nom);
  
  CheckObjsId();
  /* version */
  fprintf(fp,"%s\n",version);
  /*control*/
  control=((float)rand()/RAND_MAX);
  fprintf(fp,"%f\n",control);
  
  /* net or local game */ 
  fprintf(fp,"%d\n",GameParametres(GET,GNET,0));
  
  /* Saving the number of objects */
  n=0;
  ls=lh.list;
  while(ls!=NULL){
    switch(ls->obj->type){
    case TRACKPOINT:
    case TRACE:
      ls=ls->next;continue;
      break;
    default:
      break;
    }    
    
    n++;
    ls=ls->next;
  }
  
  fprintf(fp,"%d\n",n);
  
  /* Global variables */
  
  printf("record: %d\n",record);
  
  fprintf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d\n",
	  GameParametres(GET,GWIDTH,0),GameParametres(GET,GHEIGHT,0),
	  GameParametres(GET,GNET,0),
	  GameParametres(GET,GNGALAXIES,0),GameParametres(GET,GNPLAYERS,0),
	  GameParametres(GET,GNPLANETS,0),GameParametres(GET,GKPLANETS,0),
	  GameParametres(GET,GPIRATES,0),GameParametres(GET,GENEMYKNOWN,0),
	  GameParametres(GET,GCOOPERATIVE,0),
	  GameParametres(GET,GCOMPCOOPERATIVE,0),GameParametres(GET,GQUEEN,0));
  
  
  fprintf(fp,"%d %d %d %d %d %d %d %d\n",
	  GameParametres(GET,GULX,0),GameParametres(GET,GULY,0),
	  actual_player0,record,nav_mode,GetTime(),
	  g_objid,g_projid);
  
  sc=sv=0;
  if(ship_c!=NULL)sc=ship_c->id;
  if(cv!=NULL)sv=cv->id;
  
  fprintf(fp,"%d %d\n",sc,sv);
  
  habitat_type=habitat.type;
  habitat_obj=0;
  if(habitat.obj!=NULL){
    habitat_obj=habitat.obj->id;
    /*    habitat_planet=habitat_obj; */
  }
  
  fprintf(fp,"%d %d\n",habitat_type,habitat_obj);
  
  glocal.actual_player=actual_player0;
  glocal.g_objid=g_objid;
  glocal.g_projid=g_projid;
  glocal.ship_c=0;
  if(ship_c!=NULL){
    glocal.ship_c=ship_c->id;
  }
  glocal.cv=0;
  if(cv!=NULL){
    glocal.cv=cv->id;
  }
  glocal.habitat_type=habitat.type;
  glocal.habitat_id=0;
  if(habitat.obj!=NULL)
    glocal.habitat_id=habitat.obj->id;
  for(i=0;i<4;i++)
    glocal.fobj[i]=fobj[i];
  
  fprintf(fp,"%d %d %d %d %d %d %d %d %d %d %d\n",
	  glocal.actual_player,
	  glocal.g_objid,
	  glocal.g_projid,
	  glocal.ship_c,
	  glocal.cv,
	  glocal.habitat_type,
	  glocal.habitat_id,
	  glocal.fobj[0],
	  glocal.fobj[1],
	  glocal.fobj[2],
	  glocal.fobj[3]);
  fprintf(fp,"%d %d %d %d %d %d %d %d %d %d %d\n",
	  gclient.actual_player,
	  gclient.g_objid,
	  gclient.g_projid,
	  gclient.ship_c,
	  gclient.cv,
	  gclient.habitat_type,
	  gclient.habitat_id,
	  gclient.fobj[0],
	  gclient.fobj[1],
	  gclient.fobj[2],
	  gclient.fobj[3]);
  
  for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    nplanets=CountIntList(players[i].kplanets);
    
    nsectors=0;
    ks=players[i].ksectors.list;
    while(ks!=NULL){
      nsectors++;
      ks=ks->next;
    }
    if(nsectors!=players[i].ksectors.n){
      fprintf(stderr,"Error in num. of sectors in ExecSave(). player: %d\n",i);
      fprintf(stderr,"\tnsectors: %d ksectors: %d\n",nsectors,players[i].ksectors.n);
      fprintf(stderr,"\tnsectors: using value nsectors: %d\n",nsectors);
      players[i].ksectors.n=nsectors;
      exit(-1);  /* HERE  */
    }
    
    nsectors=players[i].ksectors.n;
    fprintf(fp,"%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %f %d %d %d %d %d %d %f %d %d ",
	    players[i].playername,
	    players[i].id,
	    players[i].status,
	    players[i].pid,
	    players[i].proc,
	    players[i].control,
	    players[i].team,
	    players[i].profile,
	    players[i].strategy,
	    players[i].gmaxlevel,
	    players[i].maxlevel,
	    players[i].color,
	    players[i].cv,
	    players[i].nplanets,
	    players[i].nships,
	    players[i].nbuildships,
	    players[i].gold,
	    players[i].lastaction,
	    players[i].ndeaths,
	    players[i].nkills,
	    players[i].points,
	    players[i].goldships,
	    players[i].goldupdates,
	    players[i].goldweapon,
	    nplanets,nsectors);
    
    if(nplanets!=0){
      kps=players[i].kplanets;
      while(kps!=NULL){
	fprintf(fp,"%d ",kps->id);
	kps=kps->next;
      }
    }
    
    if(nsectors!=0){
      ks=players[i].ksectors.list;
      while(ks!=NULL){
	fprintf(fp,"%d ",ks->id);
	ks=ks->next;
      }
    }
    
    fprintf(fp,"\n");
  }
  
  /* --Global variables */
  
  /* 
   *   Saving the objects 
   */  

  ls=lh.list;
  while(ls!=NULL){
    
    switch(ls->obj->type){
    case TRACKPOINT:
    case TRACE:
      ls=ls->next;continue;
      break;
    default:
      break;
    }
    
    FprintfObj(fp,ls->obj);

    fprintf(fp,"%d %d %d ",
	    ls->obj->cargo.capacity,
	    ls->obj->cargo.mass,
	    ls->obj->cargo.n);
    fprintf(fp,"\n");
    ls=ls->next;
  }

  /* cargo list*/
  ls=lh.list; 
  while(ls!=NULL){ 
    switch(ls->obj->type){
    case TRACKPOINT:
    case TRACE:
      ls=ls->next;continue;
      break;
    default:
      break;
    }

    fprintf(fp,"%d %d ",ls->obj->id,ls->obj->cargo.n); 
    if(ls->obj->cargo.hlist!=NULL){  
      struct ObjList *ls2;  
      ls2=ls->obj->cargo.hlist->list;  
      while(ls2!=NULL){  
 	fprintf(fp,"%d ",ls2->obj->id);
 	ls2=ls2->next;  
      }  
    } 
    fprintf(fp,"\n"); 
    ls=ls->next; 
  } 
  /* --cargo list*/
  
  /* ccdata */
  
  for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    ccdata=&ccdatap[i];
    FprintfCCData(fp,ccdata);
  }  
  
  /* --ccdata*/
  
  /* statistics */
  fprintStatistics(fp);
  
  /* --statistics */
  
  
  fprintf(fp,"%f\n",control);
  fclose(fp);
  /*  printf("control: %f\n",control); */
  /*  printf("done\n"); */
  return(0);
}

int ExecLoad(char *nom){
  /*
    load a game
  */
  
  Object obj,*nobj,*obj0,*obj1;
  struct ObjList *ls;
  struct Planet *nplanet;
  int i,j,num_objs;
  int id,projid;
  FILE *fp;
  int sc,sv;
  struct ObjTable *tbl;
  int habitat_type,habitat_obj;
  float control,control2;
  struct Global *global;
  int width,height;
  int gtime;
  char versionfile[MAXTEXTLEN];
  int net;
  int tmpint;
  int ulx,uly;
  Data *data;
  struct CCDATA *ccdata;
  int nkp,nks;
  int *planet2meetid,*planet2attackid; /* HERE set to number of players */
  int nx,ny;
  
  /* 
   *     Del all the objects ...
   */
  
  if((fp=fopen(nom,"rt"))==NULL){
    fprintf(stdout,"ExecLoad(): I can't open file %s\n",nom);return(1);
  }
  /* version */
  if(fscanf(fp,"%128s",versionfile)!=1){ /**/
    perror("fscanf");
    exit(-1);
  }
  /*control*/  
  if(fscanf(fp,"%f",&control)!=1){
    perror("fscanf");
    exit(-1);
  }
  
  if(strcmp(versionfile,MINORSAVEVERSION)>=0){
    printf("Version:  game:(%s)  save file:(%s) >= %s  ... OK\n",version,versionfile,MINORSAVEVERSION);
  }
  else if(strcmp(versionfile,MINORSAVEVERSION)<0){
    fprintf(stderr,"Error: incompatible versions.\n");
    fprintf(stderr,"\tVersion:  game:(%s)  save file:(%s) < %s\n",version,versionfile,MINORSAVEVERSION);
    fclose(fp);
    return(1);
  }
  
  if(fscanf(fp,"%d",&net)!=1){
    perror("fscanf");
    exit(-1);
  }
  
  if(net!=GameParametres(GET,GNET,0)){
    fprintf(stderr,"Error: incompatible mode game.\n");
    fclose(fp);
    return(1);
  }
  
  
  printf("Loading %s ...\n",nom);
  printf("\tDestroying objects...");
  fflush(NULL);
  
  DestroyAllObj(&listheadobjs);
  listheadobjs.list=NULL;
  listheadobjs.n=0;
  ship_c=NULL;
  cv=NULL;
  
  /* 
   *    Del all the lists 
   */
  
  for(i=0;i<GameParametres(GET,GNPLANETS,0)+1;i++){
    DestroyObjList(&listheadcontainer[i]);
    listheadcontainer[i].list=NULL;
    listheadcontainer[i].n=0;
  }
#if DEBUG
  if(debugsave){
    printf("A0 nplayers: %d\n",GameParametres(GET,GNPLAYERS,0));  
  }
#endif
  for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    DestroyObjList(&listheadkplanets[i]);
    listheadkplanets[i].list=NULL;
    listheadkplanets[i].n=0;
    
    ccdata=&ccdatap[i];  
    DestroyCCPlanetInfo(ccdata);
  }
  
  for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    DelIntList(players[i].kplanets);
    players[i].kplanets=NULL;
    DelIntIList(&players[i].ksectors);
  }
  
  free(players);
  MemUsed(MADD,-sizeof(struct Player));
  players=NULL;
  
  
  printf("done\n");
  
  /* Loading the number of objects */
  
  if(fscanf(fp,"%d",&num_objs)!=1){
    perror("fscanf");
    exit(-1);
  }
  printf("\tNumber of objects: %d\n",num_objs);
  
  tbl=malloc(num_objs*sizeof(struct ObjTable));
  if(tbl==NULL){
    fprintf(stderr,"ERROR in malloc ExecLoad()\n");
    exit(-1);
  }
  /* Global variables */
#if DEBUG  
  if(debugsave){
    printf("\tLoading global variables...");
  }
#endif  
  if(fscanf(fp,"%d%d%d",&width,&height,&tmpint)!=3){
    perror("fscanf");
    exit(-1);
  }
  GameParametres(SET,GNET,tmpint);
  
  if(fscanf(fp,"%d",&tmpint)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  GameParametres(SET,GNGALAXIES,tmpint);
  
  if(fscanf(fp,"%d",&tmpint)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  GameParametres(SET,GNPLAYERS,tmpint);
  printf("number of players: %d\n",tmpint);
  
  if(fscanf(fp,"%d",&tmpint)!=1){
    perror("fscanf");
    exit(-1);
  }
  GameParametres(SET,GNPLANETS,tmpint);
  
  if(fscanf(fp,"%d",&tmpint)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  GameParametres(SET,GKPLANETS,tmpint);
  
  if(fscanf(fp,"%d",&tmpint)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  GameParametres(SET,GPIRATES,tmpint);
  
  if(fscanf(fp,"%d",&tmpint)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  GameParametres(SET,GENEMYKNOWN,tmpint);
  
  if(fscanf(fp,"%d",&tmpint)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  GameParametres(SET,GCOOPERATIVE,tmpint);
  
  if(fscanf(fp,"%d",&tmpint)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  GameParametres(SET,GCOMPCOOPERATIVE,tmpint);
  
  if(fscanf(fp,"%d",&tmpint)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  GameParametres(SET,GQUEEN,tmpint);
  
  players=realloc(players,(GameParametres(GET,GNPLAYERS,0)+2)*sizeof(struct Player));
  if(players==NULL){ 
    fprintf(stderr,"ERROR in realloc Execload(players)\n");
    exit(-1);
  } 
  
  ccdatap=realloc(ccdatap,(GameParametres(GET,GNPLAYERS,0)+2)*sizeof(struct CCDATA));
  if(ccdatap==NULL){ 
    fprintf(stderr,"ERROR in realloc Execload(ccdatap)\n");
    exit(-1);
  } 
  
  for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    ccdatap[i].player=i;
    ccdatap[i].planetinfo=NULL;
    ccdatap[i].nkplanets=0;
    ccdatap[i].nplanets=0;
    ccdatap[i].time=0;
    ccdatap[i].ninexplore=0;
    ccdatap[i].nenemy=0;
    
    ccdatap[i].nexplorer=0;
    ccdatap[i].nfighter=0;
    ccdatap[i].ntower=0;
    ccdatap[i].ncargo=0;
    ccdatap[i].npilot=0;
    ccdatap[i].pilot=NULL;
    
    ccdatap[i].sw=0;
    ccdatap[i].war=0;
    ccdatap[i].p2a_strength=0;
    
    ccdatap[i].planethighresource=NULL;
    ccdatap[i].planetweak=NULL;
    ccdatap[i].planethighlevel=NULL;
    ccdatap[i].planet2meet=NULL;
    ccdatap[i].planet2attack=NULL;
  }
  
  planet2meetid=malloc((GameParametres(GET,GNPLAYERS,0)+2)*sizeof(int));
  if(planet2meetid==NULL){ 
    fprintf(stderr,"ERROR in malloc Execload(planet2meetid)\n");
    exit(-1);
  } 
  
  planet2attackid=malloc((GameParametres(GET,GNPLAYERS,0)+2)*sizeof(int));
  if(planet2attackid==NULL){ 
    fprintf(stderr,"ERROR in malloc Execload(planet2attackid)\n");
    exit(-1);
  } 
  
  listheadkplanets=realloc(listheadkplanets,(GameParametres(GET,GNPLAYERS,0)+2)*sizeof(struct HeadObjList));
  if(listheadkplanets==NULL){
    fprintf(stderr,"ERROR in realloc Execload(listheadkplanets)\n");
    exit(-1);
  }
  for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    listheadkplanets[i].list=NULL;
    listheadkplanets[i].n=0;
  }
  
  listheadcontainer=realloc(listheadcontainer,(GameParametres(GET,GNPLANETS,0)+1)*sizeof(struct HeadObjList));
  if(listheadcontainer==NULL){
    fprintf(stderr,"ERROR in realloc ExecLoad(listheadcontainer)\n");
    exit(-1);
  }
  for(i=0;i<GameParametres(GET,GNPLANETS,0)+1;i++){
    listheadcontainer[i].list=NULL;
    listheadcontainer[i].n=0;
  }
  
  if(fscanf(fp,"%d%d%d%d%d%d%d%d",
	    &ulx,&uly,
	    &actual_player0,&record,&nav_mode,&gtime,
	    &g_objid,&g_projid)!=8){
    perror("fscanf");
    exit(-1);
  }
  
  GameParametres(SET,GULX,ulx);
  GameParametres(SET,GULY,uly);
  
  
  SetTime(gtime);
  
  if(fscanf(fp,"%d%d\n",&sc,&sv)!=2){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%d%d\n",&habitat_type,&habitat_obj)!=2){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%d%d%d%d%d%d%d%d%d%d%d",
	    &glocal.actual_player,
	    &glocal.g_objid,
	    &glocal.g_projid,
	    &glocal.ship_c,
	    &glocal.cv,
	    &glocal.habitat_type,
	    &glocal.habitat_id,
	    &fobj[0],
	    &fobj[1],
	    &fobj[2],
	    &fobj[3])!=11){
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%d%d%d%d%d%d%d%d%d%d%d",
	    &gremote.actual_player,
	    &gremote.g_objid,
	    &gremote.g_projid,
	    &gremote.ship_c,
	    &gremote.cv,
	    &gremote.habitat_type,
	    &gremote.habitat_id,
	    &gremote.fobj[0],
	    &gremote.fobj[1],
	    &gremote.fobj[2],
	    &gremote.fobj[3])!=11){
    perror("fscanf");
    exit(-1);
  }
#if DEBUG  
  if(debugsave){
    printf("gid:%d\n",g_objid);
    printf("gid loc:%d\n",glocal.g_objid);
    printf("gid rem:%d\n",gremote.g_objid);
  }
#endif
  if(GameParametres(GET,GMODE,0)==CLIENT){
    actual_player=actual_player0=gremote.actual_player;
    g_objid=gremote.g_objid;
    g_projid=gremote.g_projid;
    habitat.type=gremote.habitat_type;
    for(i=0;i<4;i++){
      fobj[i]=gremote.fobj[i];
    }
  }
  
  if(GameParametres(GET,GMODE,0)==SERVER){
    printf("actplayer: %d actplayer0: %d glocal: %d\n",actual_player,actual_player0,glocal.actual_player);
    actual_player=actual_player0=glocal.actual_player;
  }
  
  for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    if(fscanf(fp,"%128s%hd%d%d%hd%hd%hd%hd%hd%hd%hd%d%d%d%d%d%f%d%d%d%d%d%d%f%d%d",
    	      players[i].playername,
	      &players[i].id,
	      &players[i].status,
	      &players[i].pid,
	      &players[i].proc,
	      &players[i].control,
	      &players[i].team,
	      &players[i].profile,
	      &players[i].strategy,
	      &players[i].gmaxlevel,
	      &players[i].maxlevel,
	      &players[i].color,
	      &players[i].cv,
	      &players[i].nplanets,
	      &players[i].nships,
	      &players[i].nbuildships,
	      &players[i].gold,
	      &players[i].lastaction,
	      &players[i].ndeaths,
	      &players[i].nkills,
	      &players[i].points,
	      &players[i].goldships,
	      &players[i].goldupdates,
	      &players[i].goldweapon,
	      &nkp,&nks)!=26){
      perror("fscanf");
      exit(-1);
    }
    
    players[i].modified=SENDOBJUNMOD;
    players[i].ttl=2000;
    
    /* building known sectors and known planets list */
    
    players[i].kplanets=NULL;
    
    players[i].ksectors.n=0;
    players[i].ksectors.n0=0;
    players[i].ksectors.list=NULL;
    for(j=0;j<NINDEXILIST;j++){
      players[i].ksectors.index[j]=NULL;
    }
    
    if(nkp!=0){
      players[i].nplanets=0;
      for(j=0;j<nkp;j++){
	if(fscanf(fp,"%d",&id)!=1){
	  perror("fscanf");
	  exit(-1);
	}
	players[i].kplanets=Add2IntList((players[i].kplanets),id);
	players[i].nplanets++;
#if DEBUG
	if(debugsave){
	  printf("adding planet:(%d) %d \n",i,id);
	}
#endif
      }
      if(CountIntList(players[i].kplanets)!=nkp){
	fprintf(stderr,"ERROR in load planets (%d) %d %d\n",i,nkp,CountIntList(players[i].kplanets));
	exit(-1);
      }
    }
    if(nks!=0){
      for(j=0;j<nks;j++){
	if(fscanf(fp,"%d",&id)!=1){
	  perror("fscanf");
	  exit(-1);
	}
	if(GameParametres(GET,GKPLANETS,0)==FALSE)
	  Add2IntIList(&(players[i].ksectors),id);
      }
    }
    
    /* --building known sectors and known planets list */
    
    
  }  /* for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++) */
  
  
  printf("done\n");
  
  /* Loading the  objects */
  
  obj.weapon=&obj.weapon0;
  
  printf("\tLoading the objects...");
  for(i=0;i<num_objs;i++){
    FscanfObj(fp,&obj,&tbl[i]);
    id=g_objid;
    projid=g_projid;
    nobj=NewObj(SHIP,SHIP0,
		obj.x,obj.y,obj.vx,obj.vy,
		CANNON0,ENGINE0,0,NULL,NULL); /* HEREIN */
    if(nobj==NULL){
      fprintf(stderr,"\nERROR en ExecLoad(): NewObj() devuelve NULL\n");
      exit(-1);
    }
    g_objid=id;
    g_projid=projid;
    
    data=nobj->cdata;
    CopyObject(nobj,&obj);
    if(nobj->type!=SHIP){
      free(data);
      MemUsed(MADD,-sizeof(Data));
      data=NULL;
    }
    nobj->cdata=data;
    
    nobj->weapon=NULL;
    if(obj.weapon==&obj.weapon0)nobj->weapon=&nobj->weapon0;
    if(obj.weapon==&obj.weapon1)nobj->weapon=&nobj->weapon1;
    if(obj.weapon==&obj.weapon2)nobj->weapon=&nobj->weapon2;
    
    nobj->norder=0;
    nobj->parent=NULL;
    nobj->dest=NULL;
    nobj->in=NULL;
    nobj->planet=NULL;
    nobj->lorder=NULL;
    /* nobj->data=NULL; */
    
    if(nobj->type==PLANET){
      nplanet=malloc(sizeof(struct Planet));
      MemUsed(MADD,sizeof(struct Planet));
      if(nplanet==NULL){
	fprintf(stderr,"ERROR in malloc ExecLoad()\n");
	exit(-1);
      }
      nplanet->segment=NULL;
      FscanfPlanet(fp,nplanet);
      nobj->planet=nplanet;
    }
    Add2ObjList_E(&listheadobjs,nobj);
    FscanfOrders(fp,nobj);
   
    if(fscanf(fp,"%d%d%d", 
	      &nobj->cargo.capacity, 
	      &nobj->cargo.mass, 
	      &nobj->cargo.n)!=3){
      perror("fscanf");
      exit(-1);
    } 

    nobj->cargo.mass=0; 
    nobj->cargo.n=0; 
    nobj->cargo.hlist=NULL;
 

  }  /*for(i=0;i<num_objs;i++){    */


  /*HERE TODO cargo list*/
  {
    int n;
    for(i=0;i<num_objs;i++){

      if(fscanf(fp,"%d%d",&id,&n)!=2){
	perror("fscanf");
	exit(-1);
      }  
      obj0=SelectObj(&listheadobjs,id);

      for(j=0;j<n;j++){
	if(fscanf(fp,"%d",&id)!=1){
	  perror("fscanf");
	  exit(-1);
	}
	obj1=SelectObj(&listheadobjs,id);
	/* TODO add obj1 to obj0 list */
	printf("cargoadd %d:\n",obj0->cargo.n);
	CargoAdd(obj0,obj1);
      }
    }
  }
  /* --cargo list*/

  
  /* --Loading the  objects */
  
  
  /* load ccdata*/
  
  for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    ccdata=&ccdatap[i];
    FscanfCCData(fp,ccdata);
  }
  
  /* --load ccdata*/
  
  /* statistics */
  fscanfStatistics(fp);
  
  /* --statistics */
  
  
  
  if(fscanf(fp,"%f",&control2)!=1){
    perror("fscanf");
    exit(-1);
  }
  
  /* printf("control2: %f ...",control2); */
  if(control!=control2){
    fprintf(stderr,"ERROR Execload(). Error in save file. May be corrupted.\n");
    exit(-1);
  }
  else{
    printf("OK\n");
  }
  printf("... done\n");
  
  
  
  /****CELLON ****/
  nx=GameParametres(GET,GULX,0)/2000;
  ny=GameParametres(GET,GULY,0)/2000;
  
  cell=realloc(cell,nx*ny*sizeof(int));
  if(cell==NULL){ 
    fprintf(stderr,"ERROR in malloc (creating cell)\n"); 
    exit(-1); 
  }
  for(i=0;i<nx*ny;i++){
    cell[i]=0;
  }
  
  /****CELLON****/
  
  
  /* 
   *   Asignation of pointers
   */
  
  for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    /*      printf("player cv: %d %d",i,players[i].global.cv); */
    /*      printf("\n"); */
  }
  
  cv=NULL;
  ship_c=NULL;
  
  if(GameParametres(GET,GMODE,0)==CLIENT){
    ship_c=SelectObj(&listheadobjs,gremote.ship_c);
    cv=SelectObj(&listheadobjs,gremote.cv);
  }
  else{
    ship_c=SelectObj(&listheadobjs,glocal.ship_c);
    cv=SelectObj(&listheadobjs,glocal.cv);
  }
#if DEBUG
  if(debugsave){
    if(cv!=NULL){
      printf("cv: %d\n",cv->id);
    }  
    else{
      printf("cv: NULL\n");
    }
  }
#endif
  ls=listheadobjs.list;
  i=0;
  while(ls!=NULL){
    obj0=ls->obj;
    
    obj0->parent=NULL;
    obj0->dest=NULL;
    obj0->in=NULL;
    
    if(obj0->type!=PLANET){
      obj0->planet=NULL;
      
      if(tbl[i].parent != 0){
	obj0->parent=SelectObj(&listheadobjs,tbl[i].parent);
      }
      if(tbl[i].dest != 0){
	obj0->dest=SelectObj(&listheadobjs,tbl[i].dest);
      }
      if(tbl[i].in != 0){
	obj0->in=SelectObj(&listheadobjs,tbl[i].in);
	if(obj0->in==NULL){
	  fprintf(stderr,"Error ExecLoad(): asignation of in\n");
	}
      }
      if(tbl[i].planet != 0){
	obj1=SelectObj(&listheadobjs,tbl[i].planet);
	if(obj1 != NULL){
	  obj0->planet=obj1->planet;
	  printf("assigning planet\n");
	}
      }
    }
    i++;    
    ls=ls->next;
  }
  
  global=&glocal;
  if(GameParametres(GET,GMODE,0)==CLIENT){
    global=&gremote;
  }
  
  /*  habitat.type=habitat_type; */
  habitat.obj=NULL;
  /*  habitat.planet=NULL; */
  
  habitat.type=global->habitat_type;
  
  if(global->habitat_id!=0){
    habitat.obj=SelectObj(&listheadobjs,global->habitat_id);
    if(habitat.obj==NULL){
      fprintf(stderr,"ERROR ExecLoad()\n");
      fprintf(stderr,"\t id habitat_obj: %d\n",habitat_obj);
      /*      exit(-1); */
    }
  }
  if(cv!=NULL){
    habitat.type=cv->habitat;
    habitat.obj=cv->in;
    cv->selected=TRUE;
    /*    habitat.planet=cv->planet; */
  }
  
  free(tbl);
  tbl=NULL;
  
  fclose(fp);
  /*  Check(); */
  
  /* building lists and data */
  
  CreateContainerLists(&listheadobjs,listheadcontainer);
  CreatekplanetsLists(&listheadobjs,listheadkplanets);
  
  /* building ccdata and planet info */
  /* HERE not same state that before saving */
  for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    if(players[i].control==COMPUTER && GetProc()==players[i].proc){
      ccdata=&ccdatap[i];  
      CalcCCInfo(&listheadobjs,&listheadkplanets[players[i].id],players[i].id,ccdata);
      CalcCCPlanetStrength(players[i].id,ccdata);
    }
  }
  /* --building ccdata and planet info */
  
  
  /* --building lists and data */
#if DEBUG
  if(debugsave){  
    printf("LAST IN execload() gid:%d\n",g_objid);
    printf("gid loc:%d\n",glocal.g_objid);
    printf("gid rem:%d\n",gremote.g_objid);
  }
#endif
  free(planet2meetid);
  planet2meetid=NULL;
  free(planet2attackid);
  planet2attackid=NULL;
  
  return(0);
} /* --ExecLoad() */

int FprintfObj(FILE *fp,Object *obj){
  /*
    Write an object to the file *fp
  */
  
  int in,dest,parent,weapon;
  short modified;
  int ttl;
  
  modified=SENDOBJUNMOD;
  ttl=0;
  /* positions saved normalized */
  
  fprintf(fp,"%d %d %s %hd %hd %hd %hd %g %d %d %d %d %d %d %d %d %g %d %hd %hd %hd %hd %hd ",
	  obj->id,obj->pid,obj->name,obj->player,obj->type,
	  obj->subtype,obj->level,obj->experience,obj->kills,
	  obj->durable,obj->visible,obj->radar,obj->mass,
	  obj->items,obj->cargo.capacity,obj->radio,obj->cost,obj->damage,
	  obj->ai,modified,ttl,obj->habitat,obj->mode);
  
  
  fprintf(fp,"%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g ",
	  obj->x,obj->y,
	  obj->x0,obj->y0,
	  obj->vx,obj->vy,obj->fx,obj->fy,
	  obj->fx0,obj->fy0,obj->a,obj->ang_v,
	  obj->ang_a,obj->accel,obj->gas,obj->gas_max,
	  obj->life,obj->shield,obj->state);
  
  fprintf(fp,"%g %d %d %d ",
	  obj->dest_r2,obj->sw,obj->trace,obj->norder);
  
  fprintf(fp,"%d %d %d %d ",
	  obj->actorder.priority,obj->actorder.id,
	  obj->actorder.time,obj->actorder.g_time);
  fprintf(fp,"%f %f %f %f %f %f %f %f ",
	  obj->actorder.a,obj->actorder.b,obj->actorder.c,obj->actorder.d,
	  obj->actorder.e,obj->actorder.f,obj->actorder.g,obj->actorder.h);
  
  
  fprintf(fp,"%d %d %d %d %d %g %g %g %g %d ",
	  obj->engine.type,obj->engine.a_max,obj->engine.a,
	  obj->engine.v_max,obj->engine.v2_max,
	  obj->engine.ang_a,obj->engine.ang_a_max,
	  obj->engine.ang_v_max,obj->engine.gascost,
	  obj->engine.mass);
  
  fprintf(fp,"%d %d %d %d %d %d %d ",
	  obj->weapon0.type,
	  obj->weapon0.rate,obj->weapon0.nshots,
	  obj->weapon0.cont1,obj->weapon0.mass,
	  obj->weapon0.n,obj->weapon0.max_n);
  
  fprintf(fp,"%d %d %d %d %d %d %d %f ",
	  obj->weapon0.projectile.type,
	  obj->weapon0.projectile.durable,obj->weapon0.projectile.life,
	  obj->weapon0.projectile.damage,obj->weapon0.projectile.max_vel,
	  obj->weapon0.projectile.mass,obj->weapon0.projectile.gascost,
	  obj->weapon0.projectile.unitcost);
  
  fprintf(fp,"%d %d %d %d %d %d %d ",
	  obj->weapon1.type,
	  obj->weapon1.rate,obj->weapon1.nshots,
	  obj->weapon1.cont1,obj->weapon1.mass,
	  obj->weapon1.n,obj->weapon1.max_n);
  
  fprintf(fp,"%d %d %d %d %d %d %d %f ",
	  obj->weapon1.projectile.type,
	  obj->weapon1.projectile.durable,obj->weapon1.projectile.life,
	  obj->weapon1.projectile.damage,obj->weapon1.projectile.max_vel,
	  obj->weapon1.projectile.mass,obj->weapon1.projectile.gascost,
	  obj->weapon1.projectile.unitcost);
  
  fprintf(fp,"%d %d %d %d %d %d %d ",
	  obj->weapon2.type,
	  obj->weapon2.rate,obj->weapon2.nshots,
	  obj->weapon2.cont1,obj->weapon2.mass,
	  obj->weapon2.n,obj->weapon2.max_n);
  fprintf(fp,"%d %d %d %d %d %d %d %f ",
	  obj->weapon2.projectile.type,
	  obj->weapon2.projectile.durable,obj->weapon2.projectile.life,
	  obj->weapon2.projectile.damage,obj->weapon2.projectile.max_vel,
	  obj->weapon2.projectile.mass,obj->weapon2.projectile.gascost,
	  obj->weapon2.projectile.unitcost);
  
  weapon=-1;
  if(obj->weapon==&obj->weapon0)weapon=0;
  if(obj->weapon==&obj->weapon1)weapon=1;
  if(obj->weapon==&obj->weapon2)weapon=2;
  fprintf(fp,"%d ",weapon);
  
  parent=dest=in=0;
  
  if(obj->parent!=NULL)parent=obj->parent->id;
  if(obj->dest!=NULL)dest=obj->dest->id;
  if(obj->in!=NULL)in=obj->in->id;
  
  fprintf(fp,"%d %d %d ",parent,dest,in);
  
  if(obj->type==PLANET){
    FprintfPlanet(fp,obj);
  }
  FprintfOrders(fp,obj);

  return(0);
}

int FprintfPlanet(FILE *fp,Object *obj){
  /*
    Write an planet to the file *fp
  */
  
  struct Planet *planet;
  Segment *s;
  int n;
  
  planet=obj->planet;
  
  fprintf(fp,"%d %d %g %g %g ",
	  planet->x,planet->y,
	  planet->r,planet->reggold,planet->gold);
  
  s=planet->segment;
  n=0;
  while(s!=NULL){
    n++;
    s=s->next;
  }
  fprintf(fp,"%d ",n);
  
  s=planet->segment;
  while(s!=NULL){
    fprintf(fp,"%g %g %g %g %d ",  /* normalized planet */
	    (float)s->x0,(float)s->y0,(float)s->x1,(float)s->y1,s->type);
    s=s->next;
  }
  return(0);
}

int FscanfObj(FILE *fp,Object *obj,struct ObjTable *tbl){
  /*
    Read an object from the file *fp
  */
  int in,dest,parent,weapon;
  short modified;  
  
  if(fscanf(fp,"%d%d%16s%hd%hd%hd%hd%f%d%d%d%d%d%d%d%d%f%d%hd%hd%hd%hd%hd",
	    &obj->id,&obj->pid,obj->name,&obj->player,&obj->type,
	    &obj->subtype,&obj->level,&obj->experience,&obj->kills,
	    &obj->durable,&obj->visible,&obj->radar,&obj->mass,
	    &obj->items,&obj->cargo.capacity,&obj->radio,&obj->cost,&obj->damage,
	    &obj->ai,&modified,&obj->ttl,&obj->habitat,&obj->mode)!=23){
    perror("fscanf");
    exit(-1);
  }
  obj->pexperience=0;
  obj->selected=FALSE;
  obj->modified=SENDOBJUNMOD;
  if(fscanf(fp,"%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f",
	    &obj->x,&obj->y,&obj->x0,&obj->y0,
	    &obj->vx,&obj->vy,&obj->fx,&obj->fy,
	    &obj->fx0,&obj->fy0,&obj->a,&obj->ang_v,
	    &obj->ang_a,&obj->accel,&obj->gas,&obj->gas_max,
	    &obj->life,&obj->shield,&obj->state)!=19){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%f%d%hd%d",
	    &obj->dest_r2,&obj->sw,&obj->trace,&obj->norder)!=4){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%d%d%d%d",
	    &obj->actorder.priority,&obj->actorder.id,
	    &obj->actorder.time,&obj->actorder.g_time)!=4){
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%f%f%f%f%f%f%f%f",
	    &obj->actorder.a,&obj->actorder.b,&obj->actorder.c,&obj->actorder.d,
	    &obj->actorder.e,&obj->actorder.f,&obj->actorder.g,&obj->actorder.h)!=8){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%d%d%d%d%d%f%f%f%f%d",
	    &obj->engine.type,&obj->engine.a_max,&obj->engine.a,
	    &obj->engine.v_max,&obj->engine.v2_max,
	    &obj->engine.ang_a,&obj->engine.ang_a_max,
	    &obj->engine.ang_v_max,&obj->engine.gascost,
	    &obj->engine.mass)!=10){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%d%d%d%d%d%d%d",
	    &obj->weapon0.type,
	    &obj->weapon0.rate,&obj->weapon0.nshots,
	    &obj->weapon0.cont1,&obj->weapon0.mass,
	    &obj->weapon0.n,&obj->weapon0.max_n)!=7){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%d%d%d%d%d%d%d%f",
	    &obj->weapon0.projectile.type,
	    &obj->weapon0.projectile.durable,&obj->weapon0.projectile.life,
	    &obj->weapon0.projectile.damage,&obj->weapon0.projectile.max_vel,
	    &obj->weapon0.projectile.mass,&obj->weapon0.projectile.gascost,
	    &obj->weapon0.projectile.unitcost)!=8){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%d%d%d%d%d%d%d",
	    &obj->weapon1.type,
	    &obj->weapon1.rate,&obj->weapon1.nshots,
	    &obj->weapon1.cont1,&obj->weapon1.mass,
	    &obj->weapon1.n,&obj->weapon1.max_n)!=7){
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%d%d%d%d%d%d%d%f",
	    &obj->weapon1.projectile.type,
	    &obj->weapon1.projectile.durable,&obj->weapon1.projectile.life,
	    &obj->weapon1.projectile.damage,&obj->weapon1.projectile.max_vel,
	    &obj->weapon1.projectile.mass,&obj->weapon1.projectile.gascost,
	    &obj->weapon1.projectile.unitcost)!=8){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%d%d%d%d%d%d%d",
	    &obj->weapon2.type,
	    &obj->weapon2.rate,&obj->weapon2.nshots,
	    &obj->weapon2.cont1,&obj->weapon2.mass,
	    &obj->weapon2.n,&obj->weapon2.max_n)!=7){
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%d%d%d%d%d%d%d%f",
	    &obj->weapon2.projectile.type,
	    &obj->weapon2.projectile.durable,&obj->weapon2.projectile.life,
	    &obj->weapon2.projectile.damage,&obj->weapon2.projectile.max_vel,
	    &obj->weapon2.projectile.mass,&obj->weapon2.projectile.gascost,
	    &obj->weapon2.projectile.unitcost)!=8){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%d",&weapon)!=1){
    perror("fscanf");
    exit(-1);
  }
  
  obj->weapon=NULL;
  if(weapon==0)obj->weapon=&obj->weapon0;
  if(weapon==1)obj->weapon=&obj->weapon1;
  if(weapon==2)obj->weapon=&obj->weapon2;
  
  if(fscanf(fp,"%d%d%d",
	    &parent,&dest,&in)!=3){
    perror("fscanf");
    exit(-1);
  }
  
  
  tbl->id=obj->id;
  tbl->parent=parent;
  tbl->dest=dest;
  tbl->in=in;
  tbl->planet=0;
  
  return(0);
}


int FscanfPlanet(FILE *fp,struct Planet *planet){
  /*
    Scanf a planet from the file *fp
  */
  Segment *s;
  float x0,x1,y0,y1;
  int i,n;
  
  s=NULL;
  
  if(fscanf(fp,"%d%d%f%f%f",
	    &planet->x,&planet->y,
	    &planet->r,
	    &planet->reggold,&planet->gold)!=5){
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%d",&n)!=1){
    perror("fscanf");
    exit(-1);
  }
  
  planet->segment=malloc(sizeof(Segment));
  MemUsed(MADD,sizeof(Segment));
  if(planet->segment==NULL){
    fprintf(stderr,"ERROR in malloc CreatePlanet()2\n");
    exit(-1);
  }
  
  s=planet->segment;
  
  for(i=0;i<n-1;i++){
    
    if(fscanf(fp,"%f%f%f%f%d",
	      &x0,&y0,&x1,&y1,&s->type)!=5){
      perror("fscanf");
      exit(-1);
    }
    
    s->x0=x0;
    s->x1=x1;
    s->y0=y0;
    s->y1=y1;
    
    s->next=malloc(sizeof(Segment));
    MemUsed(MADD,sizeof(Segment));
    if(s->next==NULL){
      fprintf(stderr,"ERROR in malloc CreatePlanet()2\n");
      exit(-1);
    }
    s=s->next;
  }
  if(fscanf(fp,"%f%f%f%f%d",
	    &x0,&y0,&x1,&y1,&s->type)!=5){
    perror("fscanf");
    exit(-1);
  }
  s->x0=x0;
  s->x1=x1;
  s->y0=y0;
  s->y1=y1;
  
  s->next=NULL;
  
  return(0);
}

int FprintfOrders(FILE *fp,Object *obj){
  /*
    Print the orders of the object obj to the file *fp
    return:
    the number of orders printed
  */
  
  int i;
  struct ListOrder *lo;
  struct Order *ord;
  int n,m;
  
  /*printf number of orders; */
  n=CountOrders(obj);
  m=obj->norder;
  if(m!=n){
    fprintf(stderr,"WARNING FprintfOrders(): number of orders don't match norder\n" );
    printf("\tnor: %d norder: %d \n",n,obj->norder);
  }
  fprintf(fp,"O ");
  
  fprintf(fp,"%d ",n);
  
  /* printf actorder , don't below to the list ?? check this */
  /*  printf("\n(%d) norder: %d n:%d\n",obj->id,obj->norder,n); */
  
  lo=obj->lorder;
  i=0;
  while(lo!=NULL){
    ord=&(lo->order);
    fprintf(fp,"%d %d %d %d ",
	    ord->priority,ord->id,ord->time,ord->g_time);
    fprintf(fp,"%g %g %g %g %g %g %g %g %g %g %g %g ",
	    ord->a,ord->b,ord->c,ord->d,
	    ord->e,ord->f,ord->g,ord->h,
	    ord->i,ord->j,ord->k,ord->l);
    i++;
    lo=lo->next;
  }
  if(i!=n){
    fprintf(stderr,"ERROR FprintfOrders(): number of orders don't match norder\n" );
    fprintf(stderr,"\tnor: %d norder: %d \n",i,n);
    exit(-1);
  }
  return(i);
}


int FscanfOrders(FILE *fp,Object *obj){
  /*
    Read the orders of the object obj from the file *fp
  */
  int i;
  struct Order order;
  int n;
  char c[10];
  if(fscanf(fp,"%10s",c)!=1){
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%d",&n)!=1){  /* number of orders */
    perror("fscanf");
    exit(-1);
  }
  
  /*  printf("\n(%d) norder: %d n:%d\n",obj->id,obj->norder,n); */
  
  obj->lorder=NULL;
  obj->norder=0;
  for(i=0;i<n;i++){
    if(fscanf(fp,"%d%d%d%d",
	      &order.priority,&order.id,&order.time,&order.g_time)!=4){
      perror("fscanf");
      exit(-1);
    }
    if(fscanf(fp,"%f%f%f%f%f%f%f%f%f%f%f%f",
	      &order.a,&order.b,&order.c,&order.d,
	      &order.e,&order.f,&order.g,&order.h,
	      &order.i,&order.j,&order.k,&order.l)!=12){
      perror("fscanf");
      exit(-1);
    }
    AddOrder(obj,&order);
  }
  return(n);
}


void FprintfCCData(FILE *fp,struct CCDATA *ccdata){
  
  fprintf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %f\n",
	  ccdata->player,ccdata->time,ccdata->time2,ccdata->nkplanets,ccdata->nplanets,
	  ccdata->ninexplore,ccdata->nenemy,
	  ccdata->nexplorer,ccdata->nfighter,ccdata->npilot,ccdata->ntower,ccdata->ncargo,
	  ccdata->sw,ccdata->war,ccdata->p2a_strength);
  
  if(ccdata->planethighresource!=NULL){
    fprintf(fp,"%d ",ccdata->planethighresource->id);
  }
  else{
    fprintf(fp,"%d ",0);
  }
  if(ccdata->planetweak!=NULL){
    fprintf(fp,"%d ",ccdata->planetweak->id);
  }
  else{
    fprintf(fp,"%d ",0);
  }
  if(ccdata->planethighlevel!=NULL){
    fprintf(fp,"%d ",ccdata->planethighlevel->id);
  }
  else{
    fprintf(fp,"%d ",0);
  }
  if(ccdata->planet2meet!=NULL){
    fprintf(fp,"%d ",ccdata->planet2meet->id);
  }
  else{
    fprintf(fp,"%d ",0);
  }
  if(ccdata->planet2attack!=NULL){
    fprintf(fp,"%d ",ccdata->planet2attack->id);
  }
  else{
    fprintf(fp,"%d ",0);
  }
  FprintfPlanetInfoList(fp,ccdata);
}


void FprintfPlanetInfoList(FILE *fp,struct CCDATA *ccdata){
  struct PlanetInfo *pinfo;
  int n=0;  
  
  n=CountPlanetInfoList(ccdata);
  fprintf(fp,"%d ",n);
  
  if(n>0){
    pinfo=ccdata->planetinfo;
    while(pinfo!=NULL){
      FprintfPlanetInfo(fp,pinfo);
      pinfo=pinfo->next;
    }
  }
}

void FprintfPlanetInfo(FILE *fp,struct PlanetInfo *pinfo){
  int objid;
  
  if(pinfo->planet!=NULL){
    objid=pinfo->planet->id;
  }
  else{
    objid=0;
  }
#if DEBUG
  if(debugsave){
    printf("saving info of planet %d\n",objid);
  }
#endif
  fprintf(fp,"%d %d %d %d %d %d %d %f %f %d\n",
	  objid,pinfo->time,
	  pinfo->nexplorer,pinfo->nfighter,pinfo->npilot,pinfo->ntower,pinfo->ncargo,
	  pinfo->strength,pinfo->strengtha,
	  pinfo->nassigned);
}

void FscanfCCData(FILE *fp,struct CCDATA *ccdata){
  
  int pld,pw,phl,p2m,p2a;
  
  
  if(fscanf(fp,"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%f",
	    &ccdata->player,&ccdata->time,&ccdata->time2,&ccdata->nkplanets,&ccdata->nplanets,
	    &ccdata->ninexplore,&ccdata->nenemy,
	    &ccdata->nexplorer,&ccdata->nfighter,&ccdata->npilot,&ccdata->ntower,&ccdata->ncargo,
	    &ccdata->sw,&ccdata->war,&ccdata->p2a_strength)!=15){
    perror("fscanf");
    exit(-1);
  }
  ccdata->pilot=NULL;
  
  if(fscanf(fp,"%d%d%d%d%d",&pld,&pw,&phl,&p2m,&p2a)!=5){
    perror("fscanf");
    exit(-1);
  } 
#if DEBUG  
  if(debugsave){
    printf("player: %d\n",ccdata->player);
    printf("\tlow: %d\n",pld);
    printf("\tweak: %d\n",pw);
    printf("\thlevel: %d\n",phl);
    printf("\tmeet: %d\n",p2m);
    printf("\tattack: %d\n",p2a);
  }
#endif
  
  ccdata->time=0;
  
  /***** pointers *****/
  
  ccdata->planethighresource=NULL;
  ccdata->planetweak=NULL;
  ccdata->planethighlevel=NULL;
  ccdata->planet2meet=NULL;
  ccdata->planet2attack=NULL;
  
  if(pld!=0){
    ccdata->planethighresource=SelectObj(&listheadobjs,pld);
  }
  if(pw!=0){
    ccdata->planetweak=SelectObj(&listheadobjs,pw);
  }
  if(phl!=0){
    ccdata->planethighlevel=SelectObj(&listheadobjs,phl);
  }
  if(p2m!=0){
    ccdata->planet2meet=SelectObj(&listheadobjs,p2m);
  }
  if(p2a!=0){
    ccdata->planet2attack=SelectObj(&listheadobjs,p2a);
  }
#if DEBUG
  if(debugsave){
    if(ccdata->planetinfo!=NULL){
      fprintf(stderr,"Error in fscanfccdata(): planetinfo not NULL\n");
    }
  }
#endif
  ccdata->planetinfo=NULL;
  
  FscanfPlanetInfoList(fp,ccdata);
  
  return;
}

void FscanfPlanetInfoList(FILE *fp,struct CCDATA *ccdata){
  struct PlanetInfo pinfo;
  int i;
  int n=0;  
  
  n=CountPlanetInfoList(ccdata);
  if(fscanf(fp,"%d",&n)!=1){
    perror("fscanf");
    exit(-1);
  } 
  
  for(i=0;i<n;i++){
    FscanfPlanetInfo(fp,&pinfo);
    /* HERE create list and assign pointer */
    AddPlanetInfo2CCData(ccdata,&pinfo);
  }
  
}

void FscanfPlanetInfo(FILE *fp,struct PlanetInfo *pinfo){
  int objid;
  Object *planet;
  if(fscanf(fp,"%d%d%d%d%d%d%d%f%f%d",    /* HEREBUG FIXED*/
	    &objid,&pinfo->time,
	    &pinfo->nexplorer,&pinfo->nfighter,&pinfo->npilot,&pinfo->ntower,&pinfo->ncargo,
	    &pinfo->strength,&pinfo->strengtha,
	    &pinfo->nassigned)!=10){
    perror("fscanf");
    exit(-1);
  } 
  planet=SelectObj(&listheadobjs,objid);
  if(planet==NULL){
    fprintf(stderr,"ERROR FscanfPlanetInfo(): planet %d not found\n",objid);
    exit(-1);
  }
  pinfo->planet=planet;
}

void SaveParamOptions(char *file,struct Parametres *par){
  
  FILE *fp;
  
  if((fp=fopen(file,"wt"))==NULL){
    fprintf(stdout,"Can't open the file: %s",file);
    exit(-1);
  }
  printf("saving options to file: %s\n",file);
  fprintf(fp,"%s\n",version);
  fprintf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
	  par->ngalaxies,par->nplanets, par->nplayers, par->ul,par->kplanets,
	  par->sound,par->music,par->soundvol,par->musicvol,
	  par->cooperative,par->compcooperative,par->queen,
	  par->pirates,par->enemyknown,par->port);
  
  if(strlen(par->IP)==0)strncpy(par->IP,DEFAULT_IP,MAXTEXTLEN);
  fprintf(fp,"%s\n",par->IP);
  if(strlen(par->playername)==0)strcpy(par->playername,"player");
  fprintf(fp,"%s\n",par->playername);
  if(strlen(par->font)==0)strcpy(par->font,"6x13");
  fprintf(fp,"%s\n",par->font);
  if(strlen(par->geom)==0)snprintf(par->geom,MAXTEXTLEN,"%dx%d",DEFAULTWIDTH,DEFAULTHEIGHT);
  fprintf(fp,"%s\n",par->geom);
  fprintf(fp,"%s\n",par->lang);
  fclose(fp);
} 


int LoadParamOptions(char *file,struct Parametres *par){
  
  FILE *fp; 
  char optionsversion[MAXTEXTLEN]="";
  
  if((fp=fopen(file,"rt"))==NULL){
    fprintf(stdout,"Can't open the file: %s",file);
    exit(-1);
  }
  
  if(fscanf(fp,"%64s",optionsversion)!=1){
    perror("fscanf");
    exit(-1);
  }
  /* HERE check version */
  
  if(strcmp(optionsversion,MINOROPTIONSVERSION)>=0){
    printf("Version:  game:(%s)  options file:(%s) >= %s  ... OK\n",version,optionsversion,MINOROPTIONSVERSION);
  }
  else if(strcmp(optionsversion,MINOROPTIONSVERSION)<0){
    fprintf(stderr,"Error: incompatible versions.\n");
    printf("Version:  game:(%s)  options file:(%s) < %s\n",version,optionsversion,MINOROPTIONSVERSION);
    fclose(fp);
    return(1);
  }
  
  if(fscanf(fp,"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
	    &par->ngalaxies,&par->nplanets, &par->nplayers, &par->ul,&par->kplanets,
	    &par->sound,&par->music,&par->soundvol,&par->musicvol,
	    &par->cooperative,&par->compcooperative,&par->queen,
	    &par->pirates,&par->enemyknown,&par->port)!=15){
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%128s",par->IP)!=1){
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%128s",par->playername)!=1){
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%128s",par->font)!=1){
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%128s",par->geom)!=1){
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%128s",par->lang)!=1){
    perror("fscanf");
    exit(-1);
  }
  fclose(fp);
  return(0);
} 

void PrintParamOptions(struct Parametres *par){
  
  printf("version:\"%s\"\n",version);
  printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
	 par->ngalaxies,par->nplanets, par->nplayers, par->ul,par->kplanets,
	 par->sound,par->music,par->soundvol,par->musicvol,
	 par->cooperative,par->compcooperative,par->queen,
	 par->pirates,par->enemyknown,par->port);
  
  printf("IP:\"%s\"\n",par->IP);
  printf("name:\"%s\"\n",par->playername);
  printf("font:\"%s\"\n",par->font);
  printf("geom:\"%s\"\n",par->geom);
} 


void SaveUserKeys(char *file,struct Keys *keys){
  
  FILE *fp;
  
  if((fp=fopen(file,"wt"))==NULL){
    fprintf(stdout,"Can't open the file: %s",file);
    exit(-1);
  }
  printf("saving keymap to file: %s\n",file);
  fprintf(fp,"%s\n",version);
  fprintf(fp,"fire %ud\n",
	  keys->fire.value);
  fprintf(fp,"turnleft %ud\n",
	  keys->turnleft.value);
  fprintf(fp,"turnright %ud\n",
	  keys->turnright.value);
  fprintf(fp,"accel %ud\n",
	  keys->accel.value);
  fprintf(fp,"automode %ud\n",
	  keys->automode.value);
  fprintf(fp,"manualmode %ud\n",
	  keys->manualmode.value);
  fprintf(fp,"map %ud\n",
	  keys->map.value);
  fprintf(fp,"order %ud\n",
	  keys->order.value);
  
  fclose(fp);
} 

int LoadUserKeys(char *keyfile,struct Keys *keys){
  FILE *fp;
  char cad[MAXTEXTLEN];
  
  if((fp=fopen(keyfile,"rt"))==NULL){
    printf("file does not exist\n");
    
    /* if doesn't exist, create with default values */
    SaveUserKeys(keyfile,keys);
    
  }
  else{
    fclose(fp);
  }
  
  /* Read keys  */
  
  if((fp=fopen(keyfile,"rt"))==NULL){
    fprintf(stdout,"I can't open the file: %s", keyfile);
    exit(-1);
  }
  
  if(fscanf(fp,"%128s",cad)!=1){ /* HERE check version */
    perror("fscanf");
    exit(-1);
  }
  
  if(strcmp(cad,MINOROPTIONSVERSION)>=0){
    printf("Version:  game:(%s)  keymap file:(%s) >= %s  ... OK\n",
	   version,cad,MINOROPTIONSVERSION);
  }
  else if(strcmp(cad,MINOROPTIONSVERSION)<0){
    fprintf(stderr,"Error: incompatible versions.\n");
    printf("Version:  game:(%s)  keymap file:(%s) < %s\n",
	   version,cad,MINOROPTIONSVERSION);
    fclose(fp);
    return(1);
  }
  
  if(fscanf(fp,"%128s",cad)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%ud",&keys->fire.value)!=1){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%128s",cad)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%ud",&keys->turnleft.value)!=1){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%128s",cad)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%ud",&keys->turnright.value)!=1){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%128s",cad)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%ud",&keys->accel.value)!=1){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%128s",cad)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%ud",&keys->automode.value)!=1){
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%128s",cad)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%ud",&keys->manualmode.value)!=1){
    perror("fscanf");
    exit(-1);
  }
  
  if(fscanf(fp,"%128s",cad)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%ud",&keys->map.value)!=1){
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%128s",cad)!=1){ 
    perror("fscanf");
    exit(-1);
  }
  if(fscanf(fp,"%ud",&keys->order.value)!=1){
    perror("fscanf");
    exit(-1);
  }
  
  fclose(fp);
  return(0);
}


void SaveRecord(char *file,struct Player *players,int record){
  int i,j;
  FILE *fp;
  
  j=-1;
  for(i=0;i<GameParametres(GET,GNPLAYERS,0);i++){
    if(players[i].points>=record){
      j=i;
      record=players[i].points;
    }
  }
  if(j!=-1){
    if((fp=fopen(file,"wt"))==NULL){
      fprintf(stdout,"I can't open the file: %s",file);
      exit(-1);
    }
    fprintf(fp,"%d",record);
    fclose(fp);
  }
}

