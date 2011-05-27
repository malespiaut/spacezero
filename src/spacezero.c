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

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include "general.h"
#include "spacezero.h"
#include "objects.h"
#include "ai.h"
#include "save.h"
#include "shell.h"
#include "help.h"
#include "planetnames.h"
#include "spacecomm.h"
#include "sound.h"
#include "graphics.h"
#include "functions.h"
#include "menu.h"

#define TESTSAVE FALSE
#define DEBUGFAST FALSE

int TESTSAVESTEP=1000;

#if DEBUG
int debugcrash=0;
int debugmouse=0;
int debuginit=0;
#endif

extern int g_objid;  /* id of the objects */
extern struct Global gremote,glocal;
extern GtkWidget *win_main;
extern GdkFont *gfont;
extern GdkPixmap *pixmap;
extern GdkGC *gcolors[];
extern GdkGC *penRed;
extern GdkGC *penGreen;
extern GdkGC *penLightGreen;
extern GdkGC *penBlue;
extern GdkGC *penYellow;
extern GdkGC *penWhite;
extern GdkGC *penBlack;
extern GdkGC *penOrange;
extern GdkGC *penViolet;
extern GdkGC *penPink;
extern GdkGC *penCyan;
extern GdkGC *penSoftRed;

extern Point mouse_pos;

struct Player *players;
struct CCDATA *ccdatap;
int actual_player,actual_player0;

int record=0;
int nav_mode=RELATIVE;

int p_time=0;
int g_nobjsend=0;
int g_nshotsend=0;
int g_nobjmaxsend=0;
int g_nobjtype[6]={0,0,0,0,0,0};
int g_memused=0;
int gameover=FALSE;
int observeenemies=FALSE;

char version[64]={"0.81.08"};
//char copyleft[]="Copyright XaY";
char copyleft[]="";
char TITLE[64]="SpaceZero  ";
char last_revision[]={"May 2011"};


Object *ship_c; /* ship controled by keyboard */
Object *cv;     /* coordinates center */
int fobj[4];
sem_t sem_barrier,sem_barrier1;


int g_cont;
time_t time0,time1;

int order2thread;

int *cell;


extern struct Keys keys;
extern struct Buffer buffer1,buffer2; /* buffers used in comm. */

struct HeadObjList listheadobjs;       /* list of all objects */
struct HeadObjList listheadplanets;    /* list of all planets */
struct HeadObjList *listheadcontainer; /* lists of objects that contain objects: free space and planets*/
struct HeadObjList *listheadkplanets;  /* lists of planets known by players */
struct HeadObjList listheadplayer;     /* list of objects of each player */
struct HeadObjList listheadnearobjs;   /* list of near enemies objects of actual player. only used in draw map function. */


struct TextMessageList listheadtext;
struct Parametres param;
Vector r_rel;

struct Habitat habitat;


char clientname[PLAYERNAMEMAXLEN];

unsigned int contabilidad[15];

/*  sound */
int soundenabled=TRUE;
/* -- sound */


int gordermode=FALSE; /* FALSE nav mode, TRUE order mode */ 

struct Draw gdraw;

void signal_handler(int ,siginfo_t *,void *);
void int_handler(int);
void segfault_handler(int);


char *savefile;
char *recordfile;


int main(int argc,char *argv[]){

  GtkWidget *drawing_area;
  FILE *fprecord;

  char title[64]="";
  int i,j;
  int sw;
  int state;
  struct sigaction sa;
  sigset_t wait_response; 
  int width,height;
  int npcs,npcc;
  int nteam=1;
  char *optionsfile;

  srand(time(NULL));

  for(i=0;i<15;i++)contabilidad[i]=0;
  Keystrokes(RESET,NULL);

#if TEST
  observeenemies=TRUE;
#endif

  PrintWarnings(version);
  optionsfile=CreateOptionsFile();

  state=Arguments(argc,argv,&param,optionsfile);
  if(state){
    /* printf("status:%d\n",state); */
    Usage(version,last_revision);
    exit(-1);
  }

  if(param.nplayers==-1){
    if(param.server==FALSE && param.client==FALSE){
      param.nplayers=NUMPLAYERS;
    }
    if(param.server==TRUE){
      param.nplayers=NUMPLAYERS;
    }
    if(param.client==TRUE){
      param.nplayers=1;
    }
  }

  if(CheckArgs(param)){
    fprintf(stderr,"ERROR in arguments, exiting...\n");
    exit(-1);
  }

  PrintArguments(param,"Game arguments:");



#if TESTSAVE
    strcat(savefiletmp,"/tmp/tmpsavespacezero");
#endif

  recordfile=CreateRecordFile();
  savefile=CreateSaveFile(param.server,param.client);

  fprintf(stdout,"save file: %s\n",savefile);
  fprintf(stdout,"record file: %s\n",recordfile);
  fprintf(stdout,"options file: %s\n",optionsfile);
  

  sw=0;
  if((fprecord=fopen(recordfile,"rt"))==NULL){

    if((fprecord=fopen(recordfile,"wt"))==NULL){
      fprintf(stdout,"No puede abrirse el archivo: %s", recordfile);
      exit(-1);
    }
    fprintf(fprecord,"%d\n",0);
    fclose(fprecord);

    if((fprecord=fopen(recordfile,"rt"))==NULL){
      fprintf(stdout,"No puede abrirse el archivo: %s", recordfile);
      exit(-1);
    }
  }
  if(fscanf(fprecord,"%d",&record)!=1){
    fprintf(stderr,"Setting record to 0\n");
    record=0;
  }

  fclose(fprecord);
  printf("Record: %d\n",record);

  GameParametres(SET,DEFAULT,0);   /* defaults game values */

  if(param.server==TRUE||param.client==TRUE){
    GameParametres(SET,GNET,TRUE);
  }

  if(param.server==TRUE){
    GameParametres(SET,GMODE,SERVER);
  }
  if(param.client==TRUE){
    GameParametres(SET,GMODE,CLIENT);
  }


  /* process identifier */

  SetProc(0);
  SetNProc(1);

  if(GameParametres(GET,GMODE,0)==CLIENT){
    SetProc(1);
  }
  if(GameParametres(GET,GNET,0)==TRUE){
    SetNProc(2);
  }

  /* --process identifier */


  listheadobjs.next=NULL;
  listheadobjs.n=0;
  listheadplanets.next=NULL;
  listheadplanets.n=0;

  listheadtext.next=NULL;
  listheadtext.info.n=0;

  listheadnearobjs.next=NULL;
  listheadnearobjs.n=0;

  GameParametres(SET,GNGALAXIES,param.ngalaxies);
  GameParametres(SET,GNPLANETS,param.nplanets);
  GameParametres(SET,GNPLAYERS,param.nplayers);  /*+1 system +1 pirates */

  if(param.kplanets==TRUE){
    GameParametres(SET,GKPLANETS,TRUE);
  }
#if DEBUG
  if(debuginit){
    printf("\tpirates: %d\n",param.pirates);
  }
#endif
  if(param.pirates==FALSE){
    GameParametres(SET,GPIRATES,FALSE);
  }
#if DEBUG
  if(debuginit){
    printf("\tpirates: %d %d\n",param.pirates,GameParametres(GET,GPIRATES,0));
  }
#endif
  GameParametres(SET,GULX,param.ul);
  GameParametres(SET,GULY,param.ul);

  GameParametres(SET,GCOOPERATIVE,param.cooperative);
  GameParametres(SET,GCOMPCOOPERATIVE,param.compcooperative);


  SetDefaultKeyValues(&keys,1);

  for(i=0;i<4;i++)fobj[i]=0;
  
  strcat(title,TITLE);
  if(param.server==TRUE)
    strcat(title,"(server)  ");
  if(param.client==TRUE)
    strcat(title,"(client)  ");
  strcat(title,version);
  strcat(title,"  ");
  strcat(title,copyleft);
  /* strcat(title,"  "); */
  /* strcat(title,last_revision); */
  


  time0=0;
  sem_init(&sem_barrier,1,0);
  sem_init(&sem_barrier1,1,0);


  sigemptyset(&wait_response);
  sa.sa_sigaction=signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags=SA_SIGINFO;
  if(sigaction(SIGNAL0,&sa,NULL)){
    perror("sigaction");
    exit(-1);
  }

  sa.sa_handler=int_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags=0;
  if(sigaction(SIGINT,&sa,NULL)){
    perror("sigaction");
    exit(-1);
  }

  sa.sa_handler=segfault_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags=0;
  if(sigaction(SIGSEGV,&sa,NULL)){
    perror("sigaction");
    exit(-1);
  }

  npcc=0;
  npcs=GameParametres(GET,GNPLAYERS,0);


  /* Graphics initialization */


  gtk_init(&argc,&argv);


  GameParametres(SET,GWIDTH,DEFAULTWIDTH);
  GameParametres(SET,GHEIGHT,DEFAULTHEIGHT);

  if(GetGeom(param.geom,&width,&height)<0){
    fprintf(stderr,"Warning: -geom option bad formed. Using default values.\n");
  }
  else{
    GameParametres(SET,GWIDTH,width);
    GameParametres(SET,GHEIGHT,height);
  }

  printf("Width: %d Height: %d\n",width,height);

  //  drawing_area=InitGraphics(title,optionsfile,game.width,game.height);

  gfont=InitFonts(param.font);

  drawing_area=InitGraphics(title,optionsfile,
			    GameParametres(GET,GWIDTH,0),
			    GameParametres(GET,GHEIGHT,0));

#if DEBUG
  if(debuginit){
    printf("W: %d H: %d \n", drawing_area->allocation.width,   
	   drawing_area->allocation.height);
  }
#endif

  if(gfont==NULL){
    GameParametres(SET,GPANEL,PANEL_HEIGHT);
  }
  else{
    GameParametres(SET,GPANEL,2*gdk_text_height(gfont,"pL",2));
  }


  GameParametres(SET,GHEIGHT,height-GameParametres(GET,GPANEL,0));


  /* --Graphics initialization */

  gcolors[0]=penWhite;
  gcolors[1]=penYellow;
  gcolors[2]=penRed;
  gcolors[3]=penGreen;
  gcolors[4]=penBlue;
  gcolors[5]=penOrange;
  gcolors[6]=penViolet;
  gcolors[7]=penPink;
  gcolors[8]=penCyan;
  gcolors[9]=penLightGreen; /* penWhite */
  gcolors[10]=penSoftRed;


  if(GameParametres(GET,GNET,0)==TRUE){
    if(param.server==TRUE){
      printf("SERVER\n");
      OpenComm(0,param);
    }
    if(param.client==TRUE){
      printf("CLIENT\n");
      OpenComm(1,param);
    }  
    printf("conexion established\n");
    
    /* synchronization with comm threads  */
    sem_wait(&sem_barrier);
  }
  
  printf("MAIN:\n nplayers: %d\n",GameParametres(GET,GNPLAYERS,0));
  printf("nproc: %d  proc: %d\n",GetNProc(),GetProc());

  players=malloc((GameParametres(GET,GNPLAYERS,0)+2)*sizeof(struct Player)); /* +1 system +1 pirates*/ 
  if(players==NULL){ 
    fprintf(stderr,"ERROR in malloc (players)\n"); 
    exit(-1); 
  } 
  g_memused+=(GameParametres(GET,GNPLAYERS,0)+2)*sizeof(struct Player);

  ccdatap=malloc((GameParametres(GET,GNPLAYERS,0)+2)*sizeof(struct CCDATA)); /* +1 system +1 pirates*/
  if(ccdatap==NULL){ 
    fprintf(stderr,"ERROR in malloc (ccdatap)\n"); 
    exit(-1); 
  } 
  g_memused+=(GameParametres(GET,GNPLAYERS,0)+2)*sizeof(struct CCDATA);



  for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    snprintf(players[i].playername,PLAYERNAMEMAXLEN,"player%d",i);
    players[i].id=i;
    players[i].pid=GameParametres(GET,GNPLANETS,0)+1;
    players[i].proc=0;
    players[i].control=COMPUTER;
    players[i].team=i+1;
    players[i].profile=PLAYERPROFDEFAULT;
    players[i].strategy=PLAYERSTRATRANDOM;
    /* strategy is random weight choosed at WarCCPlanets() */
    players[i].strategy=(int)(NUMPLAYERSTRAT*Random(-1));
    players[i].maxlevel=0;
    players[i].cv=0;
    players[i].color=i;
    players[i].nplanets=0;
    players[i].nships=0;
    players[i].nbuildships=0;
    players[i].gold=10000*RESOURCEFACTOR;
    players[i].lastaction=0;
    players[i].ndeaths=0;
    players[i].nkills=0;
    players[i].points=0;
    players[i].ttl=2000;
    players[i].modified=SENDOBJUNMOD;
    players[i].kplanets=NULL;
    players[i].ksectors.n=0;
    players[i].ksectors.n0=0;
    players[i].ksectors.list=NULL;
    for(j=0;j<NINDEXILIST;j++){
      players[i].ksectors.index[j]=NULL;
    }

    /* player control and assigment of processors */
    players[i].control=COMPUTER;
    players[i].proc=0;
    if(i==1){
      players[i].control=HUMAN;
    }
    if(GameParametres(GET,GNET,0)==TRUE){
      if(i==1){
	players[i].proc=1;
	if(strlen(clientname)>0){
	  snprintf(players[i].playername,PLAYERNAMEMAXLEN,"%s",clientname);	
	}
      }
      if(i==2){
	players[i].control=HUMAN;
	if(strlen(param.playername)>0){
	  snprintf(players[i].playername,PLAYERNAMEMAXLEN,"%s",param.playername);
	}
      }
    }
    else{
      if(i==1){
	players[i].control=HUMAN;
	if(strlen(param.playername)>0){
	  snprintf(players[i].playername,PLAYERNAMEMAXLEN,"%s",param.playername);
	}
      }
    }
  }
  snprintf(players[GameParametres(GET,GNPLAYERS,0)+1].playername,PLAYERNAMEMAXLEN,"%s","pirates");

  for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){ /* HERE TODO include ccdata in player*/
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
    
    ccdatap[i].sw=0;
    ccdatap[i].war=0;
    
    ccdatap[i].planetlowdefense=NULL;
    ccdatap[i].planetweak=NULL;
    ccdatap[i].planet2meet=NULL;
    ccdatap[i].planet2attack=NULL;
  }
  
  /* teams */
  players[0].team=1;   /* Universe objects */

  /* default mode:  All against all */
  
  for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    players[i].team=i+1; 
  }
  nteam=2; 

  /* human players */
  for(i=1;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    if(players[i].control==HUMAN){
      players[i].team=nteam;
      if(param.cooperative==FALSE){
	nteam++;
      }
    }
  }
  if(param.cooperative==TRUE){
    nteam++;
  }

  /* computer players */
  for(i=1;i<GameParametres(GET,GNPLAYERS,0)+1;i++){
    if(players[i].control==COMPUTER){
      players[i].team=nteam;
      if(param.compcooperative==FALSE){
	nteam++;
      }
    }
  }
  if(param.compcooperative==TRUE){
    nteam++;
  }

  /* pirate player*/
  i=GameParametres(GET,GNPLAYERS,0)+1;
  players[i].team=nteam;

  /* --teams */

  if(param.queen){
    printf("WARNING: Queen mode set to ON.\n");
    GameParametres(SET,GQUEEN,TRUE);
  }

  if(param.client==FALSE){
    printf("CreateUniverse()...");
    CreateUniverse(GameParametres(GET,GULX,0),GameParametres(GET,GULY,0),&listheadobjs,planetnames);
    CreatePlanetList(listheadobjs,&listheadplanets);
    printf("...done\n");
    printf("CreateShips()..."); 
    CreateShips(&listheadobjs); 
    //    CreateTestShips(&listheadobjs);

    printf("...done\n"); 
  }


  listheadcontainer=malloc((GameParametres(GET,GNPLANETS,0)+1)*sizeof(struct HeadObjList));
  g_memused+=(GameParametres(GET,GNPLANETS,0)+1)*sizeof(struct HeadObjList);
  if(listheadcontainer==NULL){
    fprintf(stderr,"ERROR in malloc main()\n");
    exit(-1);
  }
  for(i=0;i<GameParametres(GET,GNPLANETS,0)+1;i++){
    listheadcontainer[i].next=NULL;
    listheadcontainer[i].n=0;
  }

  listheadkplanets=malloc((GameParametres(GET,GNPLAYERS,0)+2)*sizeof(struct HeadObjList));
  g_memused+=(GameParametres(GET,GNPLAYERS,0)+2)*sizeof(struct HeadObjList);
  if(listheadkplanets==NULL){
    fprintf(stderr,"ERROR in malloc main()\n");
    exit(-1);
  }
  for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    listheadkplanets[i].next=NULL;
    listheadkplanets[i].n=0;
  }

  /************************************************************/

  if(param.server==TRUE){
    //    sprintf(players[2].playername,"%s",param.playername);
    
    if(ExecSave(listheadobjs,SAVETMPFILE)!=0){
      fprintf(stderr,"Error in main(): I cant open %s\n",SAVETMPFILE);
      exit(-1);
    }
    printf("gid: %d\n",g_objid);
    SetModifiedAll(&listheadobjs,ALLOBJS,SENDOBJUNMOD,TRUE);
    p_time=GetTime();
  }
  
  if(GameParametres(GET,GNET,0)==TRUE){
    sem_post(&sem_barrier1);
    sem_wait(&sem_barrier);
  }

  if(param.client==TRUE){

    printf("BEF GU:  gid:%d\n",g_objid);
    printf("gid loc:%d\n",glocal.g_objid);
    printf("gid rem:%d\n",gremote.g_objid);

    //    sprintf(players[1].playername,"%s",param.playername);

    GetUniverse();


    printf("AFTER GU:  gid:%d\n",g_objid);
    printf("gid loc:%d\n",glocal.g_objid);
    printf("gid rem:%d\n",gremote.g_objid);
    g_objid=glocal.g_objid;

    SetModifiedAll(&listheadobjs,ALLOBJS,SENDOBJUNMOD,TRUE);
    CreatePlanetList(listheadobjs,&listheadplanets);
  }

  actual_player=1;
  if(GameParametres(GET,GMODE,0)==SERVER){ /* only two human players, by now*/ 
    actual_player=2;
  }
  actual_player0=actual_player;
  players[actual_player].control=HUMAN;

  printf("Actual Player: %d\n",actual_player);
#if DEBUG
  if(debuginit){
    for(i=1;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
      printf("PLAYER: %d\n",i);
      printf("\tcontrol: %d\n",players[i].control);
      printf("\tproc: %d\n",players[i].proc);
    }
  }
#endif
  DestroyObjList(&listheadplayer); 
  listheadplayer.n=0; 
  listheadplayer.next=NULL; 
  CreatePlayerList(listheadobjs,&listheadplayer,actual_player);


  cv=NextCv(&listheadplayer,cv,actual_player);
  if(cv!=NULL){
    habitat.type=cv->habitat;
    habitat.obj=cv->in;
    cv->selected=TRUE;
  }
  
   
  {     /* Adding planets to players list */
    struct ObjList *ls;
    ls=listheadobjs.next;
    while(ls!=NULL){
      if(ls->obj->type==PLANET){      
	/* pirates known some planets */
	/* players[GameParametres(GET,GNPLAYERS,0)+1].kplanets= */
	/*   Add2IntList((players[GameParametres(GET,GNPLAYERS,0)+1].kplanets),ls->obj->id); */
	/* rest of players */
	for(i=0;i<GameParametres(GET,GNPLAYERS,0)+1;i++){
	  if(GameParametres(GET,GKPLANETS,0)==TRUE ||
	     players[ls->obj->player].team==players[i].team ||
	     (ENEMIESKNOWN==TRUE && players[ls->obj->player].team > 1) ){
	    players[i].kplanets=Add2IntList((players[i].kplanets),ls->obj->id);
	  }
	}
      }
      ls=ls->next;
    }
  }

#if SOUND
  /* sound initialization*/ 
  
  GameParametres(SET,GMUSIC,param.music);
  GameParametres(SET,GSOUND,param.sound);
  state=InitSound();
  if(state!=0){
    fprintf(stderr,"Error initializing sound, sound disabled Error id:%d\n",state);
    soundenabled=FALSE;
    param.sound=FALSE;
    param.music=FALSE;
    GameParametres(SET,GMUSIC,param.sound);
    GameParametres(SET,GSOUND,param.music);
    Play(NULL,-1,0); /* disable sound */
  }

  if(soundenabled==TRUE){
    PlaySound(MUSIC,SLOOP,0.75);
    if(param.music){
      printf("Music is on (%d)\n",param.music);
    }
    else{
      Sound(SSTOP,MUSIC);
      printf("Music is off\n");
    }
  }


  /*--sound initialization*/ 
#endif


  /* pruebas verlet list */
  if(0){
    struct VerletList *vl;
    printf("Creating verlet list ...");
    vl=CreateVerletList(listheadobjs);
    printf(" ...done\n");
    printf("printing ...");
    PrintVerletList(vl);
    printf("... done\n");
    printf("destrying ...");
    DestroyVerletList(vl);
    printf("... done\n");
    printf("printing ...");
    PrintVerletList(vl);
    printf("... done\n");
    
  }
  /* --pruebas verlet list */


  /* printf teams */

  for(i=1;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    printf("PLAYER %d TEAM %d ",i,players[i].team);
    if(players[i].control==HUMAN)printf("HUMAN");
    if(players[i].control==COMPUTER)printf("COMPUTER");
    printf(" name: \"%s\"",players[i].playername);
    printf("\n");
  }

#if CELLON
  {
    int nx,ny;
    nx=GameParametres(GET,GULX,0)/2000;
    ny=GameParametres(GET,GULY,0)/2000;

    cell=malloc(nx*ny*sizeof(int));
    if(cell==NULL){ 
      fprintf(stderr,"ERROR in malloc (creating cell)\n"); 
      exit(-1); 
    }
    for(i=0;i<nx*ny;i++){
      cell[i]=0;
    }
  } 
#endif    

  PrintGameOptions();
  gtk_timeout_add((int)(DT*200),MenuLoop,(gpointer)drawing_area);  /* 42 DT=0.42 in general.h*/

#if DEBUGFAST
  gtk_timeout_add((int)(DT*5),MainLoop,(gpointer)drawing_area);  /* 42 DT=0.42 in general.h*/
#else
  gtk_timeout_add((int)(DT*100),MainLoop,(gpointer)drawing_area);  /* 42 DT=0.42 in general.h*/
#endif
  
  /* what stuff draw */
  gdraw.menu=TRUE;
  gdraw.map=FALSE;
  gdraw.shiplist=FALSE;
  gdraw.stats=FALSE;
  gdraw.order=FALSE;
  gdraw.info=FALSE;
  gdraw.crash=FALSE;  

#if TEST
  gdraw.menu=TRUE;
#else
  gdraw.menu=FALSE;
#endif


  gtk_main();

  printf("\ntotal points: %d record: %d\n",players[1].points,record);



  printf("******************************************************\n");
  g_print("%sversion %s  %s\n",TITLE,version,last_revision);
  g_print("Please, send bugs and suggestions to: mrevenga at users dot sourceforge dot net\n");
  g_print("Homepage:  http://spacezero.sourceforge.net/\n");


  j=-1;
  for(i=0;i<GameParametres(GET,GNPLAYERS,0);i++){
    if(players[i].points>=record){
      j=i;
      record=players[i].points;
    }
  }
  if(j!=-1){
    if((fprecord=fopen(recordfile,"wt"))==NULL){
      fprintf(stdout,"No puede abrirse el archivo: %s", recordfile);
      exit(-1);
    }
    fprintf(fprecord,"%d",record);
    fclose(fprecord);
  }

  return 0;
} /*  main */

gint MenuLoop(gpointer data){
  GdkRectangle update_rect;
  GtkWidget *drawing_area=(GtkWidget *) data;
  int x,y;
  int gwidth,gheight;
  char point[128];

  if(gdraw.menu==FALSE)return(TRUE);
  key_eval(&keys);

  gwidth=GameParametres(GET,GWIDTH,0);
  gheight=GameParametres(GET,GHEIGHT,0);


  /* clear window */

  gdk_draw_rectangle(pixmap,     
		     drawing_area->style->black_gc,     
		     TRUE,    
		     0,0,     
		     drawing_area->allocation.width,     
		     drawing_area->allocation.height); 

  /* printf something */  

  DrawStars(pixmap,nav_mode,0,0);

  x=gwidth/2;
  y=gheight/5;
  sprintf(point,"SpaceZero");
  DrawMessageBox(drawing_area,pixmap,gfont,point,x,y,MBOXDEFAULT);

  x=gwidth/2;
  y=gheight*(.80);

  if(GameParametres(GET,GNET,0)==TRUE){
    if(param.server==TRUE){
      sprintf(point,"PRESS ENTER TO START");
    }
    else{
      sprintf(point,"WATING TO SERVER TO START");
    }
  }
  else{
    sprintf(point,"PRESS ENTER TO START");
  }

  DrawMessageBox(drawing_area,pixmap,gfont,point,x,y,MBOXDEFAULT);

  if(0)  {
    struct MenuList mlist;
    struct MenuItem item;
    int textw;

    mlist.next=NULL;


    item.type=MENUITEMTEXT;
    strncpy(item.text,"Options",MAXMENULEN);
    Add2MenuList(&mlist,&item);

    item.type=MENUITEMTEXT;
    strncpy(item.text,"Start",MAXMENULEN);
    Add2MenuList(&mlist,&item);

    item.type=MENUITEMTEXT;
    strncpy(item.text,"Quit",MAXMENULEN);
    Add2MenuList(&mlist,&item);

    textw=gdk_text_width(gfont,"Options",strlen("Options"));

    XPrintMenuList(pixmap,gfont,&mlist,GameParametres(GET,GWIDTH,0)/2-textw-20,GameParametres(GET,GHEIGHT,0)/2);

    //    PrintMenuList(&mlist);
    fflush(NULL);
    //    exit(0);
 }

  /* show window */
  update_rect.x=0;
  update_rect.y=0;
  update_rect.width=drawing_area->allocation.width;
  update_rect.height=drawing_area->allocation.height;
  
  gtk_widget_draw(drawing_area,&update_rect); /*  deprecated */
  
  return(TRUE);
}

gint MainLoop(gpointer data){
  /*
    Main gtk loop. executed 24 times by second.

*/
  int i;
  int drawmap=FALSE;
  static int cont=0;
  float x0,y0;
  static int lasttimepirates=2000;
  static int lasttimeasteroids=0;
  Object *cv0;     /* coordinates center */
  GtkWidget *drawing_area=(GtkWidget *) data;
  GdkRectangle update_rect;
  static int paused=0;
  static int swpaused=0;
  int gwidth,gheight;
  static int ulx,uly;
  int proc;
  int swcomm=0; /* TRUE if in time step has communication */ 
  int swmess=0; /* show a message */
  char point[128];
  char pointmess[128];
  int loadsw =0;

  if(gdraw.menu==TRUE)return(TRUE);

  if(DEBUG){
    if(!(cont%100)){
      printf("mem used:%d\n",g_memused);
    }
  }

  if(time0==0){
    time0=time(NULL);
  }
  
  
  ulx=GameParametres(GET,GULX,0);
  uly=GameParametres(GET,GULY,0);

  gwidth=GameParametres(GET,GWIDTH,0);
  gheight=GameParametres(GET,GHEIGHT,0);


  proc=GetProc();

#if CELLON
  {
    int nx,ny;
    nx=GameParametres(GET,GULX,0)/2000;
    ny=GameParametres(GET,GULY,0)/2000;
    for(i=0;i<nx*ny;i++){
      cell[i]=0;
    }
    UpdateCell(&listheadobjs,cell);
  } 
#endif

  for(i=0;i<GameParametres(GET,GNPLANETS,0)+1;i++){
    DestroyObjList(&listheadcontainer[i]);
    listheadcontainer[i].next=NULL;
    listheadcontainer[i].n=0;
  }
  for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    DestroyObjList(&listheadkplanets[i]);
    listheadkplanets[i].next=NULL;
    listheadkplanets[i].n=0;
  }

  key_eval(&keys);

  if(GameParametres(GET,GPAUSED,0)==FALSE){
    //    if(swpaused<NETSTEP){  
      DestroyObjList(&listheadplayer);
      listheadplayer.next=NULL;
      listheadplayer.n=0;
      
      CreatePlayerList(listheadobjs,&listheadplayer,actual_player);
      CreateContainerLists(&listheadobjs,listheadcontainer);
      CreatekplanetsLists(&listheadobjs,listheadkplanets);
      //    }
  }

  /* messages */
  swmess=0;


  /* GAME OVER */
  if(actual_player==actual_player0){
    
    static int n=1;
    static int player=0;
    if(player!=actual_player){
      n=0;
      player=actual_player;
    }
    if(GameParametres(GET,GPAUSED,0)==FALSE){
      if(n<=0){
	n=CountObjs(&listheadplayer,-1,SHIP,-1);
	n*=10;
      }
      
      if(n<=0){
	gameover=TRUE;
	observeenemies=TRUE;
      }
      n--;
    }
  }

  if(gameover==TRUE){
    sprintf(pointmess,"GAME OVER");
    swmess++;
  }
  /* game paused */
  if(GameParametres(GET,GPAUSED,0)==TRUE){
    sprintf(pointmess,"PAUSED    (press p to continue)");
    swmess++;
  }
  /* game quit */
  if(GameParametres(GET,GQUIT,0)==1){
    sprintf(pointmess,"Really QUIT?  ( y/n )");
    swmess++;
  }

  if(swmess){
    DrawMessageBox(drawing_area,pixmap,gfont,pointmess,gwidth/2,0.3*gheight,MBOXBORDER);
  }

  if(GameParametres(GET,GPAUSED,0)==TRUE){
    if(swpaused>=NETSTEP){
      //      DestroyObjList(&listheadplayer); // HERE quitar esto? revisar
      return(TRUE);
    }
    if(GameParametres(GET,GNET,0)==TRUE){
      sprintf(point,"Game PAUSED");
      SendTextMessage(point);   
    }
    swpaused++;
  }
  else{
    swpaused=0;
  }

  if(GameParametres(GET,GQUIT,0)==1){
    //    return(TRUE);
  }

  drawmap=FALSE;
  if(!(cont%2))drawmap=TRUE;
  

  /* ai */

  for(i=1;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    if(players[i].control==COMPUTER && proc==players[i].proc){
      ControlCenter(&listheadobjs,players[i]);
    }
  }

  CheckGame("");
  UpdateObjs();                     /* new positions */

  for(i=1;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
    if(proc==players[i].proc){
      players[i].ttl--;
      if(0)printf("player %d ttl %d\n",i,players[i].ttl);
      if(players[i].ttl<=0)players[i].modified=SENDPLAYERMOD;
    }
  }
  
  if(drawmap==TRUE){
    if(!(cont%10)){
      DestroyObjList(&listheadnearobjs);
      listheadnearobjs.n=0;
      listheadnearobjs.next=NULL;
      CreateNearObjsList(&listheadobjs,&listheadnearobjs,actual_player);
    }
  }

  for(i=0;i<GameParametres(GET,GNPLANETS,0)+1;i++){
    Collision(&listheadcontainer[i]);         /* interact among objects */
    //if(!(cont%100))printf("LIST: %d %d\n",i,listheadcontainer[i].n);  
  }
  //  if(!(cont%100))printf("LIST: %d %d\n",0,listheadcontainer[0].n);  
  GetGold();     
  if(GameParametres(GET,GPIRATES,0)==TRUE && (players[GameParametres(GET,GNPLAYERS,0)+1].nplanets<5)){
    if(proc==players[GameParametres(GET,GNPLAYERS,0)+1].proc){/*  Send TO ai */
      if(GetTime()-lasttimepirates>2000){
	if(((20000.0*rand())/RAND_MAX)<=1){
	  char text[TEXTMENMAXLEN];
	  lasttimepirates=GetTime();
	  x0=ulx*Random(-1)-ulx/2;
	  y0=uly*Random(-1)-uly/2;
	  CreatePirates(&listheadobjs,4,x0,y0);
	  snprintf(text,TEXTMENMAXLEN,"PIRATES!!! at sector: %d %d",(int)(x0/SECTORSIZE),(int)(y0/SECTORSIZE));
	  Add2TextMessageList(&listheadtext,text,0,-1,0,100,0);
	  if(GameParametres(GET,GNET,0)==TRUE){
	    SendTextMessage(text);
	  }
	}
      }
    }
  }

  if(proc==players[GameParametres(GET,GNPLAYERS,0)+1].proc){/*  Send TO ai */
    //    if(GameParametres(GET,GNET,0)==FALSE){
    if(GetTime()-lasttimeasteroids>3000){
      if(((11000.0*rand())/RAND_MAX)<=1){ /* every 10 minutes */
	char text[TEXTMENMAXLEN];
	float factor;

	factor=(float)GameParametres(GET,GULX,0)/100000.0;
	factor*=factor;
	factor=2*(factor+1);
	lasttimeasteroids=GetTime();
	for(i=0;i<factor;i++){
	  x0=ulx*Random(-1)-ulx/2;
	  y0=uly*Random(-1)-uly/2;
	  CreateAsteroids(&listheadobjs,6,x0,y0);
	  
	  snprintf(text,TEXTMENMAXLEN,"ASTEROIDS!!! at sector: %d %d",(int)(x0/SECTORSIZE),(int)(y0/SECTORSIZE));
	  Add2TextMessageList(&listheadtext,text,0,-1,0,100,0);
	  if(GameParametres(GET,GNET,0)==TRUE){
	    SendTextMessage(text);   
	  }
	}
      }
    }
    //    }
  }
  
  if(swpaused==TRUE){
    printf("PAUSED\n");
    fflush(NULL);
  }
  
  /* synchronization with comm threads */

  swcomm=FALSE;
  if(GameParametres(GET,GNET,0)==TRUE){
    CheckModifiedPre(&listheadobjs,proc);
    Setttl0(&listheadobjs);
    if( !(cont%NETSTEP)){  
      NetComm();     /* net communication */
      CheckModifiedPost(&listheadobjs,proc);
      swcomm=TRUE;
    }
    Setttl(&listheadobjs,-1);
  }
  else{
    swcomm=TRUE;
  }


  GetPoints(listheadobjs,proc,players);/* points and experience */

  if(GameParametres(GET,GKPLANETS,0)==FALSE){
    UpdateSectors(listheadobjs);
  }
  
#if TESTSAVE
  {
    static int testsavesw=0; 
    if(0){  
      
      savefile=savefile0;
      if(param.client==TRUE){
	savefile=savefile1;
      }
      if(param.server==TRUE){
	savefile=savefile0;
      }
    }
    if(testsavesw){
      if (!((cont-10)%TESTSAVESTEP)){
	keys.load=TRUE;
	keys.save=FALSE;
	printf("LOAD %d\n",cont);
      }
    }
    if (!(cont%TESTSAVESTEP)){
      keys.save=TRUE;
      keys.load=FALSE;
      printf("SAVE %d\n",cont);
      testsavesw=1;
    }
  }
#endif

  if(keys.load==TRUE && swcomm==TRUE){
    keys.load=FALSE;
    keys.save=FALSE;

    DestroyObjList(&listheadplayer);  //HERE verify with DestroyAllObjs() in execload()
    listheadplayer.n=0;  
    listheadplayer.next=NULL;  

    if(ExecLoad(savefile)==0){
      loadsw=1;
      p_time=GetTime();
      gameover=FALSE;
      observeenemies=FALSE;
      CreatePlanetList(listheadobjs,&listheadplanets);
      //      Density();
      
      { /* deleting the message list */
	struct TextMessageList *lh;
	lh=&listheadtext;
	while(lh->next!=NULL){
	  lh->next->info.duration=0;
	  lh=lh->next;
	}
      }
      SetDefaultKeyValues(&keys,0);
      CheckGame("Checking game after load...");
      printf("done\n");
    }
    CreatePlayerList(listheadobjs,&listheadplayer,actual_player);
  }


  if(keys.save==TRUE && swcomm==TRUE){
    char text[TEXTMENMAXLEN];
    keys.load=FALSE;
    keys.save=FALSE;


    if(ExecSave(listheadobjs,savefile)==0){    
      snprintf(text,TEXTMENMAXLEN,"GAME SAVED.");
      Add2TextMessageList(&listheadtext,text,0,-1,0,100,0);
      if(GameParametres(GET,GNET,0)==TRUE){
	SendTextMessage(text);   
      }
    }
    else{
      fprintf(stderr,"Error in MainLoop(): I cant open %s\n",savefile);
    }
  }
  
  if(cv!=NULL){
    if(nav_mode==RELATIVE){
      r_rel.x=cv->x;
      r_rel.y=cv->y;
    }
    else{
      if(cv->x - r_rel.x>gwidth/2)
	r_rel.x+=gwidth;
      if(cv->x - r_rel.x <-gwidth/2)
	r_rel.x-=gwidth;
      
      if(cv->y - r_rel.y>gheight/2)
	r_rel.y+=gheight;
      if(cv->y - r_rel.y <-gheight/2)
	r_rel.y-=gheight;
    }
  }

  if(0){
    struct Order *ord;

    if(cv!=NULL){
      ord=ReadOrder(NULL,cv,MACRO);
      if(ord!=NULL){
	printf("%d %d %d %d\n",
	       ord->priority,ord->id,ord->time,ord->g_time);
	printf("%g %g %g %g\n%g %g %g %g\n",
	       ord->a,ord->b,ord->c,ord->d,
	       ord->e,ord->f,ord->g,ord->h);
      }
    }
  }
  
  /* Drawing window */

  /*  what to draw */
  if(keys.f5==TRUE){
    gdraw.shiplist=TRUE;
    gdraw.info=FALSE;
  }
  else{
    gdraw.shiplist=FALSE;
    gdraw.info=TRUE;
  }
  if(keys.f6==TRUE){
    gdraw.stats=TRUE;
  }
  else{
    gdraw.stats=FALSE;
  }

  if(keys.o==TRUE){
    gdraw.order=TRUE;
  }
  if(keys.m==TRUE){
    
    if(!(Shell(1,NULL,NULL,NULL,NULL,NULL,NULL,NULL)==2 &&
	Shell(2,NULL,NULL,NULL,NULL,NULL,NULL,NULL)==WRITE)){
    
      gdraw.map=gdraw.map==TRUE?FALSE:TRUE;
    }
    keys.m=FALSE;
  }


  /*--  what to draw */


  /* game window*/
  if(paused==0){

    /* clear window */

    if(gdraw.map==TRUE){
      if(drawmap){ 
	gdk_draw_rectangle(pixmap,   
			   drawing_area->style->black_gc,   
			   TRUE, 
			   0,0,   
			   drawing_area->allocation.width,   
			   drawing_area->allocation.height);  
      }
    } 
    else{ 
      if(gdraw.crash){ 
	gdk_draw_rectangle(pixmap,   
			   drawing_area->style->white_gc,   
			   TRUE,  
			   0,0,   
			   drawing_area->allocation.width,   
			   drawing_area->allocation.height);  
	gdraw.crash=0; 
      } 
      else{  

#if DEBUGFAST

      if(!(cont%120)){
 	gdk_draw_rectangle(pixmap,     
 			   drawing_area->style->black_gc,     
 			   TRUE,    
 			   0,0,     
 			   drawing_area->allocation.width,     
 			   drawing_area->allocation.height); 

      }

#else

 	gdk_draw_rectangle(pixmap,     
 			   drawing_area->style->black_gc,     
 			   TRUE,    
 			   0,0,     
 			   drawing_area->allocation.width,     
 			   drawing_area->allocation.height); 

#endif

      }  
    }
    
    
    /* --clear window */

       
    if(gdraw.map==TRUE){
      if(drawmap){
	DrawMap(pixmap,actual_player,listheadobjs,cv,ulx);
      }
    }
    else{  
      if(habitat.type==H_SPACE && cv!=NULL)DrawStars(pixmap,nav_mode,r_rel.x,r_rel.y);
      {int n;
      n=DrawObjs(pixmap,&listheadobjs,habitat,cv,r_rel); 
      //      printf("obj drawed: %d\n",n);
      }
      if(cv!=NULL && (habitat.type==H_SPACE)){
	//	DrawRadar(pixmap,cv,listheadcontainer);
	DrawRadar(pixmap,cv,&listheadobjs);
      }
      
      if(habitat.type==H_PLANET && cv!=NULL){
	DrawPlanetSurface(pixmap,habitat.obj->planet,gcolors[players[habitat.obj->player].color]); 
      }

      if(cv!=NULL){
	if(cv->accel>0){
	  Play(cv,THRUST,cv->accel/cv->engine.a_max);
	}
      }
    }
    if(gdraw.shiplist==TRUE){
      DrawPlayerList(pixmap,actual_player,&listheadplayer,cv,loadsw || !(GetTime()%20));
    }
    DrawInfo(pixmap,cv);
  }/* if(paused==0) */

    if(gdraw.stats==TRUE){
      DrawGameStatistics(pixmap,players);
    }
    
    if(swmess){ 
      DrawMessageBox(drawing_area,pixmap,gfont,pointmess,gwidth/2,0.3*gheight,MBOXBORDER);
    } 


  /* Draw Shell */
  
  if(gdraw.order==TRUE){/* QWERTY */
    gdk_draw_line(pixmap,penWhite,
		  0,gheight,
		  gwidth,gheight);
    
    cv0=cv;     /* coordinates center */

    Shell(0,pixmap,gfont,penGreen,&listheadobjs,players,&keys,&cv);
    if(keys.o==FALSE)gdraw.order=FALSE;
    if(cv!=NULL){
      if(cv->state==-1){ /* Object just selled*/
	cv=SelectObjInObj(&listheadplayer,cv->in->id,cv->player);
	if(cv!=NULL){
	  cv->selected=TRUE;
	  if(cv->in==cv0->in){
	    habitat.type=cv->habitat;
	    habitat.obj=cv->in;
	  }
	  else{
	    cv=cv0;
	  }
	}
      }
      
      if(cv0!=cv){ /* if center coordinates changes */
	if(cv!=NULL){
	  habitat.type=cv->habitat;
	  habitat.obj=cv->in;
	  if(cv->type==PLANET){
	    habitat.type=H_PLANET;
	    habitat.obj=cv;
	  }
	}
      }
    }
  }
  else{
    gdk_draw_line(pixmap,penRed,
		  0,gheight,
		  gwidth,gheight);
  }
  
  /* --Draw Shell */


  /* Draw selection box */
  DrawSelectionBox(&cv,0);
  /* --Draw selection box */



  /* show window */
 
  if(gdraw.map==TRUE){ 
    if(drawmap){ 
      update_rect.x=0;
      update_rect.y=0;
      update_rect.width=drawing_area->allocation.width;
      update_rect.height=drawing_area->allocation.height;
    }
  }
  else{
    update_rect.x=0;
    update_rect.y=0;
    update_rect.width=drawing_area->allocation.width;
    update_rect.height=drawing_area->allocation.height;
  }
  if(0){ /* CPU improve: 61% -> 48 % => +21% */
    
    update_rect.x=drawing_area->allocation.width/4; 
    update_rect.y=0;
    update_rect.width=drawing_area->allocation.width/2;
    update_rect.height=drawing_area->allocation.height;
  }



  /* PRODUCTION */
  if(1){
    gtk_widget_draw(drawing_area,&update_rect); /*  deprecated */
  }
  else{
    if(GameParametres(GET,GMODE,0)==SERVER){
      if(0||!(cont%30)){
	//	gtk_widget_queue_draw(drawing_area); 
	gtk_widget_draw(drawing_area,&update_rect); /*  deprecated */
      }
    }
    else{


#if DEBUGFAST
      if(!(cont%120)){
	//gtk_widget_queue_draw(drawing_area); 
	gtk_widget_draw(drawing_area,&update_rect); /*  deprecated */
      }  
      
#else
      gtk_widget_draw(drawing_area,&update_rect); /*  deprecated */
#endif
    }
  }

  if(GameParametres(GET,GPAUSED,0)==TRUE){
    paused=1;
  }
  else{
    paused=0;
  }
  
  /* --Drawing window */  
  

  if(GameParametres(GET,GPAUSED,0)==FALSE){
    IncTime();
  }

  loadsw=0;

  if(GameParametres(GET,GQUIT,0)==2 && swcomm==TRUE){
    Quit(NULL,NULL); 
  }
  
  if(RemoveDeadObjs(&listheadobjs,cv)==NULL){
    cv=NULL;
  }

  if(IsInObjList(&listheadobjs,ship_c)==0){  
    ship_c=NULL;  
  }  

  cont++;
  return(TRUE);

  } /* MainLoop */

gint Quit(GtkWidget *widget,gpointer gdata){
  /*
    
   */


#if DEBUG
  if(debuginit){
    Object *obj;
    int i;

    printf("****************** Statistics ************************\n");
    obj=NewObj(&listheadobjs,SHIP,SHIP1,
	       0,0,0,0,
	       CANNON0,ENGINE0,0,NULL,NULL);
    Add2ObjList(&listheadobjs,obj);
    if(obj!=NULL){
      if(debuginit)printf("Last obj id: %d\n",obj->id);
      obj=NewObj(&listheadobjs,PROJECTILE,EXPLOSION,
		 0,0,0,0,
		 CANNON0,ENGINE0,obj->player,obj,NULL);
      Add2ObjList(&listheadobjs,obj);
      if(obj!=NULL){
	printf("Last projectile id: %d\n",obj->id);
      }
    }
    g_print("Quitting...   frames:%d \n",GetTime()-p_time);
    g_print("frames/s:%f\n",(float)(GetTime()-p_time)/(time(NULL)-time0));
    g_print("key strokes:%d \n",CountKey(2));
    g_print("n. total obj. : %d\n",CountObjs(&listheadobjs,-1,-1,-1));
    g_print("n. total obj. sended: %d shots: %d\n",g_nobjsend,g_nshotsend);
    g_print("MOD0: %d MOD: %d AALL: %d ALL: %d NEW: %d PLANETS: %d\n",
	    g_nobjtype[0],g_nobjtype[1],g_nobjtype[2],g_nobjtype[3],
	    g_nobjtype[4],g_nobjtype[5]);
    g_print("total time: %.1f s.\n",(float)(GetTime()-p_time)/20);
    g_print("obj send /s: %.1f\n",20.0*g_nobjsend/(GetTime()-p_time));
    printf("******************************************************\n");
  }    
#endif
#if DEBUG
  if(debuginit){
    printf("Contabilidad collision:\n");
    for(i=0;i<15;i++)printf("\t(%d) %u\n",i,contabilidad[i]);
  }
#endif

  sem_close(&sem_barrier);
  sem_close(&sem_barrier1);

#if SOUND
  printf("Sound Closed\n");
  ExitSound();
  printf("Sound Closed\n");
#endif

  DestroyAllObj(&listheadobjs);
  listheadobjs.next=NULL;
  listheadobjs.n=0;
  ship_c=NULL;
  cv=NULL;


  QuitGraphics(widget,gdata);
  printf("Graphics Closed\n");
#if DEBUG
  if(debuginit)printf("MEM:%d\n",g_memused);
#endif
  return FALSE;
}

void key_eval(struct Keys *key){
  /*
    version 01 13May11
    Evaluate all the key and mouse press 

   */
  Object *obj;
  static int swtab=0;
  static int swtab0=0;
  int gulx,guly;
  int proc;
  int keyf=-1;
#if TEST
  Object *nobj;
  float x0,y0;
  char text[TEXTMENMAXLEN];
#endif

#if DEBUG
  printf("key_eval()\n");
#endif

  if(key->enter==TRUE && gdraw.menu==TRUE){
    gdraw.menu=FALSE;
    key->enter=FALSE;
  }
  if(gdraw.menu==TRUE){
    return;
  }
  proc=GetProc();
  gulx=GameParametres(GET,GULX,0);
  guly=GameParametres(GET,GULY,0);
  

  /* QUIT game */
  
  if(key->ctrl==TRUE && key->q==TRUE){
    GameParametres(SET,GQUIT,1);  /*  ask: Really Quit? */
    //    StopSound(MUSIC);
  }

  if(GameParametres(GET,GQUIT,0)==1){
    //    key->p=FALSE;
    if(key->n==TRUE || key->esc==TRUE){
      GameParametres(SET,GQUIT,0);
      key->ctrl=FALSE;
      key->q=FALSE;
      key->n=FALSE;
      key->esc=FALSE;
      //      PlaySound(MUSIC,SLOOP,0.75);
    }
    if(key->y==TRUE){ 
      GameParametres(SET,GQUIT,2);  /* Quit game */
      GameParametres(SET,GPAUSED,FALSE);
    }
    return;
  }

#if TEST
  /* Create pirates PRODUCTION */
  if(key->ctrl==TRUE &&  key->i==TRUE){
    if(proc==players[GameParametres(GET,GNPLAYERS,0)+1].proc){/* Send TO ai */
      x0=gulx*Random(-1)-gulx/2;
      y0=guly*Random(-1)-guly/2;
      /* CreatePirates(&listheadobjs,4,x0,y0);  */
      /* snprintf(text,TEXTMENMAXLEN,"PIRATES!!! at sector: %d %d",(int)(x0/SECTORSIZE),(int)(y0/SECTORSIZE));  */
      CreateAsteroids(&listheadobjs,6,x0,y0); 
      snprintf(text,TEXTMENMAXLEN,"ASTEROIDS!!! at sector: %d %d",(int)(x0/SECTORSIZE),(int)(y0/SECTORSIZE)); 

      Add2TextMessageList(&listheadtext,text,0,-1,0,100,0);
      if(GameParametres(GET,GNET,0)==TRUE){
	SendTextMessage(text);   
      }
    }
  }
  /*--Create pirates DELETE PRODUCTION */
#endif

  /* game paused */
  if( key->p==TRUE && GameParametres(GET,GPAUSED,0)==TRUE){
    GameParametres(SET,GPAUSED,FALSE);
    key->p=FALSE;
    Sound(SPLAY,MUSIC);
  }
  // dont pause in order mode
  if(key->o==FALSE && key->p==TRUE && GameParametres(GET,GPAUSED,0)==FALSE){
    GameParametres(SET,GPAUSED,TRUE);
    key->p=FALSE;
    Sound(SPAUSE,MUSIC);
  }

  /* mouse */
  if(key->mleft==TRUE){
  }
  if(key->mright==TRUE){
  }
  if(key->mdclick==TRUE){
    key->mdclick=FALSE;
  }
  /* --mouse */

  gordermode=FALSE;
  if(key->o==TRUE)gordermode=TRUE;
  switch(gordermode){
  case FALSE: /* Nav mode */
    /* f1 f2 f3 f4 */    
    keyf=-1;
    if(key->f1)keyf=0;
    if(key->f2)keyf=1;
    if(key->f3)keyf=2;
    if(key->f4)keyf=3;
    key->f1=key->f2=key->f3=key->f4=FALSE;
    
    if(keyf>=0) {
      if(key->ctrl==TRUE){
	key->ctrl=FALSE;
	if(cv!=NULL)
	  fobj[keyf]=cv->id;
      }
      else{
	if((obj=SelectObj(&listheadobjs,fobj[keyf]))!=NULL){
	  if(cv!=NULL)cv->selected=FALSE;
	  cv=obj;
	  habitat.type=cv->habitat;
	  habitat.obj=cv->in;
	  DrawSelectionBox(&cv,1);
	  cv->selected=TRUE;
	}
      }
    }  
    /*-- f1 f2 f3 f4 */    

    /* observe enemies */

    if(observeenemies==TRUE){
      if(key->f7==TRUE || key->f8==TRUE){
	if(cv!=NULL){
	  players[actual_player].cv=cv->id;
	}
	else{
	  players[actual_player].cv=0;
	}
	if(key->f7==TRUE){
	  key->f7=FALSE;
	  actual_player--;
	  if(actual_player<1){
	    actual_player=GameParametres(GET,GNPLAYERS,0)+1;
	  }
	}
	if(key->f8==TRUE){
	  key->f8=FALSE;
	  actual_player++;
	  if(actual_player>GameParametres(GET,GNPLAYERS,0)+1){
	    actual_player=1;
	  }
	}

	DestroyObjList(&listheadplayer);
	listheadplayer.n=0;
	listheadplayer.next=NULL;
	CreatePlayerList(listheadobjs,&listheadplayer,actual_player);
	
	cv=SelectObj(&listheadplayer,players[actual_player].cv);
	if(cv==NULL){
	  cv=NextCv(&listheadplayer,cv,actual_player);
	}
	if(cv!=NULL){
	  habitat.type=cv->habitat;
	  habitat.obj=cv->in;
	  DrawSelectionBox(&cv,1);
	}
      }
    }
    /*-- observe enemies */


    //    printf("ai: %p\n",ship_c); 
    if(ship_c!=NULL){
      if(ship_c->type!=SHIP){
	key->trace=FALSE;
      ship_c->trace=FALSE;
      }
      if(ship_c->subtype!=EXPLORER &&
	 ship_c->subtype!=FIGHTER &&
	 ship_c->subtype!=QUEEN){
	key->trace=FALSE;
      ship_c->trace=FALSE;
      }
      
      if(key->trace==TRUE){
	if(ship_c->trace==TRUE)
	  ship_c->trace=FALSE;
	else
	  ship_c->trace=TRUE;
	key->trace=FALSE;
      }
      
      if(ship_c->ai == 0 && proc==players[ship_c->player].proc){
	
	if(key->down==TRUE){
	  ship_c->accel-=.08;
	  if(ship_c->accel<0)ship_c->accel=0;
	}
#if TEST
	/* PRODUCTION DELETE SATELLITES */	
	if(key->s==TRUE && key->ctrl==FALSE){ 
	  nobj=NewObj(&listheadobjs,SHIP,SATELLITE,
		      ship_c->x-2.5*ship_c->radio*cos(ship_c->a),
		      ship_c->y-2.5*ship_c->radio*sin(ship_c->a),
		      ship_c->vx,ship_c->vy,
		      CANNON4,ENGINE1,ship_c->player,ship_c,ship_c->in);
	  Add2ObjList(&listheadobjs,nobj);
	  key->s=FALSE;
	}
	/* --PRODUCTION DELETE SATELLITES */	
#endif
	if(cv==ship_c){ 
	  if(ship_c->gas>0 && gdraw.map==FALSE){
	    if(key->up==TRUE){
	      ship_c->accel+=ship_c->engine.a;
	      if(ship_c->accel>ship_c->engine.a_max)ship_c->accel=ship_c->engine.a_max;
	      
	      if(ship_c->mode==LANDED && ship_c->vy>0){ship_c->mode=NAV;} 
	    }
	    else{
	      ship_c->accel=0;
	    }
	    if(key->left==TRUE && ship_c->gas > ship_c->engine.gascost){
	      ship_c->ang_a+=ship_c->engine.ang_a;
	      if(ship_c->ang_a > ship_c->engine.ang_a_max) 
		ship_c->ang_a=ship_c->engine.ang_a_max; 
	    }
	    if(key->right==TRUE && ship_c->gas>ship_c->engine.gascost){
	      ship_c->ang_a-=ship_c->engine.ang_a;
	      if(ship_c->ang_a < -ship_c->engine.ang_a_max) 
		ship_c->ang_a = -ship_c->engine.ang_a_max; 
	    }
	    if(key->left==FALSE  && key->right==FALSE){
	      ship_c->ang_a=0;
	    }
	    if(key->space==TRUE && ship_c->gas > 2){
	      if (!ship_c->weapon->cont1){
		ChooseWeapon(ship_c);
		if(proc==players[ship_c->player].proc){
		  if(FireCannon(&listheadobjs,ship_c,NULL)==0){
		    Play(ship_c,FIRE0,1);
		  }
		}
	      }
	    }
	  }
	}
      }    
    }/* if(ship_c!=NULL) */
    
    if(cv!=NULL){
      if(key->n==TRUE){
	if(nav_mode==RELATIVE){
	  nav_mode=ABSOLUTE;
	  r_rel.x=cv->x;
	  r_rel.y=cv->y;
	}
	else{
	  nav_mode=RELATIVE;
	}
	key->n=FALSE;
      }
      
      if(actual_player==actual_player0){ /* PRODUCTION */
	
	if(key->i==TRUE || key->esc==TRUE){
	  if(GameParametres(GET,GQUIT,0)==0){
	    ship_c=cv;
	    ship_c->ai=1;
	    key->a=FALSE;
	    key->i=FALSE;
	    key->esc=FALSE;
	  }
	}
	
	if(key->a==TRUE){
	  ship_c=cv;
	  ship_c->ai=0;
	  key->a=FALSE;
	  key->i=FALSE;
	  key->esc=FALSE;
	}

	if(key->number[1]==TRUE){
	  if(cv->weapon0.type!=CANNON0)
	    cv->weapon=&cv->weapon0;    
	  key->number[1]=FALSE;
	}
	if(key->number[2]==TRUE){
	  if(cv->weapon1.type!=CANNON0)
	    cv->weapon=&cv->weapon1;    
	  key->number[2]=FALSE;
	}
	if(key->number[3]==TRUE){
	  if(cv->weapon1.type!=CANNON0)
	    cv->weapon=&cv->weapon2;    
	  key->number[3]=FALSE;
	}
      }
    }
    
    break;
  case TRUE: /* order mode */
    //    key->m=FALSE;
    break;
  default:
    break;
  }

  if(!(key->Pagedown | key->Pageup | key->home | key->tab)){
    swtab=0;
    swtab0=0;
  }

  if(key->Pagedown|key->Pageup|key->home|key->tab){
    Shell(0,NULL,NULL,NULL,NULL,NULL,NULL,NULL); /* reset shell() */
    key->o=FALSE; //aqui01 salir de order mode
    gordermode=FALSE;
    key->p=FALSE;
    
    if(key->tab==TRUE){
      if(swtab==0){
	if(cv!=NULL)cv->selected=FALSE;
	if(key->ctrl==TRUE){
	  cv=PrevCv(&listheadplayer,cv,actual_player);
	}
	else{
	  cv=NextCv(&listheadplayer,cv,actual_player);
	}
      }
      swtab++;
      //      if(swtab>3)swtab=0;  
      if(swtab0)if(swtab>1)swtab=0;  
      if(swtab>6){swtab=0;swtab0=1;}  
    }
  
    if(key->Pagedown|key->Pageup|key->home){
      if(swtab==0){
	if(cv!=NULL)cv->selected=FALSE;
	if(key->Pagedown==TRUE){
	  cv=NextPlanetCv(&listheadplayer,cv,actual_player);
	  key->Pagedown=FALSE;
	}
	if(key->Pageup==TRUE){
	  cv=PrevPlanetCv(&listheadplayer,cv,actual_player);
	  key->Pageup=FALSE;
	}
	if(key->home==TRUE){
	  key->home=FALSE;
	  cv=FirstShip(&listheadplayer,cv,actual_player);
	}
      }
      swtab++;
      if(swtab0)if(swtab>1)swtab=0;  
      if(swtab>6){swtab=0;swtab0=1;}  
    }

    if(cv!=NULL){
      habitat.type=cv->habitat;
      habitat.obj=cv->in;
      DrawSelectionBox(&cv,1);
      cv->selected=TRUE;
      
      if(nav_mode==ABSOLUTE){
	while(cv->x - r_rel.x>GameParametres(GET,GWIDTH,0)/2){
	  r_rel.x+=GameParametres(GET,GWIDTH,0);
	}
	while(cv->x - r_rel.x <-GameParametres(GET,GWIDTH,0)/2){
	  r_rel.x-=GameParametres(GET,GWIDTH,0);
	}
	while(cv->y - r_rel.y>GameParametres(GET,GHEIGHT,0)/2){
	  r_rel.y+=GameParametres(GET,GHEIGHT,0);
	}
	while(cv->y - r_rel.y <-GameParametres(GET,GHEIGHT,0)/2){
	  r_rel.y-=GameParametres(GET,GHEIGHT,0);
	}
      }
    }
  }

  /* save and load game */
  if(key->ctrl==TRUE && key->s==TRUE){
    if(GameParametres(GET,GMODE,0)==CLIENT){
      key->save=FALSE;
    }
    else{
      key->save=TRUE;
      key->s=FALSE;
    }
  }

  if(key->ctrl==TRUE && key->l==TRUE){

    if(GameParametres(GET,GMODE,0)==CLIENT){
      key->load=FALSE;
    }
    else{
      key->load=TRUE;
      key->l=FALSE;  
    }
  }
  /*-- save and load game */
}



void UpdateShip(Object *obj){
  /* 
     Calculate the new coordinates of the ship *obj

  */
  float vx,vy,vmax2;
  float cosa,sina;
  float rx,ry;
  float vr,vo;
  float fx,fy;
  float fx0,fy0;
  float dtim;
  float a,factor;
  float g=1.2/250000;
  float U;
  int time;
  int n;

  int proc,gwidth;


#if DEBUG
  /*  printf("UpdateShip()\n"); */
#endif

  proc=GetProc();

  vx=obj->vx;
  vy=obj->vy;
  dtim=DT/obj->mass;
  
  fx0=fy0=0;

  if(0&&obj==cv){
    printf("mode: %d\n",obj->mode);
  }

  if(obj->habitat==H_SPACE){
    U=PlanetAtraction(&fx0,&fy0,obj->x,obj->y,obj->mass);
    if(0&&obj==cv){
      printf("P.A. f=%f U=%f\n",sqrt(fx0*fx0+fy0*fy0),U);
    }
    
    if(obj->subtype==SATELLITE){
      if(fx0>0.0000001){
	obj->life=2400;
      }
    }
  }
  else{
      if(obj->in!=NULL){
	fy0=-g*obj->mass*(float)obj->in->mass;
	if(0&&obj==cv){
	  printf("P.A. in f=%f\n",fy0);
	}
      }
      else{
	printf("in = NULL in id: %d\n",obj->id);
	return;
      }
  }

  if(0&&obj==cv){
    printf("P.A. f=%f %f %f accel: %f\n",sqrt(fx0*fx0+fy0*fy0),fx0,fy0,obj->accel);
  }

  obj->x0=obj->x;
  obj->y0=obj->y;

  if(obj->mode==NAV){
    obj->x+=obj->vx*DT+.5*(fx0)*DT*dtim;
    obj->y+=obj->vy*DT+.5*(fy0)*DT*dtim;
  }

  fx=fy=0;
  if(obj->habitat==H_SPACE){
    PlanetAtraction(&fx,&fy,obj->x,obj->y,obj->mass);
  }
  else{
    //    if(obj->mode==NAV){
      fy=-g*obj->mass*(float)obj->in->mass;
      //  }
  }

  if(proc==players[obj->player].proc){
    if(obj->engine.a_max && obj->accel>0){
      obj->gas-=10*obj->engine.gascost*obj->accel/obj->engine.a_max;
      if(obj->gas<0){
	obj->gas=0;
	obj->accel=0;
      }
    }
    if(obj->engine.ang_a_max && obj->ang_a!=0){
      obj->gas-=fabs(obj->engine.gascost*obj->ang_a/obj->engine.ang_a_max);
      if(obj->gas<0){
	obj->gas=0;
	obj->ang_a=0;
      }
    }
  }
  if(obj->accel){
    vx+=(.5*(fx+fx0)+cos(obj->a)*obj->accel)*dtim;
    vy+=(.5*(fy+fy0)+sin(obj->a)*obj->accel)*dtim;
  }
  else{
    vx+=(.5*(fx+fx0))*dtim;
    vy+=(.5*(fy+fy0))*dtim;
  }

  if(obj->mode==LANDED && vy>0){
    obj->mode=NAV;
  } 
  if(obj->mode==LANDED && vy<0){
    vy=0;
    vx=0;
  } 


  /* if max vel is reached, reescaling */

  if(obj->type==SHIP){
    vmax2=obj->engine.v2_max*(1-0.4375*(obj->state<25));
  }
  else{
    vmax2=obj->engine.v2_max;
  }
  if(vx*vx+vy*vy>vmax2){
    factor=sqrt(vmax2/(vx*vx+vy*vy));
    if(factor>1){
      fprintf(stderr,"ERROR factor>1\n");
      printf("\tobj_enginev2max: %d %d\n",obj->engine.v2_max,(obj->state<25));
    }
    vx*=factor;
    vy*=factor;
  }
  
  if(0&&obj==cv){
    printf("vx: %f vy: %f vmax: %d\n",obj->vx,obj->vy,obj->engine.v_max);
  }

  if(obj->ang_a!=0){
    obj->ang_v+=obj->ang_a*100.*dtim;
    if(obj->ang_v > obj->engine.ang_v_max)obj->ang_v=obj->engine.ang_v_max;
    if(obj->ang_v < -obj->engine.ang_v_max)obj->ang_v=-obj->engine.ang_v_max;
  }
  else{
    obj->ang_v*=0.5;
  }
  
  obj->a+=(obj->ang_v+0.5*obj->ang_a*100.*dtim)*DT; 
  
  if(obj->a > PI)obj->a-=2*PI;
  if(obj->a < -PI)obj->a+=2*PI;


  if(0&&obj==ship_c){
    printf("ang: a: %f v:%f\n",obj->ang_a,obj->ang_v);
  }
  
  obj->vx=vx;
  obj->vy=vy;
  obj->fx0=obj->fx;
  obj->fy0=obj->fy;
  
  if(obj->weapon0.cont1)obj->weapon0.cont1--;
  if(obj->weapon1.cont1)obj->weapon1.cont1--;
  if(obj->weapon2.cont1)obj->weapon2.cont1--;

  if(proc==players[obj->player].proc){
    
    if(obj->type==SHIP && obj->habitat==H_SPACE){

      obj->gas+=0.05; // 0.05

      if(obj->gas > obj->gas_max)obj->gas=obj->gas_max; 
      if(0&&obj==cv)printf("update ship. gas: %f %f\n",obj->gas,obj->accel);
      if(obj->state>0 && obj->state<25){
	obj->state+=0.005;
      }
    }


    if(obj->type==SHIP && obj->mode==LANDED){
      time=GetTime();
      /* repair and refuel  */
      if(players[obj->player].gold<5){
	n=5*Random(-1);
      }
      else{
	n=0;
      }

      if(players[obj->player].gold>0 && n==0){
	if(obj->state<100){
	  if(obj->state>0){
	    obj->state+=0.05;
	  }
	  if(obj->state>100)obj->state=100;
	  players[obj->player].gold-=.125; /* total cost 250  */
	  
	}
	if(obj->gas < obj->gas_max){
	  obj->gas+=2;
	  if(obj->gas>obj->gas_max)obj->gas=obj->gas_max;
	  players[obj->player].gold-=.2; /* total cost 100 */
	  
	}
	/* if(cv==obj){ */
	/*   printf("womax:%d\n",obj->weapon0.max_n); */
	/* } */
	if(!(time%4)){
	  if(obj->weapon0.n<obj->weapon0.max_n){
	    if(players[obj->player].gold>obj->weapon0.projectile.unitcost){
	      obj->weapon0.n++;
	      players[obj->player].gold-=obj->weapon0.projectile.unitcost;
	    }
	  }
	}
	if(!(time%240)){
	  if(obj->weapon1.n<obj->weapon1.max_n){
	    if(players[obj->player].gold>obj->weapon1.projectile.unitcost){
	      players[obj->player].gold-=obj->weapon1.projectile.unitcost;
	      obj->weapon1.n++;
	    }
	  }
	}
	if(!(time%14)){
	  if(obj->weapon2.n<obj->weapon2.max_n){
	    if(players[obj->player].gold>obj->weapon2.projectile.unitcost){
	      players[obj->player].gold-=obj->weapon2.projectile.unitcost;
	      obj->weapon2.n++;
	    }
	  }
	}
      }
      /* Learning Experience */
      
      if(time - obj->cdata->tmlevel>50){
	int mlevel=0;
	mlevel=NearMaxLevelObj(obj,&listheadcontainer[obj->in->id]);

	obj->cdata->tmlevel=time;
	obj->cdata->mlevel=mlevel;
      }
      if(obj->cdata->mlevel - obj->level > 2){
	float factor=0.02;
	Experience(obj,(obj->cdata->mlevel - obj->level)*factor);
      }

    }  /*if(obj->mode==LANDED) */

  }
  if(obj->habitat==H_PLANET){
    gwidth=GameParametres(GET,GWIDTH,0);
    gwidth=LXFACTOR;
    if(obj->x<0)obj->x+=gwidth;
    if(obj->x>gwidth)obj->x-=gwidth;

    /* if(obj->y>GameParametres(GET,GHEIGHT,0)){  *//* its out of planet */
    if(obj->y>LYFACTOR){ /* its out of planet */
      if(proc==players[obj->player].proc){
	a=obj->x*(2*PI)/gwidth-PI;
	cosa=cos(a);
	sina=sin(a);
	obj->x=obj->in->planet->x+2*obj->in->planet->r*cosa;
	obj->y=obj->in->planet->y+2*obj->in->planet->r*sina;
	
	vo=-obj->vx;
	vr=obj->vy;
	obj->vx=vr*cosa-vo*sina;
	obj->vy=vr*sina+vo*cosa;
	
	rx=obj->x - obj->in->planet->x;
	ry=obj->y - obj->in->planet->y;
	
	a=atan2(ry,rx);
	
	/*      obj->a=obj->a-a+PI/2;  */
	obj->a=a-PI/2+obj->a;
	
	obj->habitat=H_SPACE;
	obj->planet=NULL;
	obj->in=NULL;
	if(GameParametres(GET,GNET,0)==TRUE){
	  SetModified(obj,SENDOBJAALL);  /*on getting out of planet */
	  obj->ttl=0;
	}
	if(obj==cv){
	  habitat.type=H_SPACE;
	  habitat.obj=NULL;
	}
      }
    }
  }
  
#if DEBUG
  if(!(GetTime()%50))
    printf("Ec:%f U:%f E:%f\n",.5*obj->mass*(vx*vx+vy*vy),
	   U,.5*obj->mass*(vx*vx+vy*vy)+U);
#endif


#if DEBUG
  /*  printf("--UpdateShip()\n"); */
#endif

  return;
}

void UpdateAsteroid(Object *obj){
  /* 
     Calculate the new coordinates of the asteroid *obj
     Only gravitational forces.
  */

  float vx,vy,vmax2;
  float cosa,sina;
  float rx,ry;
  float vr,vo;
  float fx,fy;
  float fx0,fy0;
  float dtim;
  float a,factor;
  float g=1.2/250000;
  float U;
  int proc,gwidth;


#if DEBUG
  /*  printf("UpdateAsteroid()\n"); */
#endif

  if(obj->type!=ASTEROID)return;

  proc=GetProc();

  vx=obj->vx;
  vy=obj->vy;
  dtim=DT/obj->mass;
  
  fx0=fy0=0;

  if(obj->habitat==H_SPACE){
    U=PlanetAtraction(&fx0,&fy0,obj->x,obj->y,obj->mass);
  }
  else{
    if(obj->mode==NAV){
      if(obj->in!=NULL){
	fy0=-g*obj->mass*(float)obj->in->mass;
      }
      else{
	printf("in = NULL in id: %d\n",obj->id);
	return;
      }
    }
  }

  obj->x0=obj->x;
  obj->y0=obj->y;
 
  obj->x+=obj->vx*DT+.5*fx0*DT*dtim;
  obj->y+=obj->vy*DT+.5*fy0*DT*dtim;
  
  fx=fy=0;
  if(obj->habitat==H_SPACE){
    PlanetAtraction(&fx,&fy,obj->x,obj->y,obj->mass);
  }
  else{
    fy=-g*obj->mass*(float)obj->in->mass;
  }

  vx+=.5*(fx+fx0)*dtim;
  vy+=.5*(fy+fy0)*dtim;

  /* if max vel is reached, reescaling */
  
  vmax2=(VELMAX*VELMAX/16.0);

  if(vx*vx+vy*vy>vmax2){
    factor=sqrt(vmax2/(vx*vx+vy*vy));
    if(factor>1){
      fprintf(stderr,"ERROR factor>1\n");
      printf("\tobj_enginev2max: %d %d\n",obj->engine.v2_max,(obj->state<25));
    }
    vx*=factor;
    vy*=factor;
  }

  obj->a+=DT*obj->ang_v; 

  if(obj->a > PI)obj->a-=2*PI;
  if(obj->a < -PI)obj->a+=2*PI;

  obj->vx=vx;
  obj->vy=vy;
  obj->fx0=obj->fx;
  obj->fy0=obj->fy;

  if(obj->habitat==H_PLANET){
    gwidth=GameParametres(GET,GWIDTH,0);

    if(obj->x<0)obj->x+=gwidth;
    if(obj->x>gwidth)obj->x-=gwidth;

    if(0 && obj->y>GameParametres(GET,GHEIGHT,0)){ /* its out of planet */
      if(proc==players[obj->player].proc){
	a=obj->x*(2*PI)/gwidth-PI;
	cosa=cos(a);
	sina=sin(a);
	obj->x=obj->in->planet->x+2*obj->in->planet->r*cosa;
	obj->y=obj->in->planet->y+2*obj->in->planet->r*sina;
	
	vo=-obj->vx;
	vr=obj->vy;
	obj->vx=vr*cosa-vo*sina;
	obj->vy=vr*sina+vo*cosa;
	
	rx=obj->x - obj->in->planet->x;
	ry=obj->y - obj->in->planet->y;
	
	a=atan2(ry,rx);
	
	/*      obj->a=obj->a-a+PI/2;  */
	obj->a=a-PI/2+obj->a;
	
	obj->habitat=H_SPACE;
	obj->planet=NULL;
	obj->in=NULL;
	if(GameParametres(GET,GNET,0)==TRUE){
	  if(GetProc()==players[obj->player].proc){
	    SetModified(obj,SENDOBJAALL);  /*al salir del planeta */
	    obj->ttl=0;
	  }
	}
	if(obj==cv){
	  habitat.type=H_SPACE;
	  habitat.obj=NULL;
	}
      }
    }
  }
  
#if DEBUG
  if(!(GetTime()%50))
    printf("Ec:%f U:%f E:%f\n",.5*obj->mass*(vx*vx+vy*vy),
	   U,.5*obj->mass*(vx*vx+vy*vy)+U);
#endif

#if DEBUG
  /*  printf("--UpdateAsteroid()\n"); */
#endif

  return;
}


void Collision(struct HeadObjList *lh){
  /*
    version 02.(200211) without CollisionList
    Calculate the collision between objects 
  */  

  struct ObjList *ls1,*ls2;
  Object *obj1,*obj2,*objt1,*objt2;
  Object *obj,*pnt,*shot;
  Object *nobj;
  struct Planet *planet;
  Segment s;
  float radio2,r,r2,rx,ry;
  float radar1,radar2;
  float r02,r0x,r0y;
  float damage;
  float v;
  float a,b;
  char text[TEXTMENMAXLEN];  
  int i,j;
  int gwidth,gheight,gkplanets,gnet,gnplayers;
  int proc;
  int crashsw=0;
  static int cont=0;


#if DEBUG
  printf("Collision()\n");
#endif

  gwidth=GameParametres(GET,GWIDTH,0);
  gheight=GameParametres(GET,GHEIGHT,0);
  gkplanets=GameParametres(GET,GKPLANETS,0);
  gnet=GameParametres(GET,GNET,0);
  gnplayers=GameParametres(GET,GNPLAYERS,0);

  proc=GetProc();

  /* among objs and ship */
  ls1=lh->next;

  /* among objs */
  while(ls1!=NULL){ 

    obj1=ls1->obj;

    if(0&&obj1->type==PLANET){
	printf("PLANET: %d %d\n",obj1->id,players[obj1->player].team);
    }

    if(obj1->state<=0){
      contabilidad[0]++;
      ls1=ls1->next;
      continue;
    }

    switch(obj1->type){
    case SHIP:
      if(0&&obj1->cdata!=NULL){
	if(obj1->cdata->obj[0]==NULL){
	  //	printf("NULL\n");
	  ls1=ls1->next;continue;
	}
      }
      
      if(gnet==TRUE && obj1->ttl<MINTTL){ls1=ls1->next;continue;}
#if CELLON
      if(0&&obj1->habitat==H_SPACE){
	if(ValueCell(cell,obj1)==1<<(players[obj1->player].team+2)){
	  printf(".");	 
	  ls1=ls1->next;continue;
	}
      }
#endif
      break;
    case PROJECTILE:
      if(obj1->subtype==EXPLOSION){ 
	ls1=ls1->next;continue; 
      }
      break;
    case TRACKPOINT:
    case TRACE:
      contabilidad[1]++;
      ls1=ls1->next;
      continue;
      break;
    default:
      break;
    }
    
    radar1=obj1->radar;

    ls2=ls1->next;

    if(obj1->type==PLANET){
      while(ls2!=NULL && ls2->obj->type==PLANET){
	ls2=ls2->next;
      }
    }
    while(ls2!=NULL){  /* double loop */
      if(obj1->state<=0){
	contabilidad[0]++;
	ls2=ls2->next;
	continue;
      }
      obj2=ls2->obj;

/*       if(obj1->subtype==EXPLOSION && obj2->subtype==EXPLOSION){ */
/* 	ls2=ls2->next;continue; */
/*       } */

/*       if(obj2->type==PROJECTILE && obj2->subtype==EXPLOSION){ */
/* 	ls2=ls2->next;continue; */
/*       } */


      

      switch(obj2->type){
      case SHIP:
	if(gnet==TRUE){
	  if(obj2->ttl<MINTTL){ls2=ls2->next;continue;}
	}
	if(0&&obj2->cdata!=NULL){
	  if(obj2->cdata->obj[0]==NULL){
	    //	printf("NULL\n");
	    ls2=ls2->next;continue;
	  }
	}

	break;
      case PROJECTILE:
	if(obj1->type==PROJECTILE){
	  /* missiles collides with the others projectiles */ 
	  if(obj1->subtype!=MISSILE && obj2->subtype!=MISSILE){
	    contabilidad[6]++;
	    ls2=ls2->next;continue;
	  }
	}
	break;
      case PLANET:
	if(obj1->type==PLANET){
	  contabilidad[2]++;
	  ls2=ls2->next;continue;
	}
	break;
      case ASTEROID:
	if(obj1->type==ASTEROID){
	  ls2=ls2->next;continue;
	}
	break;
      case TRACKPOINT:
      case TRACE:
	contabilidad[10]++;
	ls2=ls2->next;continue;
	break;
      default:
	break;
      }

      if(obj1->player==obj2->player){ /* same player, two ships or shots */
	if((obj1->type!=PLANET && obj2->type!=PLANET)){
	  ls2=ls2->next;
	  contabilidad[3]++;
	  continue;
	}
      }

      if(obj2->state<=0){
	contabilidad[4]++;
	ls2=ls2->next;
	continue;
      }

      if(obj1->in!=obj2->in){
	ls2=ls2->next;
	contabilidad[5]++;
	continue;
      }
      if(obj1->parent==obj2 || obj2->parent==obj1){
	ls2=ls2->next;
	contabilidad[7]++;
	continue;
      }
      if(gnet==TRUE){
	if(players[obj1->player].proc==players[obj2->player].proc){
	  if(proc!=players[obj1->player].proc){
	    ls2=ls2->next;
	    contabilidad[8]++;
	    continue;
	  }
	}
      }

      if(players[obj1->player].team==players[obj2->player].team){
	if(!(obj1->type==PLANET || obj2->type==PLANET)){
	  contabilidad[9]++;
	  ls2=ls2->next;
	  continue;
	}
      }

      contabilidad[11]++;
      radar2=obj2->radar;
      radio2=(obj1->radio+obj2->radio)*(obj1->radio+obj2->radio);
      
      rx=obj2->x - obj1->x;
      ry=obj2->y - obj1->y;
      r2=rx*rx+ry*ry;
      
      r0x=obj2->x0 - obj1->x0;
      r0y=obj2->y0 - obj1->y0;
      r02=0.25*((rx+r0x)*(rx+r0x)+(ry+r0y)*(ry+r0y));


      /* list of near enemy ships */
      if(0){/* HERE in another routine */
	if(obj2->player!=obj1->player){
	  if(obj1->player==actual_player && obj1->type==SHIP){
	    if(obj2->player==SHIP && obj2->habitat==H_SPACE){
	      if(r2<radar1*radar1){
		Add2ObjList(&listheadnearobjs,obj2);
	      }
	    }
	    if(obj2->player==ASTEROID && obj2->habitat==H_SPACE){
	      Add2ObjList(&listheadnearobjs,obj2);
	    }
	  }
	}
      }
      /* --list of near enemy objects */

      /* Experience in combat, among SHIPS */
      if(r2<1000000){      
	if(obj1->type==SHIP && obj2->type==SHIP){
	  if(players[obj2->player].team!=players[obj1->player].team){
	    if(obj1->habitat==obj1->habitat){
	      int il;
	      float factor,points;
	      /* obj1 */
	      if(proc==players[obj1->player].proc){
		il=obj2->level-obj1->level;
		factor=0.001;
		if(il==-1)factor/=2;
		if(il<-1)factor=0;
		if(factor>0){
		  points=(il+2)*(il+2)*factor*(obj2->level+1);
		  Experience(obj1,points);
		}
	      }
	      /* obj2 */
	      if(proc==players[obj2->player].proc){
		il=obj1->level-obj2->level;
		factor=0.001;
		if(il==-1)factor/=2;
		if(il<-1)factor=0;
		if(factor>0){
		  points=(il+2)*(il+2)*factor*(obj1->level+1);
		  Experience(obj2,points);
		}
	      }
	    }
	  }
	}
      }
    /* --Experience in combat */

      /* planets and SHIPS */
      if(gkplanets==FALSE){
	/*	if(r2<RADAR_RANGE2){  planet  discovered  */
	if((obj1->type==PLANET && obj2->type==SHIP) ||
	   (obj2->type==PLANET && obj1->type==SHIP)){

	  if(r2<radar1*radar1 || r2<radar2*radar2){ /* planet  discovered */	    
	    if(obj1->type==PLANET){
	      obj=obj2;
	      pnt=obj1;
	    }
	    else{
	      obj=obj1;
	      pnt=obj2;
	    }
	    
	    if(!((cont+obj->id)%20)){
	      if(r2<obj->radar*obj->radar){

		if(IsInIntList((players[obj->player].kplanets),pnt->id)==0){
		  for(i=0;i<=gnplayers+1;i++){
		    if(players[obj->player].team==players[i].team){
		      players[i].kplanets=Add2IntList((players[i].kplanets),pnt->id);;
		      snprintf(text,TEXTMENMAXLEN,"(%d) PLANET %d discovered",obj->pid,pnt->id);
		      Add2TextMessageList(&listheadtext,text,obj->id,obj->player,0,100,0);
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
      /* --planets and SHIPS */


      if(r2<radio2 || r02<radio2){
	contabilidad[12]++;
	if(obj1->type==PROJECTILE || obj2->type==PROJECTILE){
	  if(obj1->subtype==LASER || obj2->subtype==LASER){
	    if(obj1->subtype==LASER){
	      obj=obj2;
	      shot=obj1;
	    }
	    else{
	      obj=obj1;
	      shot=obj2;
	    }
	    r=(obj->x-shot->x)*sin(shot->a)-(obj->y-shot->y)*cos(shot->a);
	    r2=r*r;
	    if(r2>obj->radio*obj->radio){
	      ls2=ls2->next;
	      continue;
	    }
	  }
	}

	/*
	   objects and planets 
	*/

	if((obj1->type==PLANET && obj2->type!=PLANET) ||
	   (obj2->type==PLANET && obj1->type!=PLANET)){

	  /* inner planet */
	  if(obj1->type==PLANET){
	    obj=obj2;
	    pnt=obj1;
	  }
	  else{
	    obj=obj1;
	    pnt=obj2;
	  }
	  if(proc==players[obj->player].proc){
	    obj->habitat=H_PLANET;
	    obj->planet=NULL;
	    obj->in=pnt;
	    
	    /*entering planet */
	    if(gnet==TRUE){
	      if(proc==players[obj->player].proc){
		SetModified(obj,SENDOBJAALL);
		obj->ttl=0;
	      }
	    }
	    if(obj==cv){
	      habitat.type=H_PLANET;
	      habitat.obj=pnt;
	    }
	    /* initial conditions at planet */
	    a=atan2(-pnt->y+obj->y,-pnt->x+obj->x);
	    b=atan2(obj->vy,obj->vx)-a;
	    obj->x=(a+PI)/(2*PI)*LXFACTOR;
	    obj->y=LYFACTOR;
	    v=sqrt(obj->vx*obj->vx+obj->vy*obj->vy);
	    
	    obj->vx=-v*sin(b);
	    obj->vy=v*cos(b);
	    
	    obj->a=obj->a-a+PI/2;
	    if(obj->a > PI)obj->a-=2*PI;
	    if(obj->a < -PI)obj->a+=2*PI;
	    if(obj->actorder.id!=-1)obj->actorder.time=0;
	  }
	}

	/*  --objects and planets */


	/* among ships and projectiles */
	if((obj1->type!=PLANET && obj2->type!=PLANET)){

	  for(i=0;i<2;i++){
	    if(i==0){
	      objt1=obj1;
	      objt2=obj2;
	    }
	    if(i==1){
	      objt1=obj2;
	      objt2=obj1;
	    }


	    //if(proc==players[objt1->player].proc){ // no puntuan asteroides
	    damage=objt2->damage*(1-objt1->shield);
	    
	    //	      if(proc==players[objt1->player].proc){// no puntuan asteroides
	    objt1->state-=damage;
	    //	      }
	    
	    Play(objt1,CRASH,1);
	    if(objt1==cv)gdraw.crash=1;
	    if(objt1->type==SHIP){
	      /*receive an impact */
	      if(objt1->state>0){
		if(proc==players[objt1->player].proc){
		  Experience(objt1,damage/2); /* experience for receive an impact*/
		}
		
		if(objt1->player==actual_player && cv!=objt1){
		  snprintf(text,TEXTMENMAXLEN,"(%c %d) HELP",Type(objt1),objt1->pid);
		  Add2TextMessageList(&listheadtext,text,objt1->id,objt1->player,0,100,2);
		}
	      }
	    }
	    if(objt1->type==SHIP || objt1->type==ASTEROID || objt1->subtype==MISSILE){//HERE
	      if(gnet==TRUE){
		if(proc==players[objt1->player].proc){
		  SetModified(objt1,SENDOBJAALL);
		  objt1->ttl=0;
		}
	      }
	    }
	    
	    if(objt1->state<=0){
	      //	      printf("obj %d dead by %d\n",objt1->id,objt2->id);
	      if(obj1->in!=NULL && objt1->type==SHIP){
		float price;
		
		price=0.025*GetPrice(objt1,0,0,0);
		if(price>0){
		  objt1->in->planet->gold+=price;
		  /* printf("ship (L%d) destroyed at planet  %d gold: %f price:%f\n", */
		  /* 	 obj1->level,objt1->in->id,objt1->in->planet->gold,price); */
		}
	      }


	      if(objt2->type==SHIP){
		objt1->sw=objt2->id;
	      }
	      if(objt2->type==PROJECTILE){
		if(objt2->parent!=NULL){
		  objt1->sw=objt2->parent->id;
		}
	      }

		
	      if(gnet==TRUE){
		if(proc==players[objt1->player].proc){
		  SetModified(objt1,SENDOBJKILL);
		  objt1->ttl=0;
		}
	      }
	      switch(objt1->type){
	      case SHIP:
		break;
	      case ASTEROID:
/* 		if(objt1->in!=NULL){ */
/* 		  objt1->in->planet->gold+=(4-objt1->subtype)*200; */
/* 		} */
		if(objt1->subtype<ASTEROID3){
		  if(proc==players[objt1->player].proc){
		    for(j=0;j<3;j++){
		      nobj=NewObj(&listheadobjs,ASTEROID,objt1->subtype+1, 
				  objt1->x,objt1->y,objt1->vx+10.0*rand()/RAND_MAX-5,objt1->vy+10.0*rand()/RAND_MAX-5, 
				  CANNON0,ENGINE0,0,NULL,objt1->in); 
		      nobj->a=PI/2;
		      nobj->ai=0;
		      nobj->in=objt1->in;
		      nobj->habitat=objt1->habitat;
		      nobj->mode=NAV;
		      Add2ObjList(&listheadobjs,nobj);
		    }
		  }
		}
		break;
	      default:
		break;
	      }
	    }
	  } /* for(i=0;i<2;i++){ */
	}
	/* --among ships and projectiles */
      }     /*  if(r2<radio2 || r02<radio2){ */
      else{
	if(r2<4*radio2 && 0){
	  /* Avoid collision */ /* TODO */


	}
      }

      ls2=ls2->next;
    } /* while ls2!=NULL */ /* --double loop*/ 

#if DEBUG
    if(keys.d==TRUE && obj1==cv){
      printf("======\n");
      PrintfObjInfo(stdout,cv);
      printf("obj1:%p  cv:%p\n",obj1,cv); 
     printf("======\n");
    }
#endif

    /* collision with planet terrain */
    if(obj1->mode!=LANDED && obj1->habitat==H_PLANET){
      if(proc==players[obj1->player].proc || obj1->type==PROJECTILE){
	planet=obj1->in->planet;
	contabilidad[13]++;
	crashsw=0;
	if(!GetSegment(&s,obj1)){
	  /* if(obj1->y - obj1->radio +1< s.y0 && obj1->y - obj1->radio +1< s.y1){ */
	  if(obj1->y < s.y0 && obj1->y < s.y1){
	    if(obj1->type!=SHIP){ /* destroy object */
	      obj1->state=0;
	      obj1->sw=0;
	      
	      if(obj1->type==ASTEROID){
		planet->gold+=100*pow(2,5-obj1->subtype);
		//	printf("asteroid crashed at planet %d gold: %f\n",obj1->in->id,obj1->in->planet->gold);
	      }
	      if(gnet==TRUE){
		if(proc==players[obj1->player].proc){
		  SetModified(obj1,SENDOBJKILL);
		  SetModified(obj1->in,SENDOBJPLANET); //HERE
		  obj1->ttl=0;
		}
	      }
	    }
	    if(obj1->type==SHIP){
	      switch(s.type){
	      case TERRAIN:  /* destroy ship */
		crashsw=1;
		break;
	      case LANDZONE:
		if(fabs(obj1->vx)>2 || /* ship crashed */
		   fabs(obj1->vy)>5 ||
		   fabs(obj1->a-PI/2)>.35){
		  crashsw=1;
		}
		else{ /* ship has landed */
		  if(obj1->vy<0){
		    obj1->mode=LANDED;
		    if(gnet==TRUE){
		      if(proc==players[obj1->player].proc){
			SetModified(obj1,SENDOBJAALL);
			obj1->ttl=0;
		      }
		    }
		    //		    obj1->accel=0;		    
		    obj1->vx=0;
		    obj1->vy=0;
		    obj1->ang_a=0;
		    obj1->ang_v=0;
		    /* obj1->y=obj1->y0=s.y0+obj1->radio+1; */
		    obj1->y=obj1->y0=s.y0;
		  }
		  
		  if(players[obj1->in->player].team!=players[obj1->player].team){
		    char text[TEXTMENMAXLEN];
		    if(players[obj1->in->player].team != 1){
		      GetInformation(&players[obj1->player],&players[obj1->in->player],obj1);
		    }
		    Experience(obj1,50);  /* Experience for conquest a planet */
		    
		    snprintf(text,TEXTMENMAXLEN,"Planet %d LOST",obj1->in->id);
		    Add2TextMessageList(&listheadtext,text,obj1->in->id,obj1->in->player,0,100,2);
		    
		    obj1->in->player=obj1->player;
		    if(gnet==TRUE){
		      if(proc==players[obj1->player].proc){
			SetModified(obj1->in,SENDOBJPLANET);
			players[obj1->player].modified=SENDPLAYERMOD;
			obj1->ttl=0;
		      }
		    }
		  }
		  else{
		    if(PlanetEmpty(obj1->in,obj1)){
		      obj1->in->player=obj1->player;
		      if(gnet==TRUE){
			if(proc==players[obj1->player].proc){
			  SetModified(obj1->in,SENDOBJPLANET);
			  players[obj1->player].modified=SENDPLAYERMOD;
			  obj1->ttl=0;
			}
		      }
		    }
		  }
		}
		break;
	      default:
		break;
	      }
	      
	      if(crashsw){
		float price,factor;

		factor=0.01*obj1->state*0.25;
		if(factor<.1)factor=.1;  /* at least an 10 percent*/
		price=factor*GetPrice(obj1,0,0,0);
		if(price>0){
		  planet->gold+=price;
#if DEBUG
		  if(debugcrash){
		    printf("ship (L%d) crash at planet C %d gold: %f price:%f\n",
			   obj1->level,obj1->in->id,planet->gold,price);
		  }
#endif
		}
		
		obj1->state=0;
		obj1->sw=0;
		
		if(gnet==TRUE){
		  if(proc==players[obj1->player].proc){
		    SetModified(obj1,SENDOBJKILL);
		    SetModified(obj1->in,SENDOBJPLANET);
		    obj1->ttl=0;
		  }
		}
		crashsw=0;
	      }
	    }/* if SHIP*/
	  } /* if(obj1->y - obj1->radio < s.y0 && obj1->y - obj1->radio < s.y1){ */
	} /* if GetSegment()*/
      } /*      if(obj1->mode!=LANDED && obj1->habitat==H_PLANET ) */
    }
    /* --collision with planet terrain */
    
    ls1=ls1->next;
  } /* while(ls1!=NULL) */

  cont++;
  if(cont>10000)cont=0;
  return;
}

int UpdateObjs(void){
  /*
    Update the coordinates of the objects.
  */  
  
  int n=0;
  struct ObjList *ls;
  Object *obj,*nobj,*ship_enemy;
  float d2;
  float rx,ry,b,ib;  
  int gnet;
  int proc;

#if CLOSEDUNIVERSE 
  int gulx,guly;
  gulx=GameParametres(GET,GULX,0);
  guly=GameParametres(GET,GULY,0);

#endif
#if DEBUG
  printf("UpdateObjs()\n");
#endif
  
  gnet=GameParametres(GET,GNET,0);
  proc=GetProc();

  ls=listheadobjs.next;
  
  if(listheadobjs.n==0)return(0);
  
  while(ls!=NULL){
    obj=ls->obj;
    //    obj->sw=0;
    
    if(obj->durable==TRUE){
      
      obj->life--;
      if(obj->life<=0){
	obj->life=0;
	obj->state=0;
	obj->sw=0;
      }
    }
    
    if(obj->life>0){/* && proc==players[obj->player].proc){ */
      
      switch(obj->type){
      case PROJECTILE:
	switch(obj->subtype){
	case MISSILE:
	  ship_enemy=NearestObj(&listheadobjs,obj,SHIP,PENEMY,&d2);
	  if(ship_enemy!=NULL){
	    if(ship_enemy->habitat!=obj->habitat)ship_enemy=NULL;
	    if(d2>obj->radar*obj->radar)ship_enemy=NULL;
	  }
	  if(0){
	    printf("MISS: id: %d life: %f accel 1: %f gas: %f v:%f stype: %d mod: %d\n",
		   obj->id,obj->life,obj->accel,obj->gas,
		   sqrt(obj->vx*obj->vx+obj->vy*obj->vy),
		   obj->subtype,obj->modified);
	  }
	  if(ship_enemy==NULL){
	    obj->ang_a=obj->ang_v=0;
	    
	    obj->accel+=obj->engine.a;
	    if(obj->accel > obj->engine.a_max)
	      obj->accel=obj->engine.a_max;	    
	    
	  }
	  else{
	    rx=ship_enemy->x - obj->x;
	    ry=ship_enemy->y - obj->y;
	    b=atan2(ry,rx);
	    ib=b - obj->a;
	    
	    if(ib>0){
	      obj->ang_a+=obj->engine.ang_a;
	      if(obj->ang_a<0)obj->ang_a=0;
	    }
	    if(ib<0){
	      obj->ang_a-=obj->engine.ang_a;
	      if(obj->ang_a>0)obj->ang_a=0;
	    }
	    obj->accel+=obj->engine.a;
	    if(obj->accel > obj->engine.a_max)
	      obj->accel=obj->engine.a_max;	    
	  }
	  if(0&&obj==cv){
	    printf("\t accel 2: %f engacel: %d\n",obj->accel,obj->engine.a);
	  }
	  UpdateShip(obj);	  
	  break;
	default:
	  obj->x0 = obj->x;
	  obj->y0 = obj->y;
	  
	  obj->x += obj->vx*DT;
	  obj->y += obj->vy*DT;
	  break;
	}
	break;
      case SHIP:
	if(obj->trace==TRUE && habitat.type==H_SPACE){
	  nobj=NewObj(&listheadobjs,TRACE,NULO,
		 obj->x,obj->y,
		 0,0,
		      CANNON0,ENGINE0,obj->player,obj,obj->in);
	  Add2ObjList(&listheadobjs,nobj);
	}

	switch(obj->subtype){
	case SHIP1:
	case SHIP2:
	case SHIP3:
	case SHIP4:
	case SATELLITE:
	case TOWER:
	  if(proc==players[obj->player].proc){
	    ai(&listheadobjs,obj,actual_player);
	  }
	  if(0&&obj==cv){
	    printf("\t accel 2: %f v: (%f,%f)   engacel: %d\n",obj->accel,obj->vx,obj->vy,obj->engine.a);
	  }
	  if(0&&obj==cv)printf("\tupdate ship. gas: %f %f\n",obj->gas,obj->accel);
	  UpdateShip(obj);
	  if(0&&obj==cv)printf("\tupdate ship. gas: %f %f\n",obj->gas,obj->accel);
	  break;
	  
	default:
	  g_print("ERROR (UpdateObjs 1)\n");
	  exit(-1);
	  break;
	}
	break;
      case ASTEROID:
	UpdateAsteroid(obj);
	break;
	
      case TRACKPOINT:
      case TRACE:
	break;
      case PLANET:
	/* planets create some gold */
	if(0&&obj->id==11 && cv!=NULL){
	  
	  float dist=sqrt((cv->x-obj->x)*(cv->x-obj->x)+(cv->y-obj->y)*(cv->y-obj->y));
	  printf("dist: %f \n",dist);
	}

	obj->planet->A=obj->planet->reggold;
	break;
	
      default:
	g_print("ERROR (UpdateObjs 2) %d\n",obj->type);
	exit(-1);
	break;
	
      }
#if CLOSEDUNIVERSE 
      if(obj->x>gulx)obj->x-=gulx;
      if(obj->x<0)obj->x+=gulx;
      if(obj->y>guly)obj->y-=guly;
      if(obj->y<0)obj->y+=guly;
#endif
    } /* if (obj->life>0) */
    
    if(0){
      if(obj->life<=0){
	if(proc==players[obj->player].proc){
	  obj->state=0;obj->sw=0;
	  if(gnet==TRUE){
	    if(GetProc()==players[obj->player].proc){
	      SetModified(obj,SENDOBJKILL);
	      obj->ttl=0;
	    }
	  }
	}
	else{
	  if(gnet==TRUE){
	    if(obj->type==PROJECTILE && obj->subtype==EXPLOSION){
	      if(GetProc()==players[obj->player].proc){
		SetModified(obj,SENDOBJKILL);
		obj->ttl=0;
	      }
	    }
	  }
	}
      }
    }
    
    if(obj->life<=0){
      obj->state=0;obj->sw=0;
    }
    
    if(gnet==TRUE){
      obj->ttl--;
      if(obj->life<=0){
	if(GetProc()==players[obj->player].proc){
	  SetModified(obj,SENDOBJKILL);
	  obj->ttl=0;
	}
      }
    }
    
    ls=ls->next;
    n++;
    if (n>listheadobjs.n){
      g_print("ERROR: n: %d  N: %d\n",n,listheadobjs.n);
      return(-1);
    }
  } /* while(ls!=NULL) */
  return (n);
}

gint TimerCreateObj(gpointer data){
  /* not used */
  Object *nobj;

  nobj=NewObj(&listheadobjs,SHIP,SHIP1, 
	 GameParametres(GET,GWIDTH,0)*Random(-1), 
	 GameParametres(GET,GHEIGHT,0)*Random(-1), 
	 0,0,
	 /*level_vel*(2*VELMAX*(Random(-1))-VELMAX),  */
	 /*level_vel*(2*VELMAX*(Random(-1))-VELMAX),  */
	      CANNON0,ENGINE2,0,NULL,NULL);
  Add2ObjList(&listheadobjs,nobj);
  return 1000;
}


int CheckPlanetDistance(struct HeadObjList *lh,float x,float y){
  /*
    The distance between two planets must be greater than 600
  */

  struct ObjList *ls;

  ls=lh->next;
  while(ls!=NULL){
    if(ls->obj->type==PLANET){      
      if((ls->obj->x-x)*(ls->obj->x-x)+(ls->obj->y-y)*(ls->obj->y-y)<600*600 ){
	return(1);
      }
    }
    ls=ls->next;
  }
  
  return(0);

}

void CreateUniverse(int ulx,int uly,struct HeadObjList *lheadobjs,char **ptnames){
  /*
    Create Galaxies and planets.
   */


  int i,j;
  float x,y;
  float x0,y0;
  float rg;
  Object *obj;
  int n,np=0;
  int nplanetpergalaxie=1;
  int nplanets;
  int ngalaxies=1;


  //HERE check this equation. galaxie size

  ngalaxies=GameParametres(GET,GNGALAXIES,0);

  rg=ulx/ngalaxies;

  nplanets=GameParametres(GET,GNPLANETS,0);
  nplanetpergalaxie=(int)((float)nplanets/ngalaxies+0.5);

  //  rg=sqrt(nplanetpergalaxie/1E-9);
  //  printf("galaxie radius: %f\n",rg);

  if(nplanetpergalaxie/(rg*rg) < 3E-9){
    //HERE    rg=sqrt(nplanetpergalaxie/3E-9);
  }

  printf("\n\tnumber of galaxies: %d\n",ngalaxies);
  printf("\tnumber of planets per galaxie: %d\n",nplanetpergalaxie);
  printf("\tgalaxie radius: %.0f\n",rg);

  for(j=0;j<ngalaxies;j++){
    if(np>=nplanets)break;
    x0=ulx*Random(-1)-ulx/2;
    y0=uly*Random(-1)-uly/2;
    if(ngalaxies==1){
      x0=0;
      y0=0;
    }
    for(i=0;i<nplanetpergalaxie;i++){
      if(np>=nplanets)break;
      n=0;
      do{
	x=x0+rg*Random(-1)-rg/2;
	y=y0+rg*Random(-1)-rg/2;
	n++;
	if(n>100){
	  fprintf(stderr,"ERROR: Universe size too small or too much planets. Exiting...\n");
	  exit(-1);
	}
      }
      while(CheckPlanetDistance(lheadobjs,x,y));
	
      /*    printf("%f %f\n",x,y); */
      obj=NewObj(lheadobjs,PLANET,NULO,x,y,0,0,CANNON0,ENGINE0,0,NULL,NULL);
      if(obj!=NULL){
	//	printf("Created Planet %d\n",obj->id);
      
	if(i<NUMPLANETNAMES-1 && j==0){
	  strncpy(obj->name,ptnames[i+1],OBJNAMESMAXLEN);
	}
	else{
	  strncpy(obj->name,ptnames[0],OBJNAMESMAXLEN);
	}
	/* obj->mass=150000;//HEREPROD */
	Add2ObjList(lheadobjs,obj);
	np++;
      }
    }
  }
}

float PlanetAtraction(float *fx,float *fy,float x,float y,float m){
  /*
    calculates the atraction forces betwwen ships and planets
    returns:
    the force in fx,fy arguments.
    the potential energy.
  */

  struct ObjList *ls;
  float rx,ry,r,r2,ir2r;
  float U; /* potential*/
  float GM;
  
  ls=listheadplanets.next;
  U=0;

  while (ls!=NULL){
    rx=ls->obj->x-x;
    ry=ls->obj->y-y;
    r2=rx*rx+ry*ry;

    if(r2<GRAVITATION_RANGE2){
      GM=G*ls->obj->mass;
      r=sqrt(r2);
      ir2r=1./(r2*r);
      *fx+=GM*rx*ir2r;
      *fy+=GM*ry*ir2r;
      U-=GM/r;
    }

#if DEBUG
    if(ls->obj->type!=PLANET)
      fprintf(stderr,"ERROR PlanetAtraction():(%d) Its not a planet.\n",
	     ls->obj->id);
#endif

    ls=ls->next;
  }
 
  *fx*=m;
  *fy*=m;
  U*=m;

#if DEBUG
  if(!(GetTime()%50))  
    printf("fx:%f fy:%f f:%f\n",*fx,*fy,sqrt(*fx*(*fx)+*fy*(*fy)));
#endif 
  return(U);
}



int PrintfObjInfo(FILE *fp,Object *obj){
  /*
    printf obj info
    used only for debug.
   */

  int in,dest,parent,planet;
  fprintf(fp,"time: %d\n id:%d durable:%d visible:%d\n \
points:%d habitat:%d mode:%d \nmodified: %d \n\
x:%g y:%g x0:%g y0:%g \nvx:%g vy:%g \n\
fx0:%g fy0:%g fx:%g fy:%g\n a:%g ang_v:%g accel:%g \n\
gas:%g gas_max:%g life:%g shield:%g state:%g\n",
	  GetTime(),
	  obj->id,obj->durable,obj->visible,
	  obj->kills,obj->habitat,obj->mode,
	  obj->modified,
	  obj->x,obj->y,obj->x0,obj->y0,
	  obj->vx,obj->vy,
	  obj->fx0,obj->fy0,obj->fx,obj->fy,
	  obj->a,obj->ang_v,obj->accel,
	  obj->gas,obj->gas_max,
	  obj->life,obj->shield,obj->state);

  fprintf(fp,"dest_r2:%g mass:%d radio:%d \ntype:%d subtype:%d\n\
 damage:%d player:%d player:%d\nai:%d trace:%d norder:%d\n",
	  obj->dest_r2,obj->mass,obj->radio,
	  obj->type,obj->subtype,
	  obj->damage,obj->player,obj->player,
	  obj->ai,obj->trace,obj->norder);
  
  in=0;
  dest=0;
  planet=0;
  parent=0;

  if(obj->in!=NULL)in=obj->in->id;
  if(obj->dest!=NULL)dest=obj->dest->id;
  if(obj->parent!=NULL)parent=obj->parent->id;
  if(obj->planet!=NULL)planet=obj->id;
  
  fprintf(fp,"parent:%d dest:%d in:%d planet:%d\n weapon:%d engine:%d\n ",
	  parent,dest,in,planet,
	  obj->weapon->type,obj->engine.type);
  

  fprintf(fp,"parent:%p dest:%p in:%p planet:%p\n",
	  obj->parent,obj->dest,obj->in,obj->planet);

  return(0);
}

Object *ChooseInitPlanet(struct HeadObjList lheadobjs){
  /*
    returns:
    a pointer to a free planet.
   */

  struct ObjList *ls;
  int cont=0;
  int nplanets=0;
  int rplanet;

  /* counting planets */
  ls=lheadobjs.next;
  while(ls!=NULL){
    if(ls->obj->type==PLANET){
      cont++;
    }
    ls=ls->next;
  }

  nplanets=cont;
  rplanet=(int)(nplanets*Random(-1));

  ls=lheadobjs.next;
  cont=0;
  while(ls!=NULL){
    if(ls->obj->type==PLANET){
      if(ls->obj->player==0 && cont==rplanet){  /* a random planet */
	return(ls->obj);
      }
    }
    ls=ls->next;
    cont++;
  }

  cont=0;
  ls=lheadobjs.next;
  while(ls!=NULL){
    if(ls->obj->type==PLANET ){
      if(ls->obj->player==0){  /* the first free planet */
	return(ls->obj);
      }
    }
    ls=ls->next;
    cont++;
  }

  fprintf(stderr,"ERROR ChooseInitPlanet(): Too many players or too few planets\n");
  return(NULL);
}

void CreateShips(struct HeadObjList *lheadobjs){
  /*
    Choose initial planet.
    Create the initial player ships
   */
  Object *obj,*planet;
  Segment *s;
  int i;

  
  for(i=1;i<=GameParametres(GET,GNPLAYERS,0);i++){
    planet=ChooseInitPlanet(*lheadobjs);
    if(planet==NULL){
      fprintf(stderr,"ERROR CreateShips(): obj==NULL\n");
      exit(-1);
    }
    s=LandZone(planet->planet);
    if(s==NULL){
      fprintf(stderr,"ERROR CreateShips(): Segment==NULL\n");
      exit(-1);
    }

    //    for(n=0;n<10;n++){
    obj=NewObj(lheadobjs,SHIP,QUEEN,
	       0,0,0,0,
	       CANNON5,ENGINE5,i,NULL,planet);
    obj->x=obj->x0=(.5*(s->x0+s->x1));
    /* obj->y=obj->y0=(s->y0)+(obj->radio+1.0); */
    obj->y=obj->y0=(s->y0);
    obj->a=PI/2;
    //    obj->player=i;
    obj->ai=1;
    obj->habitat=H_PLANET;
    obj->mode=LANDED;


      
    Add2ObjList(lheadobjs,obj);
    players[obj->player].nbuildships++;
    //    }
    planet->player=obj->player;

    if(GameParametres(GET,GNET,0)==TRUE){
      if(GetProc()==players[planet->player].proc){
	SetModified(planet,SENDOBJPLANET);
	planet->ttl=0;
      }
    }

    players[obj->player].kplanets=Add2IntList((players[obj->player].kplanets),planet->id);
    players[obj->player].nplanets++;

    if(GameParametres(GET,GKPLANETS,0)==FALSE){
      int j,k;
      
      for(j=-2;j<3;j++){
	for(k=-2;k<3;k++){
	  if(j*j+k*k<8){  
	    Add2IntIList(&(players[obj->player].ksectors),
			 Cuadrante(planet->x+j*SECTORSIZE,planet->y+k*SECTORSIZE));
	  }
	}
      }
    }
    if(i==actual_player){
      cv=obj;
      ship_c=obj;
      habitat.type=H_PLANET;
      habitat.obj=planet;
      /*      obj->in->mass*=5; */
    }
  }
  return;
}


void CreateTestShips(struct HeadObjList *lheadobjs){
  /*
    Create some ships for test propose.
   */
  Object *obj;
  float a,x,y;
  int i;
  

  if(1){ /* asteroids */
    for(i=0;i<4;i++){
      a=i*PI/3;
      x=450*sin(a);
      y=450*cos(a);
      obj=NewObj(lheadobjs,ASTEROID,ASTEROID1,
		 0,0,0,0,
		 CANNON0,ENGINE0,2,NULL,NULL);
      obj->x=obj->x0=x;
      obj->y=obj->y0=y;
      obj->vx=0;
      obj->vy=0;
      obj->a=PI/2;
      //      obj->player=2;
      obj->ai=0;
      obj->in=NULL;
      obj->habitat=H_SPACE;
      obj->mode=NAV;
      obj->player=0;
      Add2ObjList(lheadobjs,obj);
    }  
  }

  if(0){ /* five static ships */
    for(i=0;i<5;i++){
      a=i*PI/3;
      x=400*sin(a);
      y=400*cos(a);
      obj=NewObj(lheadobjs,SHIP,SHIP3,
		 0,0,0,0,
		 CANNON4,ENGINE1,2,NULL,NULL);
      obj->x=obj->x0=x;
      obj->y=obj->y0=y;
      obj->a=PI/2;
      obj->player=2;
      obj->ai=1;
      obj->in=NULL;
      obj->habitat=H_SPACE;
      obj->mode=NAV;
      Add2ObjList(lheadobjs,obj);
    }  
  }
  if(0){  
    x=400;
    y=100;
    obj=NewObj(lheadobjs,SHIP,SHIP3,
	       0,0,0,0,
	       CANNON4,ENGINE3,2,NULL,NULL);
    obj->x=obj->x0=x;
    obj->y=obj->y0=y;
    obj->a=PI/2;
    //    obj->player=2;
    obj->weapon->n=250;  
    obj->ai=1;
    obj->in=NULL;
    obj->habitat=H_SPACE;
    obj->mode=NAV;
    obj->experience=0;  
    Add2ObjList(lheadobjs,obj);
    x=-400;
    y=100;
    obj=NewObj(lheadobjs,SHIP,SHIP3,
	       0,0,0,0,
	       CANNON4,ENGINE3,2,NULL,NULL);
    obj->x=obj->x0=x;
    obj->y=obj->y0=y;
    obj->a=PI/2;
    //    obj->player=2;
    obj->weapon->n=250;  
    obj->ai=1;
    obj->in=NULL;
    obj->habitat=H_SPACE;
    obj->mode=NAV;
    obj->experience=0;  
    Add2ObjList(lheadobjs,obj);
  }

  /* player ship */
  
  x=0;
  y=0;
  obj=NewObj(lheadobjs,SHIP,SHIP3,
	     0,0,0,0,
	     CANNON4,ENGINE3,1,NULL,NULL);
  obj->x=obj->x0=x;
  obj->y=obj->y0=y;
  obj->vy=0;
  obj->a=PI/2;
  //  obj->player=1;
  obj->ai=0;
  obj->in=NULL;
  obj->habitat=H_SPACE;
  obj->mode=NAV;
  obj->weapon->n=250;  
  obj->experience=0;
  Experience(obj,800);
  Add2ObjList(lheadobjs,obj);
  obj->weapon1.n=10;
  obj->weapon2.n=30;
  if(obj->player==actual_player){
    cv=obj;
    ship_c=obj;
  }
  return;
}


int CheckGame(char *cad){
  /*
    Check some game consistence.
    Used every step (!!HERE check this)
    Used after load a game.
  */

  struct ObjList *ls;
  struct ListOrder *lo;
  Object *obj;
  int nord;
  int i;
  int n=0;
  int types[20];
  int type;
  int proc;


  proc=GetProc();

  /* Checking Orders */

#if DEBUG
  printf("CheckGame()\n");
#endif

  for(i=0;i<20;i++){
    types[i]=0;
  }


  printf("%s",cad);
 
  ls=listheadobjs.next;
  while(ls!=NULL){
    obj=ls->obj;

    type=obj->type+1;
    if(type>19)type=19;
    types[type]++;


    if(proc==players[obj->player].proc){
      nord=obj->norder;
      n=0;
      lo=obj->lorder;
      while(lo!=NULL){
	n++;
	lo=lo->next;
      }
      if(n!=obj->norder){
	fprintf(stderr,"ERROR CheckGame():norder incorrect\n");
	printf("\tObj: %d, type: %d norder=%d , size of list: %d\n",
	     obj->id,obj->type,obj->norder,n);
	printf("\tDeleting all the orders of object %d\n",obj->id);
	DelAllOrder(obj);
      } 

      /* if planet dont belongs to a landed ship */
      if(obj->mode==LANDED && obj->in==NULL){
	fprintf(stderr,"\tError CheckGame() obj %d landed and in NULL\n",obj->id);
	ls=ls->next;continue;
      }
      if(obj->mode==LANDED && obj->in!=NULL){
	if(obj->in->planet==NULL){
	  fprintf(stderr,"\tError CheckGame() obj %d planet not assigned\n",obj->id);
	  ls=ls->next;continue;
	}
      }
      if(obj->mode==LANDED && players[obj->in->player].team!=players[obj->player].team){ 
	/* 
	   this is posible if two ships of different players are landed in the same planet.
	 */
	obj->in->player=obj->player;
      }
    }
    ls=ls->next;
  }

/*   for(i=0;i<20;i++){ */
/*     printf("n. obj of type %d: %d\n",i,types[i]); */
/*   } */

  return(0);
}


void Density(void){
  /*
    
    Not used by now

   */
  struct ObjList *ls;
  int i,j;
  int den[10][10];
  float x,y;

  for(i=0;i<10;i++)
    for(j=0;j<10;j++)
      den[i][j]=0;

  ls=listheadobjs.next;
  while(ls!=NULL){
    if(ls->obj->type==PLANET){
      
      if(ls->obj->in!=NULL){
	x=ls->obj->in->planet->x;
	y=ls->obj->in->planet->y;
      }
      else{
	x=ls->obj->x;
	y=ls->obj->y;
      }


      i=10.*x/(float)GameParametres(GET,GULX,0);
      j=10.*y/(float)GameParametres(GET,GULY,0);
      /*      printf("%d: %d %d %f %f %f\n",ls->obj->id,i,j,x,y,(float)gulx); */
      i+=5;
      j+=5;
      if(i<0)i=0;
      if(j<0)j=0;
      if(i>9)i=9;
      if(j>9)j=9;
      den[i][j]++;
    }
    ls=ls->next;
  }

  for(i=0;i<10;i++){
    for(j=0;j<10;j++){
      /*      printf("%d ",den[i][j]); */
    }
    /*    printf("\n"); */
  }
}
void GetUniverse(void){

  /*ExecLoad("kk"); */
  ExecLoad(savefile);
  p_time=GetTime();
  CheckGame("Checking game after load...");
  printf("done\n");
}

void NetComm(void){
   /* net gamming */

  /* orders to  thread:
     OTSENDOBJS: send modified objects.
     OTSENDALLOBJS save: send all the objects.
     OTSENDLOAD load: load all the objects.
     OTSENDKILL to finish: quit the program.
  */
  int mode;  
  int fd;

  mode=GameParametres(GET,GMODE,0);

  order2thread=OTSENDOBJS;

  if(mode==SERVER){
    if(keys.save==TRUE){
      order2thread=OTSENDSAVE;
    }
    
    if(keys.load==TRUE){
      /* check if is possible open the dir */ 
      /* checking the file */
      if((fd=open(savefile,O_RDONLY))==-1){
	fprintf(stdout,"CommServer()[OTSENDLOAD]:No puede abrirse el archivo %s\n",savefile);
      }
      else{
	order2thread=OTSENDLOAD;
      }
      close(fd);
    }
  }

  if(GameParametres(GET,GQUIT,0)==2){
    order2thread=OTSENDKILL;
  }

  switch(mode){
  case SERVER:
    LoadBuffer(order2thread,&buffer1,SERVER);
    sem_post(&sem_barrier1); /* run comm. thread*/
    break;
  case CLIENT:
    //    LoadBuffer(order2thread,&buffer1,CLIENT);
    sem_post(&sem_barrier1); /* run comm. thread*/
    break;
  default:
    break;
  }
  
  sem_wait(&sem_barrier); //HERE send this far away in code
}


void signal_handler(int sign,siginfo_t *info,void *ignored){
  g_cont++;
  printf("sig: %d cont:%d\n",sign,g_cont);
}

void int_handler(int sign){
  fprintf(stderr,"signal n %d received (SIGINT)\n",sign);
  Quit(NULL,NULL);
}

void segfault_handler(int sign){
  fprintf(stderr,"signal n %d received (SEGFAULT)\n",sign);
  if(ExecSave(listheadobjs,SAVETMPFILE)!=0){
    fprintf(stderr,"Error in main(): I cant open %s\n",SAVETMPFILE);
    exit(-1);
  }
  fprintf(stderr,"game saved in %s\n",SAVETMPFILE);
  exit(-1);
  /*  Quit(NULL,NULL); */
}


void DrawInfo(GdkPixmap *pixmap,Object *obj){
  /*
    Show info of the player and actual ship.
  */  

  static GdkGC *gc;
  static GdkGC *gcframe;
  static int charw=10;
  static int charh=10;
  static int swgmess=0;
  static int glen=0;

  GdkGC *gcmessage;
  int textw;
  Object *planet;
  Object *objt;
  float d2;
  char point[TEXTMENMAXLEN];
  char tmpcad[16];
  int x,y;
  int h,m,s;
  int i;
  static int sw=0;
  int lsw;
  struct TextMessageList *lh;
  int time;
  int gwidth,gwidth2,gheight;
  int incy;

#if DEBUG
  printf("DrawInfo()\n");
#endif

  if(sw==0){
    gc=penGreen;
    gcframe=penGreen;
    sw=1;
  }
  gwidth=GameParametres(GET,GWIDTH,0);
  gheight=GameParametres(GET,GHEIGHT,0);
  gwidth2=gwidth/2;

  time=GetTime();

  /* 
   *    General info
   */
  if(gfont!=NULL){
    charh=gdk_text_height(gfont,"pL",2);
    charw=gdk_text_width(gfont,"O",1);
  }
  else{
    charh=12;
    charw=12;
  }
  incy=charh;
  y=incy;
  x=gwidth2-7*charw;
  sprintf(point,"record: %d",record);
  DrawString(pixmap,gfont,penGreen,x,y,point);

  if(TEST){/* PRODUCTION */
    x+=15*charw;
    gc=penRed;
    sprintf(point,"%d   %d   %d   %d  %d  (%.2f)",
	    CountObjs(&listheadobjs,-1,-1,-1),
	    CountObjs(&listheadobjs,-1,SHIP,-1),
	    CountObjs(&listheadobjs,-1,PROJECTILE,-1),
	    CountObjs(&listheadobjs,-1,ASTEROID,-1),
	    CountObjs(&listheadobjs,-1,TRACE,-1),
	    (float)((20.0*g_nobjsend)/time));
    DrawString(pixmap,gfont,penRed,x,y,point);

    sprintf(point,"%d   %d   %d   %d",
	    CountObjs(&listheadobjs,actual_player,-1,-1),
	    CountObjs(&listheadobjs,actual_player,SHIP,EXPLORER),
	    CountObjs(&listheadobjs,actual_player,SHIP,FIGHTER),
	    CountObjs(&listheadobjs,actual_player,SHIP,TOWER));
    
    DrawString(pixmap,gfont,penRed,x,y+15,point);
  }

  y+=incy;
  h=(int)(time/(20*60*60));
  m=(int)(time/(20*60))-h*60;
  s=(int)(time/(20))-h*3600-m*60;
  
  sprintf(point,"time: %d:",h);
  if(m<10){
    sprintf(tmpcad,"0%d:",m);
  }
  else{
    sprintf(tmpcad,"%d:",m);
  }
  strcat(point,tmpcad);

  if(s<10){
    sprintf(tmpcad,"0%d",s);
  }
  else{
    sprintf(tmpcad,"%d",s);
  }
  strcat(point,tmpcad);
  x=gwidth2-7*charw;
  DrawString(pixmap,gfont,penGreen,x,y,point);

  
  /* 
   *      order info 
   */

  if(keys.o==FALSE){ 
    gdraw.order=FALSE;
    DrawString(pixmap,gfont,penRed,10,gheight+GameParametres(GET,GPANEL,0)/2+4, 
	       "O: Introduce command"); 
  }
  //  printf("%d %d\n",PANEL_HEIGHT,GameParametres(GET,GPANEL,0));  
    /*
     *    Ship info
     */
  if(obj!=NULL){
    if(obj->type==SHIP && gdraw.info==TRUE){  
      y=DrawShipInfo(pixmap,gfont,penGreen,obj,0,0);

      sprintf(point,"============");
      DrawString(pixmap,gfont,penGreen,10,y,point);
      y+=incy;

    }
  }

  /*
   *  Player info
   */
  if(gdraw.info==TRUE){
    y=DrawPlayerInfo(pixmap,gfont,penCyan,&players[actual_player],10,y);
    sprintf(point,"============");
    DrawString(pixmap,gfont,penGreen,10,y,point);
    y+=incy;
  
    
    /*
     *   Planet info
     */
    if(obj!=NULL){
      if(obj->type==PLANET)planet=obj;
      else planet=obj->in;
      
      if(planet!=NULL){

	y=DrawPlanetInfo(pixmap,gfont,penGreen,planet,10,y);
	sprintf(point,"============");
	DrawString(pixmap,gfont,penGreen,10,y,point);
	y+=incy;
      }
    }
  }
  /* Enemy info */
  objt=NearestObj(&listheadobjs,cv,SHIP,PENEMY,&d2);
  if(cv!=NULL && objt!=NULL){
    if(d2 < (cv->radar*cv->radar)){
      DrawEnemyShipInfo(pixmap,gfont,penGreen,objt,gwidth,0);
    }
  }
  
  
  /* text messages */

 /* net message */  
  if(PendingTextMessage()){
    GetTextMessage(point);
    DrawString(pixmap,gfont,gcframe,gwidth2-incy*6,.25*gheight,point);
    //    printf("draw info(): message: %s\n",point);
  }

  /* game messages */

  lh=listheadtext.next;
  i=0;
  lsw=0;
  if(swgmess){
    gdk_draw_rectangle(pixmap,    
		       penBlack,
		       TRUE,   
		       10,
		       gheight-incy*swgmess-10,
		       glen+15,
		       incy*swgmess+10);

    gdk_draw_rectangle(pixmap,    
		       penRed,
		       FALSE,   
		       10,
		       gheight-incy*swgmess-10,
		       glen+15,
		       incy*swgmess+10);
  }
  glen=0;
  while(lh!=NULL){
    strncpy(point,lh->info.text,TEXTMENMAXLEN);
    if(lh->info.dest!=actual_player && lh->info.dest !=-1){
      lh->info.duration=0;
      lsw=1;
    }
    else{
      
      switch(lh->info.value){
      case 0:
	gcmessage=penRed;
	break;
      default:
	gcmessage=penWhite;
	break;
      }

      DrawString(pixmap,gfont,gcmessage,20,gheight-incy*swgmess+incy*i+5,point);
      textw=gdk_text_width(gfont,point,strlen(point));      
      if(textw>glen)glen=textw;
      //      printf("draw info2(): message: %s\n",point);
      if(lh->info.print==0){
	fprintf(stdout,"%s\n",point);
	lh->info.print=1;
      }
      /*      DrawString(pixmap,gfont,gcframe,350,120+15*i,point); */
      if(i<3){
	lh->info.duration--;
	if(lh->info.duration<=0)lsw=1;
      }
      i++;
    }
    if(i>9)break;

    lh=lh->next;
  }
  swgmess=i;  
  if(lsw){ /* cleaning the message list */
    struct TextMessageList *freels;
    lh=&listheadtext;
    while(lh->next!=NULL){
      if(lh->next->info.duration<=0){
	freels=lh->next;
	lh->next=lh->next->next;
	free(freels);
	g_memused-=sizeof(struct TextMessageList);
	freels=NULL;
	listheadtext.info.n--;
	continue;
      }
      lh=lh->next;
    }
  }
}

void GetGold(void){
  /*
    version 03 21Dic2010
    Count the number of planets that belongs to each player    
    Increase players gold 
   */


  struct ObjList *ls;
  int nplayers;
  int i;
  int proc;
  float inctower;
  float levelfactor;
 
#if DEBUG
  printf("GetGold()\n");
#endif

  proc=GetProc();
  nplayers=GameParametres(GET,GNPLAYERS,0);


  for(i=0;i<nplayers+2;i++){
    players[i].nplanets=0;
    players[i].nships=0;
    players[i].balance=0;
  }

  ls=listheadobjs.next;
  while(ls!=NULL){
    switch(ls->obj->type){
    case PLANET:
      players[ls->obj->player].nplanets++;
      break;
    case SHIP:
      players[ls->obj->player].nships++;
      break;
    default:
      ls=ls->next;continue;
      break;
    }

    if(proc!=players[ls->obj->player].proc){
      ls=ls->next;continue;
    }
    if(ls->obj->player==0){
      ls=ls->next;continue;
    }

    switch(ls->obj->type){
    case PLANET:
      /*gold for each planet*/
      players[ls->obj->player].balance+=.01*RESOURCEFACTOR; 
      //      printf("planet %d reggold: %f  A: %f\n",ls->obj->id,ls->obj->planet->reggold,ls->obj->planet->A);
      break;
    case SHIP:
      players[ls->obj->player].balance-=ls->obj->cost;
      //      printf("%d type: %d L:%d cost: %f\n",ls->obj->pid,ls->obj->subtype,ls->obj->level,ls->obj->cost);


      switch(ls->obj->subtype){
      case TOWER:
	levelfactor=(1.0+1.0*ls->obj->level);
	
	if(ls->obj->in->planet->gold>0){
	  inctower=(.4+0.1*levelfactor)*RESOURCEFACTOR;
	  if(ls->obj->in->planet->gold>inctower){
	    players[ls->obj->player].balance+=inctower; 
	    ls->obj->in->planet->gold-=inctower;
	    ls->obj->in->planet->A-=0.015;
	    if(ls->obj->in->planet->A<0)ls->obj->in->planet->A=0;
	    

	  }
	  else{
	    players[ls->obj->player].balance+=ls->obj->in->planet->gold;
	    ls->obj->in->planet->gold=0;
	  }
	}
	else{
	  ls->obj->in->planet->gold=0;
	  if(ls->obj->in->planet->A > 0.015){
	    players[ls->obj->player].balance+=0.015*levelfactor;
	    ls->obj->in->planet->A-=0.015;
	    if(ls->obj->in->planet->A<0)ls->obj->in->planet->A=0;
	  }
	  else{
	    players[ls->obj->player].balance+=ls->obj->in->planet->A*levelfactor;
	    ls->obj->in->planet->A=0.0;
	  }
	}
	/* 	  printf("\tplanet %d reggold: %f  A: %f (%d) type: %d %f\n",  */
	/* 		 ls->obj->in->id,ls->obj->in->planet->reggold,ls->obj->in->planet->A,ls->obj->pid,ls->obj->subtype,players[ls->obj->player].balance);  */
	
	//	  printf("planet %d reggold: %f  A: %f\n",ls->obj->id,ls->obj->planet->reggold,ls->obj->planet->A);
	//	  ls->obj->in->planet->gold+=(0.01+0.01*ls->obj->level)*RESOURCEFACTOR;
	
	break;
	
      default:
	break;
      }
      /* 	printf("\tplanet %d reggold: %f  A: %f type: %d\n", */
      /* 	       ls->obj->in->id,ls->obj->in->planet->reggold,ls->obj->in->planet->A,ls->obj->subtype); */
      
      
      break;
    default:
      break;
    }
    ls=ls->next;
  }
  
  for(i=0;i<nplayers+2;i++){
    if(proc==players[i].proc){
      players[i].gold+=players[i].balance;
    }
  }
}

void GetPoints(struct HeadObjList hol,int proc,struct Player *p){
  /* 
     version 0.3    12Dic2010

     if state is 0 sum points and experience
  */
  
  struct ObjList *ls;
  Object *obj;   /* dead object */
  Object *obj2;  /* killer */
  Object *obj3;  /* who receive points */
  int il;
  float factor,points;
  int sw=0;
  int gnet;
  
#if DEBUG
  printf("GetPoints()\n");
#endif

  ls=hol.next;
  gnet=GameParametres(GET,GNET,0);

  while(ls!=NULL){
    obj=ls->obj;
    obj2=obj3=NULL;
    sw=0;
    if(gnet==TRUE){
      if(obj->modified==SENDOBJDEAD)sw=1;
    }
    else{
      if(obj->state<=0)sw=1;
    }

    if(sw){
      //      printf("obj pid: %d modified: %d\n",obj->pid,obj->modified);

      switch(obj->type){
      case PROJECTILE:
	/* points to nobody */

	break;
      case SHIP:
	//	printf("%d killed by ",obj->id);
	p[obj->player].ndeaths++;
	/* points to the killer */
	if(obj->sw!=0){
	  obj2=SelectObj(&listheadobjs,(obj->sw));
	  if(obj2!=NULL){
	    obj3=NULL;
	    if(obj2->type==SHIP)obj3=obj2;
	    if(obj3!=NULL){
	      /* must be a SHIP */
	      //    printf("%d",obj3->id);
	      obj3->kills++;
	      p[obj3->player].nkills++;

	      /* Experience for kill an enemy */
	      il=obj->level-obj3->level;
	      factor=50;
	      if(il<0)factor/=2;
	      if(il<-1)factor/=2;
	      if(il<-2)factor/=2;
	      if(factor>0){
		points=factor*pow(2,obj->level);
		if(points<10)points=10;
		Experience(obj3,points);
	      }
	      /* --Experience for kill an enemy */
	    }
	  }
	}
	break;
      case ASTEROID:
	if(obj->sw!=0){
	  obj2=SelectObj(&listheadobjs,(obj->sw));
	  if(obj2!=NULL){
	    obj3=NULL;
/* 	    if(obj2->type==PROJECTILE){ */
/* 	      obj3=SelectObj(&listheadobjs,obj2->parent->id); */
/* 	    } */
	    if(obj2->type==SHIP)obj3=obj2;
	    if(obj3!=NULL){
	      /* must be a SHIP */

	      switch(obj->subtype){
	      case ASTEROID1:
		p[obj3->player].gold+=50;
		break;
	      case ASTEROID2:
		p[obj3->player].gold+=100;
		break;
	      case ASTEROID3:
		p[obj3->player].gold+=200;
		break;
	      default:
		fprintf(stderr,"ERROR in GetGold():asteroid subtype unknown\n");
		exit(-1);
		break;
	      }
	    }
	  }
	}
	break;
      default:
	break;
      }
    } /*  if(sw) */
    ls=ls->next;    
  }
  return;
}

void PrintGameOptions(void){

  printf("actual game options:\n");
  printf("\tNUM GALAXIES: %d\n",GameParametres(GET,GNGALAXIES,0));
  printf("\tNUM PLANETS: %d\n",GameParametres(GET,GNPLANETS,0));
  printf("\tNUM PLAYERS: %d\n",GameParametres(GET,GNPLAYERS,0));
  if(GameParametres(GET,GKPLANETS,0)==TRUE){
    printf("\tPlanets are known by all the players.\n");
  }
  printf("\tknown planets: %d\n",GameParametres(GET,GKPLANETS,0));
  printf("\tcooperative mode: %d\n",GameParametres(GET,GCOOPERATIVE,0));
  printf("\tcomputer cooperative mode: %d\n",GameParametres(GET,GCOMPCOOPERATIVE,0));
  printf("\tQueen mode: %d\n",GameParametres(GET,GQUEEN,0));
  printf("\tpirates: %d\n",GameParametres(GET,GPIRATES,0));
  
  printf("\tUniverse size: %d\n",GameParametres(GET,GULX,0));

}
