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

/* Comm Perfomance :
bin/space -s -p 8 -l 20000
fondo: 5.8 kB/s

2 minutes: 53 objects 15 ships, 18 shots   196 KB/s   141007
2 min 8(4,4) 65 45 0 : MAX: 256.8 kB/s TOT: 25.1 MB => 214 kB/s 
90 min       56 36 0 : MAX: 792   kB/s TOT: 1021 MB => 194 kB/s 
don't send projectiles:
5 min        45 25 0 : MAX: 213   kB/s TOT: 49 MB =>   167 kB/s, 311 obj/s
SENDOBJMOD
9:10 m       32 12 0 : MAX: 52.6  kB/s TOT: 18.8 MB =>  35 kB/s, 194 obj/s
SENDOBJMOD0
2:40 m       28  8 0 : MAX: 49.8  kB/s TOT: 4.1 MB =>  26.2 kB/s, 163 obj/s
5:20 m       49 29 0 : MAX: 58.9  kB/s TOT: 10.7MB =>  34.2 kB/s, 250 obj/s
5:41 m       38 18 0 : MAX: 66.4  kB/s TOT: 11.3MB =>  33.9 kB/s, 254 obj/s
SENDOBJAALL
5:26 m       43 23 0 : MAX: 52.7  kB/s TOT: 12.6MB =>  39.6 kB/s, 279 obj/s
SENDOBJUNMOD
6:13 m       48 21 7 : MAX: 61.3  kB/s TOT: 10.1MB =>  27.7 kB/s, 119 obj/s
SENDOBJUNMOD0
7:33 m       44 24 0 : MAX: 38.4  kB/s TOT: 9.8MB =>  22.15 kB/s, 109 obj/s

*/

#include "spacecomm.h"
#include "ai.h"
#include "data.h"
#include "functions.h"
#include "clock.h"
#include <time.h>

#define SENDORDERS 1
#define COMMDEBUG 0


#if TESTNET
struct timespec latency;
#endif

#if DEBUG
int debugcomm1=1;
#endif

int debugpilot=0;

extern struct HeadObjList listheadobjs;
struct TextMessageList listheadtext;
extern struct CharListHead gameloglist;          /* list of all game messages */
extern struct Window windowgamelog;
extern struct Habitat habitat;
extern char *savefile;
extern sem_t sem_barrier;
extern sem_t sem_barrier1;

extern int order2thread;

extern int actual_player,actual_player0;
extern Object *ship_c;
extern int g_objid;
extern int g_projid;
extern int g_nobjsend;
extern int g_nshotsend;
extern int g_nobjtype[6];
extern struct Parametres param;

extern int fobj[4];

extern struct Player *players;
extern struct Keys keys;
extern Object *cv;              /* coordenates center */

extern char clientname[MAXTEXTLEN];

struct TextMessage textmen0;  /* send message here */
struct TextMessage textmen1;  /* recv message here */
struct Buffer buffer1,buffer2; /* buffers used in comm. */
struct Global gclient;


int OpenComm(int mode,struct Parametres par,struct Sockfd *sockfd){
  /*
    version 02 26May2011
    Initiates comm sockets
    create thread to communication
    
   */

  struct hostent* he;

  int sfd,nsfd;
  struct sockaddr_in ser_addr,cli_addr;
  int cli_addr_len;

  int sfd2,nsfd2;
  struct sockaddr_in ser_addr2,cli_addr2;
  int cli_addr_len2;

  int i;

  sfd=nsfd=sfd2=nsfd2=0;

  textmen0.n=0;
  textmen0.time=0;
  strcpy(textmen0.text,"");

  textmen1.n=0;
  textmen1.time=0;
  strcpy(textmen1.text,"");



  buffer1.data=malloc(BUFFERSIZE*sizeof(char));
  if(buffer1.data==NULL){ 
    fprintf(stderr,"ERROR in malloc (buffer1)\n"); 
    exit(-1); 
  } 
  buffer1.n=0;
  buffer1.size=BUFFERSIZE;



  buffer2.data=malloc(BUFFERSIZE*sizeof(char));
  if(buffer2.data==NULL){ 
    fprintf(stderr,"ERROR in malloc (buffer2)\n"); 
    exit(-1); 
  } 
  buffer2.n=0;
  buffer2.size=BUFFERSIZE;

  for(i=0;i<buffer1.size;i++)buffer1.data[i]=0;
  for(i=0;i<buffer2.size;i++)buffer2.data[i]=0;

  switch(mode){
  case 0:/* server */
    /***** Resolving Hostname **********************/
    if ((he = gethostbyname(par.IP)) == NULL){
        perror("gethostbyname");
        exit(-1);
    }
     /**************************************************/
    fprintf(stdout,"Waiting for player ...\n");
    /* Apertura de un conector del tipo STREAM de la familia AF_INET */
    if ((sfd=socket(PF_INET,SOCK_STREAM,0)) == -1){
      perror("abrir socket");
      exit(-1);
    }
    
    /* Publicidad de la direccion del servidor */
 
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr= *((struct in_addr *)he->h_addr);
    ser_addr.sin_port = htons ( par.port);
    
    fprintf(stdout,"waiting conexion %s:%d\n",par.IP,par.port);

    if(bind(sfd,(struct sockaddr *)&ser_addr,sizeof(ser_addr)) == -1){
      perror("bind");
      fprintf(stderr,"port %d already in use\n",par.port);
      exit(-1);
    }

    /* Declaracion de una cola con un elemento para 
       peticiones de conexion */
    listen(sfd,1);

    /*Segundo puerto */

    /* Apertura de un conector del tipo STREAM de la familia AF_INET */
    if ((sfd2=socket(PF_INET,SOCK_STREAM,0)) == -1){
      perror("open socket");
      exit(-1);
    }
    
    /* Publicidad de la direccion del servidor */
 
    ser_addr2.sin_family = AF_INET;
    ser_addr2.sin_addr= *((struct in_addr *)he->h_addr);
    ser_addr2.sin_port = htons ( par.port2);
    
    if(bind(sfd2,(struct sockaddr *)&ser_addr2,sizeof(ser_addr)) == -1){
      perror("bind");
      fprintf(stderr,"port %d already in use\n",par.port);
      exit(-1);
    }

    /* Declaracion de una cola con un elemento para 
       peticiones de conexion */
    listen(sfd2,1);


    /* Atender Conexion 1 */
    cli_addr_len = sizeof(cli_addr);
    if((nsfd = accept (sfd,(struct sockaddr *)&cli_addr,(socklen_t *)&cli_addr_len)) == -1){
      perror("accept");
      exit(-1);
    }
 
    /* Atender Conexion 2 */
    cli_addr_len2 = sizeof(cli_addr2);
    if((nsfd2 = accept (sfd2,(struct sockaddr *)&cli_addr2,(socklen_t *)&cli_addr_len2)) == -1){
      perror("accept");
      exit(-1);
    }

    break;
  case 1: /* client */
    /*  **** Resolving Hostname **********************/
    if ((he = gethostbyname(par.IP)) == NULL){
        perror("gethostbyname");
        exit(-1);
    }
    /* ************************************************/
    fprintf(stdout,"Connecting with server: %s:%d\n",par.IP,par.port);

    /* Apertura de un conector del tipo STREAM de la familia AF_INET */
    if ((sfd=socket(PF_INET,SOCK_STREAM,0)) == -1){
      perror("open socket");
      exit(-1);
    }
    
    /* Peticion de conexion con el servidor */
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr= *((struct in_addr *)he->h_addr);
    ser_addr.sin_port = htons ( par.port);
    if(connect(sfd,(struct sockaddr *)&ser_addr,sizeof(ser_addr)) == -1){
      perror("conexion:");
      fprintf(stderr,"port: %d\n",par.port);
      exit(-1);
    }

    /* HERE duplicado */
    fprintf(stdout,"Connecting with server: %s:%d\n",par.IP,par.port2);
    /* Apertura de un conector del tipo STREAM de la familia AF_INET */
    if ((sfd2=socket(PF_INET,SOCK_STREAM,0)) == -1){
      perror("open socket");
      exit(-1);
    }
    
    /* Peticion de conexion con el servidor */
    ser_addr2.sin_family = AF_INET;
    ser_addr2.sin_addr= *((struct in_addr *)he->h_addr);
    ser_addr2.sin_port = htons ( par.port2);
    if(connect(sfd2,(struct sockaddr *)&ser_addr2,sizeof(ser_addr2)) == -1){
      perror("conexion");
      fprintf(stderr,"port: %d",par.port2);
      exit(-1);
    }

    break;
  default:
    break;
  }
  sockfd->sfd=sfd;
  sockfd->sfd2=sfd2;
  sockfd->nsfd=nsfd;
  sockfd->nsfd2=nsfd2;

  return(0);
}

int StartComm(int mode,struct Sockfd *sockfd){
  /* Initial comunication between server and client */
  char *buf1,*buf2;

  int sfd,nsfd;
  int sfd2,nsfd2;
  pthread_attr_t attr;
  pthread_t thread;
  
  struct Thread_arg targs; /* arguments sended to the server and client */

  int gnplayers;
  struct Parametres paramc;
  int npcc,npcs;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

  sfd=  sockfd->sfd;
  sfd2= sockfd->sfd2;
  nsfd=  sockfd->nsfd;
  nsfd2= sockfd->nsfd2;


  switch(mode){
  case 0:/* server */


    buf1=buffer1.data;
    buf2=buffer2.data;
    

    /* receive game options from client */
    recv_buf(nsfd,buf2); 
    memcpy(&paramc,buf2,sizeof(struct Parametres));
    
    fprintf(stdout,"Client parametres:\n\tnplayers:%d\n\tname: %s\n",
	   paramc.nplayers,paramc.playername);
    
    /* interpret the message */
    if(strlen(paramc.playername)>0){
      snprintf(clientname,MAXTEXTLEN,"%s",paramc.playername); 
    }
    npcc=paramc.nplayers;
    if(npcc>GameParametres(GET,GNPLAYERS,0)){
      npcc=GameParametres(GET,GNPLAYERS,0);
    }

    paramc.nplayers=npcc;
    
    fprintf(stdout,"Server parametres:\n\tnplayers:%d\n\tnpcs:%d\n",GameParametres(GET,GNPLAYERS,0),GameParametres(GET,GNPLAYERS,0)-npcc);
    
    /* send the final options */
    gnplayers=GameParametres(GET,GNPLAYERS,0);
    memcpy(buf1,&gnplayers,sizeof(int));
    memcpy(buf1+sizeof(int),&npcc,sizeof(int));
    send_buf(nsfd,buf1,2*sizeof(int));


    /* create server thread */

    targs.sfd=nsfd;
    targs.sfd2=nsfd2;
    targs.id=0;
    targs.n=0;

    pthread_create(&(thread),&attr,CommServer,(void *)(&targs));

    break;
  case 1: /* client */

    buf1=buffer1.data;
    buf2=buffer2.data;
    
    /* sending game options to server */
    
    fprintf(stdout,"HELLO CLIENT\n");
    
    fprintf(stdout,"sending client parametres:\n\tnplayers:%d\n\tname: %s\n",
	   param.nplayers,param.playername);
    
    memcpy(buf1,&param,sizeof(struct Parametres));
    send_buf(sfd,buf1,sizeof(struct Parametres));
    
    /* receiving the final options */
    
    recv_buf(sfd,buf2);
    
    gnplayers=GameParametres(GET,GNPLAYERS,0);
    memcpy(&gnplayers,buf2,sizeof(int));
    memcpy(&npcc,buf2+sizeof(int),sizeof(int));
    npcs=GameParametres(GET,GNPLAYERS,0)-npcc;
    
    fprintf(stdout,"Game:\n\tnplayers:%d \n\t npcc:%d\n\t npcs:%d\n",
	   GameParametres(GET,GNPLAYERS,0),
	   npcc,npcs);
    
    /*create client thread*/
    targs.sfd=sfd;
    targs.sfd2=sfd2;
    targs.id=0;
    targs.n=0;

    pthread_create(&(thread),&attr,CommClient,(void *)(&targs));

    break;
  default:
    break;
  }
  sleep(1);
  return(0);
}


void *CommServer(void /* struct Thread_arg */ *a){
  /*
    version 01 25Nov2010 
    Server Thread 
  */
  struct Thread_arg *args=(struct Thread_arg *)a;
  int sfd,sfd2;
  int fd;
  /*  struct IntList *kp,*ks; */
  int status;


#if COMMDEBUG
  int cont=0;
#endif
#if TESTNET
  latency.tv_sec=0;
  latency.tv_nsec=LATENCY;
#endif
  sfd=args->sfd;
  sfd2=args->sfd2;

  sem_post(&sem_barrier); 
  sem_wait(&sem_barrier1); 


  /* sending file with universe */

  if((fd=open(SAVETMPFILE,O_RDONLY))==-1){
    fprintf(stdout,"Commserver():Can't open the file: %s\n","/tmp/spacesavetmp");
    exit(-1);
  }

  fprintf(stdout,"Sending file: %s\n",SAVETMPFILE);
  SendFile(fd,sfd);
  close(fd);


  /* synchronization with main program  */
  
  sem_post(&sem_barrier);

  /* set clocks to zero */
#if TESTNET
  Clock(1,CL_CLEAR);
#endif
  /* loop of communication */
  for(;;){ /* server */

    /* synchronization with main program  */
    sem_wait(&sem_barrier1); 

#if TESTNET
    Clock(1,CL_CLEAR);
    Clock(1,CL_START);
#endif

#if COMMDEBUG
    printf("[%d]\n",cont);
    cont++;
#endif

    SendBuffer(sfd,&buffer1);
#if TESTNET
    nanosleep(&latency,NULL);
#endif

    fdatasync(sfd);

    switch(order2thread){
    case OTSENDLOAD:
#if TESTNET
      printf("LOAD\n");
#endif
      /* checking the file */
      if((fd=open(savefile,O_RDONLY))==-1){
	fprintf(stdout,"CommServer()[OTSENDLOAD]: Can't open the file: %s\n",savefile);
	exit(-1);
      }
      else{
	/* sending file with universe */
	SendFile(fd,sfd); /* copy file to buffer, send buffer */
      }
      close(fd);

      
      break;
    case OTSENDKILL:
      fprintf(stdout,"exiting...\n");
      RecvBuffer(sfd2,&buffer2);
      GameParametres(SET,GNET,FALSE);
      /*	game.quit=2; */
      close(sfd);
      close(sfd2);
      sem_post(&sem_barrier); 
      return((void *)1);

      break;
    default:
      break;
    }

    RecvBuffer(sfd2,&buffer2);
#if TESTNET
    nanosleep(&latency,NULL);
#endif
    status=ServerProcessBuffer(&buffer2);
    switch(status){
    case OTSENDKILL:
      fprintf(stdout,"client has gone\n");
      GameParametres(SET,GNET,FALSE);
      /*	game.quit=2; */
      close(sfd);
      close(sfd2);
      sem_post(&sem_barrier); 
      return((void *)1);
      
      break;
    default:
      break;
    }

#if TESTNET
    Clock(1,CL_STOP);
    printf("clock 1: %3.5f\n",Clock(1,CL_READ));
#endif 

    sem_post(&sem_barrier); 
  }
  return((void *)0);
}


void *CommClient(void /*struct Thread_arg*/ *a){
  /*
    version 01 25Nov2010 
    Client Thread 
  */
  struct Thread_arg * args=(struct Thread_arg *) a;
  char *buf;
  int sfd;
  int sfd2;
  int fd;
  int nbytes;
  int status;

  struct MessageHeader messh;
  int order;


#if COMMDEBUG
  int cont=0;
#endif

#if TESTNET
  latency.tv_sec=0;
  latency.tv_nsec=LATENCY;
#endif

  buf=buffer2.data;
  sfd=args->sfd;
  sfd2=args->sfd2;

  sem_post(&sem_barrier); 
  sem_wait(&sem_barrier1);


  /* receiving file with universe */

  if((fd=open(savefile,O_WRONLY|O_CREAT,S_IREAD|S_IWRITE|S_IRGRP|S_IROTH))==-1){
    fprintf(stdout,"CommClient(): Can't open the file: %s\n",savefile);
    exit(-1);
  }
  
  fprintf(stdout,"Receiving file...%s\n",savefile);
  
  RecvFile(fd,sfd);
  close(fd);

  fprintf(stdout,"...done\n");

  /* synchronization with main program  */

  sem_post(&sem_barrier);

  /* loop of communication */

  /* set clocks to zero */
#if  TESTNET
  Clock(1,CL_CLEAR);
#endif

  for(;;){ /* client */

  /* synchronization with main program  */
    sem_wait(&sem_barrier1); 
#if  TESTNET
    Clock(1,CL_CLEAR);
    Clock(1,CL_START);
#endif
    RecvBuffer(sfd,&buffer2);
#if  TESTNET
    printf("clock 1: %3.5f\n",Clock(1,CL_READ));
#endif
#if TESTNET
    nanosleep(&latency,NULL);
#endif
      
    nbytes=sizeof(struct MessageHeader);
    
    buf=buffer2.data;
    memcpy(&messh,buf,nbytes);
    order=messh.id;


    if(order2thread==OTSENDKILL){ /* client has gone */
      /* sending message */
      fprintf(stdout,"exiting...\n");
      order=OTSENDKILL;
      LoadBuffer(order,&buffer1,CLIENT);

      SendBuffer(sfd2,&buffer1);
      GameParametres(SET,GNET,FALSE);

      close(sfd);
      close(sfd2);
      sem_post(&sem_barrier);
      return((void *)1);
    }

    switch(order){
    case OTSENDOBJS:
      LoadBuffer(order,&buffer1,CLIENT);
      break;
    case OTSENDSAVE:
      SetModifiedAll(&listheadobjs,ALLOBJS,SENDOBJALL,FALSE);
      SetModifiedAll(&listheadobjs,PLANET,SENDOBJPLANET,TRUE);
      Setttl(&listheadobjs,0);	
      LoadBuffer(order,&buffer1,CLIENT);
      break;
    case OTSENDLOAD:
      
      fprintf(stdout,"received: load data\n");
      
      /* receiving file with universe */
      
      if((fd=open(savefile,O_WRONLY|O_CREAT,S_IREAD|S_IWRITE|S_IRGRP|S_IROTH))==-1){
	fprintf(stdout,"CommClient()[OTSENDLOAD]: Can't open the file: %s\n",savefile);
	exit(-1);
      }
      
      fprintf(stdout,"receiving file ...");
      RecvFile(fd,sfd);
      close(fd);
      
      fprintf(stdout,"...done\n");
      keys.load=TRUE;
      LoadBuffer(order,&buffer1,CLIENT);

      break;
    default:
      
      break;
    }
    
    if(order!=OTSENDLOAD){
      status=ClientProcessBuffer(&buffer2);
      
      switch(status){
      case OTSENDKILL:
	fprintf(stdout,"SERVER has gone\n");
	GameParametres(SET,GNET,FALSE);
	SendBuffer(sfd2,&buffer1);
	close(sfd);
	close(sfd2);
	sem_post(&sem_barrier);

	return((void *)1);
	break;
      default:
	break;
      }
    }

    SendBuffer(sfd2,&buffer1);

#if TESTNET
    nanosleep(&latency,NULL);
#endif
    fdatasync(sfd2);

#if TESTNET
    Clock(1,CL_STOP);
    printf("clock 1: %3.5f\n",Clock(1,CL_READ));
#endif
    sem_post(&sem_barrier); 
  }
  
  return((void *)0);
}


void PrintTextMessage(struct Message *mess){

  fprintf(stdout,"\tid: %d\n\tnobjs: %d\n\tnbytes: %d\n",
	 mess->header.id,mess->header.nobjs,mess->header.nbytes);
}



int CopyObjs2Buffer(struct Buffer *buffer,struct HeadObjList hl){
  /*
    version 02 251110
    Copy all objects with ttl <=0  belonging to proc to buffer
    return:
    the bytes copied to buffer.
  */

  struct ObjList *ls;
  Object *obj;
  int nbytes=0;
  int proc;
  int i;
  struct MessageHeader messh;
  struct NetMess mess;
  

  proc=GetProc();
  ls=hl.next;
  while(ls!=NULL ){
    if(proc!=players[ls->obj->player].proc){
      ls=ls->next;continue;
    }

    if(ls->obj->ttl>0){
      ls=ls->next;continue;
    }

    if(ls->obj->modified==SENDOBJDEAD || 
       ls->obj->modified==SENDOBJNOTSEND || 
       ls->obj->modified==SENDOBJUNMOD){
      ls=ls->next;continue;
    } 

    /* exceptions errors*/
    if(ls->obj->type==PROJECTILE){
      if(ls->obj->modified!=SENDOBJNEW){
	fprintf(stderr,"WARNING: Trying to send a projectile\n");
	fprintf(stderr,"\t id:%d type: %d mod:%d\n",ls->obj->id,ls->obj->type,ls->obj->modified);
      }
    }
    
    /* --exceptions errors*/
    obj=ls->obj;

    if(obj->type==PLANET){  /* HERE , this must be mod  */
      obj->modified=SENDOBJPLANET; 
    }

    nbytes+=CopyObj2Buffer(buffer,obj,obj->modified);
    g_nobjsend++;
    if(obj->type==PROJECTILE){
      g_nshotsend++;
    }

    SetModified(obj,SENDOBJSEND);

#if TESTNET
    if(obj->modified!=SENDOBJSEND){
      fprintf(stdout,"CopyObj2Buffer obj: %d not changed\n",obj->id);
    }
#endif
    ls=ls->next;
  }
  /**** Sending pending messages *****/
  if(textmen0.time==-1){
    nbytes+=CopyObj2Buffer(buffer,&textmen0,SENDOBJSTR);
    textmen0.time=0;
  }


  /***** Send players *****/
  {

    /* HERE continue here       */
    for(i=1;i<GameParametres(GET,GNPLAYERS,0)+2;i++){ 
      if(proc==players[i].proc && players[i].modified==SENDPLAYERMOD){
#if DEBUG
	if(debugcomm1){
	  fprintf(stdout,"send. SENDPLAYERMOD: %d %d\n",i,GetTime());/* copy 2 buffer TODO*/ 
	}
#endif
	messh.id=SENDPLAYERMOD;
	messh.nobjs=1;
	messh.nbytes=0;
	CopyMessHeader2Buffer(buffer,&messh);
	CopyPlayerMod2Buffer(buffer,&players[i]);

	players[i].ttl=2000+i;
	players[i].modified=SENDOBJUNMOD;
      } 
    } 
  }


  /***** Send Messages *****/
  
  mess.id=mess.a=mess.b=0;
  while(NetMess(&mess,NMREAD)!=0){
    messh.id=SENDMESS;
    messh.nobjs=1;
    messh.nbytes=0;
    nbytes+=CopyMessHeader2Buffer(buffer,&messh);
    nbytes+=CopyNetMess2Buffer(buffer,&mess);
  }

  nbytes+=CopyObj2Buffer(buffer,NULL,SENDEND); /* end of transmision */ 
  return(nbytes);
}


int CopyObj2Buffer(struct Buffer *buffer,void *object,int modtype){
  /*
    version 01 (011210)
    add object data to buffer
    reallocate if necessary.
    actualize buffer write position (buffer.n)
    return the bytes writed to the buffer
  */

  int n0;
  int nbytes=0;
  char *buf;
  struct MessageHeader *header;
  struct MessageHeader messh;
  Object *obj;
  struct Objectpos opos;
  struct Objectdynamic odyn; 
  struct ObjectAll oall; 
  struct ObjectAAll oaall; 
  struct ObjectNew onew;
  struct TextMessage *text;
  int kid;


  /*  if(modtype==SENDOBJKILL)return(0); */
  if(buffer->n+sizeof(struct MessageHeader)+2*sizeof(Object) > buffer->size){
    int newsize;
    newsize=(int)((buffer->size+sizeof(struct MessageHeader)+2*sizeof(Object))*1.1);
    
    buffer->data=realloc(buffer->data,newsize*sizeof(char));
    if(buffer->data==NULL){ 
      fprintf(stderr,"ERROR in malloc Copyfile2Buf()\n");
      exit(-1);
    } 
    buffer->size=newsize;
  }

  n0=buffer->n;
  buf=(buffer->data+buffer->n);

  /* copy the header */

  header=(struct MessageHeader *)buf;


  messh.id=modtype;
  obj=(Object *)object;
  

  switch (modtype){
  case SENDOBJUNMOD:
    fprintf(stderr,"WARNING CopyObj2Buffer() sending ans unmod obj (%d)%d",obj->player,obj->pid);
  case SENDOBJMOD0: /* Object modified send only position */
    messh.nobjs=1;
    messh.nbytes=sizeof(struct Objectpos);
    g_nobjtype[0]++;
    break;
  case SENDOBJMOD: /* Object modified  */
    messh.nobjs=1;
    messh.nbytes=sizeof(struct Objectdynamic);
    g_nobjtype[1]++;
    break;
  case SENDOBJAALL:
    messh.nobjs=1;
    messh.nbytes=sizeof(struct ObjectAAll);
    g_nobjtype[2]++;
    break;
  case SENDOBJALL:
    messh.nobjs=1;
    messh.nbytes=sizeof(struct ObjectAll);
    g_nobjtype[3]++;
    break;
  case SENDOBJNEW: /* new object. */
    messh.nobjs=1;
    messh.nbytes=sizeof(struct ObjectNew);
    g_nobjtype[4]++;
    break;
  case SENDOBJKILL: /* qwerty */
    messh.nobjs=1;
    messh.nbytes=2*sizeof(int);
    break;
  case SENDOBJPLANET:
    messh.nobjs=1;
    messh.nbytes=2*sizeof(int)+sizeof(float);
    g_nobjtype[5]++;
    break;
  case SENDOBJSTR:
    messh.nobjs=1;
    messh.nbytes=((struct TextMessage *)object)->n;
    break;
  case SENDEND: /* end of message */
    messh.nobjs=0;
    messh.nbytes=0;
    break;
  default:
    fprintf(stderr,"ERROR CopyObj2Buffer(): id: %d  modtype: %d\n",((Object *)object)->id,modtype);
    exit(-1);
    break;
  }


  CopyMessHeader2Buffer(buffer,&messh);

  buf=buffer->data+buffer->n;

  /* copy the object */

  switch (modtype){
  case SENDOBJUNMOD:
  case SENDOBJMOD0: /* Object modified copy only position */
    opos.id=obj->id;
    opos.x=obj->x;
    opos.y=obj->y;
/* aqui here */
    nbytes=sizeof(struct Objectpos);

    memcpy(buf,&opos,nbytes);
    buffer->n+=nbytes;
    break;
  case SENDOBJMOD: /* Object modified, copy dynamic properties  */
    nbytes=sizeof(struct Objectdynamic);

    odyn.id=obj->id;
    odyn.level=obj->level;
    odyn.habitat=obj->habitat;

    odyn.inid=0;
    if(obj->in!=NULL){
      odyn.inid=obj->in->id;
    }
    odyn.mode=obj->mode;
    odyn.x=obj->x;
    odyn.y=obj->y;
    odyn.x0=obj->x0; 
    odyn.y0=obj->y0; 
    odyn.vx=obj->vx;
    odyn.vy=obj->vy;
    odyn.a=obj->a;
    odyn.ang_v=obj->ang_v;
    odyn.ang_a=obj->ang_a;
    odyn.accel=obj->accel;
    /* odyn.gas=obj->gas; */
    odyn.state=obj->state;


    if(debugpilot&&obj->type==SHIP && obj->subtype==PILOT){
      int intype=0;
      if(obj->in!=NULL){
	intype=obj->in->type;
      }
    }
    
    /* aqui    memcpy((struct Objectdynamic *)buf,&odyn,nbytes); */
    memcpy(buf,&odyn,nbytes);
    buffer->n+=nbytes;
    break;

  case SENDOBJAALL: /* Object modified, copy dynamic properties  */
    nbytes=sizeof(struct ObjectAAll);

    oaall.id=obj->id;
    /* oaall.visible=obj->visible; */
    oaall.level=obj->level;
    /* oaall.kills=obj->kills; */
    oaall.habitat=obj->habitat;
    oaall.mode=obj->mode;
    oaall.x=obj->x;
    oaall.y=obj->y;
    oaall.x0=obj->x0;
    oaall.y0=obj->y0;
    oaall.vx=obj->vx;
    oaall.vy=obj->vy;   
    
    if(debugpilot&&obj->type==SHIP && obj->subtype==PILOT){
      int intype=0;
      if(obj->in!=NULL){
	intype=obj->in->type;
      }
    }

    oaall.a=obj->a;
    oaall.ang_v=obj->ang_v;
    oaall.ang_a=obj->ang_a;
    oaall.accel=obj->accel;
    oaall.gas=obj->gas;
    oaall.life=obj->life;
    /* oaall.shield=obj->shield; */
    oaall.state=obj->state;

    oaall.inid=0;
    if(obj->in!=NULL){
      oaall.inid=obj->in->id; 
    }


    if(debugpilot&&obj->type==SHIP && obj->subtype==PILOT){
      int intype=0;
      if(obj->in!=NULL){
	intype=obj->in->type;
      }
    }


    /* aqui    memcpy((struct ObjectAAll *)buf,&oaall,nbytes); */
    memcpy(buf,&oaall,nbytes);
    buffer->n+=nbytes;
    break;

  case SENDOBJNEW:
    nbytes=sizeof(struct ObjectNew);

    onew.id=obj->id;
    onew.player=obj->player;
    onew.type=obj->type;
    onew.subtype=obj->subtype;
    onew.durable=obj->durable;
    onew.radio=obj->radio;
    onew.damage=obj->damage;

    onew.ai=obj->ai;
    onew.modified=obj->modified;
    onew.habitat=obj->habitat;
    onew.mode=obj->mode;
    onew.x=obj->x;
    onew.y=obj->y;
    onew.vx=obj->vx;
    onew.vy=obj->vy;

    onew.a=obj->a;
    onew.gas=obj->gas;
    onew.life=obj->life;

    onew.parent=0;
    onew.inid=0;
    onew.planet=0;

    onew.engtype=obj->engine.type;

    if(obj->parent!=NULL){
      onew.parent=obj->parent->id; 
    }
    if(obj->in!=NULL){
      onew.inid=obj->in->id;
    }

    /* aqui    memcpy((struct ObjectNew *)buf,&onew,nbytes); */
    memcpy(buf,&onew,nbytes);
    buffer->n+=nbytes;

#if SENDORDERS
    nbytes=AddObjOrders2Buffer(buffer,obj);

#endif 

    break;
  case SENDOBJALL:
 
   nbytes=sizeof(struct ObjectAll);

    oall.id=obj->id;
    oall.pid=obj->pid;
    strncpy(oall.name,obj->name,OBJNAMESMAXLEN);
    oall.player=obj->player;
    oall.type=obj->type;
    oall.subtype=obj->subtype;

    oall.level=obj->level;
    oall.experience=obj->experience;
    oall.kills=obj->kills;

    oall.durable=obj->durable;
    oall.visible=obj->visible;
    oall.selected=obj->selected;
    oall.radar=obj->radar;
    oall.mass=obj->mass;

    oall.cargo=obj->cargo;
    oall.items=obj->items;
    oall.radio=obj->radio;
    oall.cost=obj->cost;
    oall.damage=obj->damage;

    oall.ai=obj->ai;
    oall.modified=obj->modified;
    oall.ttl=obj->ttl;
    oall.habitat=obj->habitat;
    oall.mode=obj->mode;

    oall.x=obj->x;
    oall.y=obj->y;
    oall.x0=obj->x0;
    oall.y0=obj->y0;
    oall.vx=obj->vx;
    oall.vy=obj->vy;
    oall.fx=obj->fx; 
    oall.fy=obj->fy; 
    oall.fx0=obj->fx0; 
    oall.fy0=obj->fy0; 

    oall.a=obj->a;
    oall.ang_v=obj->ang_v;
    oall.ang_a=obj->ang_a;
    oall.accel=obj->accel;
    oall.gas=obj->gas;
    oall.gas_max=obj->gas_max;
    oall.life=obj->life;
    oall.shield=obj->shield;
    oall.state=obj->state;

    oall.dest_r2=obj->dest_r2;
    oall.sw=obj->sw;
    oall.trace=obj->trace;

    oall.norder=obj->norder;
    memcpy(&oall.actorder,&obj->actorder,sizeof(struct Order));

    if(obj->parent!=NULL){oall.parent=obj->parent->id;}else{oall.parent=0;}
    if(obj->dest!=NULL){oall.dest=obj->dest->id;}else{oall.dest=0;}
    if(obj->in!=NULL){oall.inid=obj->in->id;}else{oall.inid=0;}

    oall.weapon=0; /* HERE choose correct weapon */
    memcpy(&oall.weapon0,&obj->weapon0,sizeof(Weapon));
    memcpy(&oall.weapon1,&obj->weapon1,sizeof(Weapon));
    memcpy(&oall.weapon2,&obj->weapon2,sizeof(Weapon));
    memcpy(&oall.engine,&obj->engine,sizeof(Engine));



    if(debugpilot&&obj->type==SHIP && obj->subtype==PILOT){
      int intype=0;
      if(obj->in!=NULL){
	intype=obj->in->type;
      }
    }

    /* aqui    memcpy((struct ObjectAAll *)buf,&oall,nbytes); */
    memcpy(buf,&oall,nbytes);
    buffer->n+=nbytes;

#if SENDORDERS
    nbytes=AddObjOrders2Buffer(buffer,obj);
#endif 

    break;
  case SENDOBJKILL:
    nbytes=sizeof(int);
    kid=obj->id;
    memcpy(buf,&kid,nbytes);

    kid=obj->sw; /* id of the killer */
    memcpy(buf+sizeof(int),&kid,nbytes);
    buffer->n+=2*nbytes;
    break;
  case SENDOBJPLANET:
    nbytes=2*sizeof(int)+sizeof(float);
    memcpy(buf,&(obj->id),sizeof(int));
    memcpy(buf+sizeof(int),&(obj->player),sizeof(int));
    memcpy(buf+2*sizeof(int),&(obj->planet->gold),sizeof(float));
    buffer->n+=nbytes;
    break;
  case SENDOBJSTR:
    text=(struct TextMessage *)object;
    nbytes=header->nbytes;
    memcpy(buf,&text->text,nbytes);

    buffer->n+=nbytes;
    break;
  case SENDEND: /* end of message */
    break;
  default:
    fprintf(stderr,"ERROR 2:CopyObj2Buffer() id:%d",modtype);
    exit(-1);
    break;
  }
  return(buffer->n-n0);
}


int ReadObjsfromBuffer(char *buf){
  /* 
     read the modified objs from the buffer and copy them to memory
     returns:
     the number of bytes readed.
   */

  struct MessageHeader header;
  Object *nobj,*objt,*parent;
  struct Objectpos objpos;
  struct Objectdynamic objdyn;
  struct ObjectAll objall;
  struct ObjectAAll objaall;

  struct ObjectNew objnew;
  int gnplayers;
  int id,projid,idkiller;
  int inid;
  int nbytes,tbytes;
  int proc;
  Data *data;

#if COMMDEBUG
  char *buf0;
  buf0=buf;
#endif


  header.id=-1;
  tbytes=0;

  proc=GetProc();
  gnplayers=GameParametres(GET,GNPLAYERS,0);

  while(header.id!=SENDEND){


    /****** 
	read the header 
    ******/ 

    memcpy(&header,buf,sizeof(struct MessageHeader));
    buf+=sizeof(struct MessageHeader);
    tbytes+=sizeof(struct MessageHeader);
#if COMMDEBUG
    printf("READED FROM BUF: %d \n\t id: %d\n\t nobjs:%d\n\t nbytes:%d\n",
	 buf-buf0,header.id,header.nobjs,header.nbytes);
#endif

    
    nobj=NULL;    
    switch(header.id){
    case SENDOBJUNMOD:
    case SENDOBJMOD0: /* only position */

      nbytes=sizeof(struct Objectpos);
      nobj=SelectObj(&listheadobjs,((struct Objectpos *)buf)->id);
      if(nobj!=NULL){
	/* aqui	  memcpy(&objpos,(struct Objectpos *)buf,nbytes); */
	  memcpy(&objpos,buf,nbytes);
	  nobj->x=objpos.x;
	  nobj->y=objpos.y;

	  nobj->x0=objpos.x;  /* x no x0 */
	  nobj->y0=objpos.y;
	  nobj->ttl=0;
	  	  /* nobj->a=objpos.a; */
      }
      else{   /* New object or object has been killed in client side*/
	fprintf(stderr,"ERROR ReadObjsfromBuffer(SENDOBJMOD0) id: %d  type:%d mod: %d doesn't exists\n",
	       ((Object *)buf)->id,((Object *)buf)->type,((Object *)buf)->modified);
	exit(-1);/*HERE TODO try to do something with this. LINE must not be reached */
      }

      buf+=nbytes;
      tbytes+=nbytes;
      break;
    case SENDOBJMOD:
      nbytes=sizeof(struct Objectdynamic);
      nobj=SelectObj(&listheadobjs,((struct Objectdynamic *)buf)->id);
      if(nobj!=NULL){
	/* aqui	  memcpy(&objdyn,(struct Objectdynamic *)buf,nbytes); */
	  memcpy(&objdyn,buf,nbytes);

	  nobj->level=objdyn.level;
	  inid=0;
	  if(nobj->in!=NULL){
	    inid=nobj->in->id;
	  }
	  if(inid!=objdyn.inid){
	    if(objdyn.inid==0){
	      nobj->in=NULL;
	    }
	    else{
	      nobj->in=SelectObj(&listheadobjs,objdyn.inid);		
	    }
	  }

	  nobj->habitat=objdyn.habitat;
	  nobj->mode=objdyn.mode;
	  nobj->x=objdyn.x;
	  nobj->y=objdyn.y;
	  nobj->x0=objdyn.x0;
	  nobj->y0=objdyn.y0;
	  nobj->vx=objdyn.vx;
	  nobj->vy=objdyn.vy;
	  nobj->a=objdyn.a;
	  nobj->ang_v=objdyn.ang_v;
	  nobj->ang_a=objdyn.ang_a;
	  nobj->accel=objdyn.accel;
	  /* nobj->gas=objdyn.gas; */
	  nobj->state=objdyn.state;
	  nobj->ttl=0;

	  if(debugpilot){
	    int inid=0,intype=0;
	    if(nobj->in!=NULL){
	      inid=nobj->in->id;
	      intype=nobj->in->type;
	    }
	  }
      }
      else{   /* New object or object has been killed in client side*/
	fprintf(stderr,"ERROR ReadObjsfromBuffer(SENDOBJMOD) id: %d  doesn't exists\n",
	       ((struct Objectdynamic *)buf)->id);
	exit(-1);/*HERE TODO try to do something with this. LINE must not be reached */
      }
      buf+=nbytes;
      tbytes+=nbytes;
      break;

    case SENDOBJAALL:
      nbytes=sizeof(struct ObjectAAll);
      nobj=SelectObj(&listheadobjs,((struct ObjectAAll *)buf)->id);
      if(nobj!=NULL){

	memcpy(&objaall,buf,nbytes);

	nobj->id=objaall.id;
	nobj->level=objaall.level;
	nobj->habitat=objaall.habitat;
	nobj->mode=objaall.mode;
	nobj->x=objaall.x;
	nobj->y=objaall.y;
	nobj->x0=objaall.x0;
	nobj->y0=objaall.y0;
	nobj->vx=objaall.vx;
	nobj->vy=objaall.vy;

	nobj->fx=0;/* objaall.fx; */
	nobj->fy=0;
	nobj->fx0=0;
	nobj->fy0=0;

	nobj->a=objaall.a;
	nobj->ang_v=objaall.ang_v;
	nobj->ang_a=objaall.ang_a;
	nobj->accel=objaall.accel;
	nobj->gas=objaall.gas;
	nobj->life=objaall.life;
	/* nobj->shield=objaall.shield; */
	nobj->state=objaall.state;
	nobj->in=NULL;
	
	nobj->ttl=0;
	if(objaall.inid!=0){
	  nobj->in=SelectObj(&listheadobjs,objaall.inid);

	  if(nobj->in!=NULL){
	    nobj->planet=NULL;
	    if(nobj->in->type==PLANET){
	      nobj->planet=nobj->in->planet;
	    }
	  }
	  else{
	    fprintf(stderr,"ERROR in ReadObjsfromBuffer(OBJAALL)in =NULL\n");
	    exit(-1);
	  }
	}

      }
      else{   /* New object or object has been killed in client side*/
	fprintf(stderr,"%d ERROR  ReadObjsfromBuffer(SENDOBJAALL) id: %d doesn't exists\n",GetTime(),
	       ((struct ObjectAAll *)buf)->id);
	exit(-1);/*HERE TODO try to do something with this. LINE must not be reached */
      }
      buf+=nbytes;
      tbytes+=nbytes;
      break;

    case SENDOBJALL:
      nbytes=sizeof(struct ObjectAll);
      nobj=SelectObj(&listheadobjs,((struct ObjectAll *)buf)->id);

      if(nobj==NULL){    /* the object doesn't exist */
	fprintf(stderr,"\nERROR en ReadObjsfromBuffer(SENDOBJALL): Object %d doesn't exists\n",((Object *)buf)->id);
	buf+=nbytes;
	tbytes+=nbytes;
	exit(-1);
	break;
      }

      memcpy(&objall,buf,nbytes);
      
      /*      memcpy(&obj0,&nobj,nbytes); */
      data=nobj->cdata;
      parent=nobj->parent;

      /***********************/
      /* CopyObject(nobj,obj); */
      nobj->id=objall.id;
      nobj->pid=objall.pid;
      strncpy(nobj->name,objall.name,OBJNAMESMAXLEN);
      nobj->player=objall.player;
      nobj->type=objall.type;
      nobj->subtype=objall.subtype;
      
      nobj->level=objall.level;
      nobj->experience=objall.experience;
      nobj->pexperience=0;
      nobj->kills=objall.kills;
      
      nobj->durable=objall.durable;
      nobj->visible=objall.visible;
      nobj->selected=objall.selected;
      nobj->radar=objall.radar;
      nobj->mass=objall.mass;
      nobj->cargo=objall.cargo;

      if(nobj->items & ITPILOT){
	if(!(objall.items & ITPILOT)){
	  if(nobj->type==SHIP && nobj->subtype==PILOT){
	    if(debugpilot){
	      fprintf(stdout,"checking for pilots in %d (%d)\n",nobj->id,nobj->pid);
	    }
	    EjectPilotsObj(&listheadobjs,nobj);
	  }
	}
      }

      nobj->items=objall.items;
      nobj->radio=objall.radio;
      nobj->cost=objall.cost;
      nobj->damage=objall.damage;
      
      nobj->ai=objall.ai;
      nobj->modified=objall.modified;
      nobj->ttl=objall.ttl;
      nobj->habitat=objall.habitat;
      nobj->mode=objall.mode;
      
      nobj->x=objall.x;
      nobj->y=objall.y;
      nobj->x0=objall.x0;
      nobj->y0=objall.y0;
      nobj->vx=objall.vx;
      nobj->vy=objall.vy;
      nobj->fx=objall.fx; 
      nobj->fy=objall.fy; 
      nobj->fx0=objall.fx0; 
      nobj->fy0=objall.fy0; 
      
      nobj->a=objall.a;
      nobj->ang_v=objall.ang_v;
      nobj->ang_a=objall.ang_a;
      nobj->accel=objall.accel;
      nobj->gas=objall.gas;
      nobj->gas_max=objall.gas_max;
      nobj->life=objall.life;
      nobj->shield=objall.shield;
      nobj->state=objall.state;
      
      nobj->dest_r2=objall.dest_r2;
      nobj->sw=objall.sw;
      nobj->trace=objall.trace;
      
      nobj->norder=objall.norder;
      memcpy(&nobj->actorder,&objall.actorder,sizeof(struct Order));


      memcpy(&nobj->weapon0,&objall.weapon0,sizeof(Weapon));
      memcpy(&nobj->weapon1,&objall.weapon1,sizeof(Weapon));
      memcpy(&nobj->weapon2,&objall.weapon2,sizeof(Weapon));
      memcpy(&nobj->engine,&objall.engine,sizeof(Engine));

      /***********************************/

      nobj->cdata=data;
      nobj->parent=parent;

      /*      nobj->modified=0;       */
      /*      nobj->norder=0; */

      /* HERE save as data */
      /* nobj->parent=NULL; */
      nobj->dest=NULL;
      nobj->in=NULL;
      nobj->planet=NULL;
      nobj->lorder=NULL;
      nobj->weapon=NULL;
      
      objt=NULL;

      nobj->dest=SelectObj(&listheadobjs,(objall.dest)); /* HERE one function for all the pointers */
      /* nobj->parent=SelectObj(&listheadobjs,(int)(obj->parent)); */
      nobj->in=SelectObj(&listheadobjs,objall.inid);


      if(debugpilot){
	int inid=0,intype=0;
	if(nobj->in!=NULL){
	  inid=nobj->in->id;
	  intype=nobj->in->type;
	}
      }

      if(nobj->in!=NULL){

	nobj->planet=NULL;

	if(nobj->in->type==PLANET){
	  nobj->planet=nobj->in->planet;
	}
	if(nobj->in->type==SHIP){
	  nobj->in->items=nobj->in->items| ITPILOT;
	}
      }
      else{
	if(debugpilot&&nobj->type==SHIP && nobj->subtype==PILOT){
	  fprintf(stdout,"%d pilot received(%d): player:%d id: %d pid: %d in=NULL objallin:%d\n",
		 GetTime(),header.id,nobj->player,nobj->id,nobj->pid,objall.inid);
	}

	if(objall.inid!=0){
	  fprintf(stderr,"ERROR in ReadObjsfromBuffer(id: %d)in =NULL\n",nobj->id);
	  exit(-1);
	}
      }
      nobj->weapon=&nobj->weapon0;
      nobj->ttl=0;
      buf+=nbytes;
      tbytes+=nbytes;

#if SENDORDERS
      nbytes=CopyObjOrdersfromBuffer(nobj,buf);

      buf+=nbytes;
      tbytes+=nbytes;
#endif 
#if DEBUG
      if(debugcomm1){
	fprintf(stdout,"recv SENDOBJALL: %d %d %d\n",nobj->id,nobj->level,nobj->engine.v2_max);
      }
#endif
      break;
    case SENDOBJPLANET:
      nbytes=2*sizeof(int)+ sizeof(float);
      memcpy(&id,buf,sizeof(int));
      nobj=SelectObj(&listheadobjs,id);
      if(nobj==NULL){
	fprintf(stderr,"ERROR ReadObjsfromBuffer(SENDOBJPLANET) id: %d doesn't exists\n",id);
	exit(-1);
      }
      /* memcpy(&player,buf+sizeof(int),sizeof(int)); */
      
      if(nobj->modified==SENDOBJSEND && nobj->sw){
	nobj->sw=0;
      }
      else{
	memcpy(&(nobj->player),buf+sizeof(int),sizeof(int));
	memcpy(&(nobj->planet->gold),buf+2*sizeof(int),sizeof(float));
      }
      
      /**** check if I a have that information 
       received ally conquered planets ****/

      if(players[nobj->player].team==players[actual_player].team){
	if(IsInIntList((players[actual_player].kplanets),nobj->id)==0){
	  players[actual_player].kplanets=Add2IntList((players[actual_player].kplanets),nobj->id);
	}
      }
      

      buf+=nbytes;
      tbytes+=nbytes;
      break;
    case SENDOBJKILL:
      nbytes=sizeof(int);
      memcpy(&id,buf,nbytes);
      buf+=nbytes;
      tbytes+=nbytes;
      memcpy(&idkiller,buf,nbytes);
      buf+=nbytes;
      tbytes+=nbytes;
      nobj=SelectObj(&listheadobjs,id);
      
      if(nobj==NULL){
	fprintf(stderr,"ERROR ReadObjsfromBuffer(SENDOBJKILL) id: %d doesn't exists\n",id);
	/*exit(-1);*/ /*HERE TODO try to do something with this. LINE must not be reached */
      }
      else{
	nobj->state=0;
	nobj->modified=SENDOBJDEAD;/*	RemoveObj(nobj); */
	nobj->sw=idkiller;
      }
      
      break;
    case SENDOBJNEW:
      id=g_objid;
      projid=g_projid;
      nbytes=sizeof(struct ObjectNew);

      /* aqui      memcpy(&objnew,(struct ObjectNew *)buf,nbytes); */
      memcpy(&objnew,buf,nbytes);

      nobj=SelectObj(&listheadobjs,objnew.id);
      if(nobj!=NULL){    /* the object exist */
	fprintf(stderr,"\nERROR in ReadObjsfromBuffer(SENDOBJNEW): Object %d exists type:%d stype:%d proc:%d\n",nobj->id,nobj->type,nobj->subtype,players[nobj->player].proc);
	buf+=sizeof(struct ObjectNew);
	tbytes+=nbytes;
	exit(-1); /*HERE TODO try to do something with this. LINE must not be reached */
      }


      /*      obj=(struct ObjectNew *)buf; */

      parent=SelectObj(&listheadobjs,objnew.parent);

      nobj=NewObj(objnew.type,objnew.subtype,
		  objnew.x,objnew.y,
		  objnew.vx,objnew.vy,
		  CANNON0,objnew.engtype,objnew.player,parent,NULL);

      if(nobj==NULL){
	fprintf(stderr,"\nERROR in ReadObjsfromBuffer(): NewObj() returns NULL\n");
	fprintf(stderr,"\t object not created. Exiting ...\n");
	exit(-1);
      }

      if(parent==NULL &&nobj->type!=SHIP && nobj->type!=ASTEROID){    /*  */
	fprintf(stderr,"\nWARNING in ReadObjsfromBuffer(SENDOBJNEW): id:%d type:%d(parent) obj %d doesn't exists\n",nobj->id,nobj->type,((struct ObjectNew *)buf)->parent);
	/*	buf+=sizeof(Object); */
	/*	exit(-1); */
      }


      g_objid=id;
      g_projid=projid;

      nobj->id=objnew.id;
      /* nobj->player=objnew.player; */

      if(nobj->player!=objnew.player){
	fprintf(stderr,"ERROR readobjsfrombuffer\n");
	exit(-1);
      }
      nobj->type=objnew.type;
      nobj->subtype=objnew.subtype;
      nobj->durable=objnew.durable;

      nobj->radio=objnew.radio;
      nobj->damage=objnew.damage;
      nobj->ai=objnew.ai;
      nobj->modified=objnew.modified;
      nobj->habitat=objnew.habitat;
      nobj->mode=objnew.mode;
      nobj->x=objnew.x;
      nobj->y=objnew.y;
      nobj->vx=objnew.vx;
      nobj->vy=objnew.vy;

      nobj->a=objnew.a;
      nobj->gas=objnew.gas;
      nobj->life=objnew.life;

      nobj->norder=0;
      nobj->parent=parent;
      nobj->dest=NULL;
      nobj->in=NULL;
      nobj->planet=NULL;
      nobj->lorder=NULL;
      nobj->weapon=&nobj->weapon0;
      nobj->ttl=0;
      objt=NULL;

      if(objnew.inid!=0){
	nobj->in=SelectObj(&listheadobjs,objnew.inid);
	
	if(nobj->habitat==H_SPACE){
	    fprintf(stderr,"ERROR in ReadObjsfromBuffer(OBJNEW)in !=NULL\n");
	    exit(-1);
	}
      }

      objt=SelectObj(&listheadobjs,objnew.planet);
      if(objt!=NULL){ 
 	nobj->planet=objt->planet; 
      } 

      Add2ObjList(&listheadobjs,nobj);
      buf+=nbytes;
      tbytes+=nbytes;

#if SENDORDERS
      nbytes=CopyObjOrdersfromBuffer(nobj,buf);
      buf+=nbytes;
      tbytes+=nbytes;
#endif 

      break;

    case SENDOBJSTR:
      if(header.nbytes>MAXTEXTLEN || header.nbytes<0){
	fprintf(stderr,"ERROR in SENDOBJSTR : %d\n",header.nbytes);
	exit(-1);
      }
      textmen1.n=header.nbytes;
      textmen1.time=100;
      strncpy(textmen1.text,buf,header.nbytes);
      strncpy(textmen1.text+header.nbytes,"\0",1);
      fprintf(stdout,"MEN RECV: %s\n",textmen1.text);

      if(strncmp(textmen1.text,"GAME PAUSED",12)==0){
	textmen1.time=4;
      }

      buf+=header.nbytes;
      tbytes+=header.nbytes;
      break;


    case SENDPLAYER:
      {
	struct HeadIntIList ks;
	struct IntList *kp;
	struct PlayerAll playerall;
	struct Player *player;

#if DEBUG
	fprintf(stdout,"SENDPLAYER\n");
#endif
	nbytes=sizeof(struct PlayerAll);
	memcpy(&playerall,buf,nbytes); 

	/* DelIntIList(&(players[playerall.id].ksectors)); */
	kp=players[playerall.id].kplanets;

	memcpy(&ks,&players[playerall.id].ksectors,sizeof(struct HeadIntIList));
	/****/
	player=&players[playerall.id];

	strncpy(player->playername,playerall.playername,MAXTEXTLEN);

	player->id=playerall.id;
	player->pid=playerall.pid;
	player->proc=playerall.proc;
	player->control=playerall.control;
	player->team=playerall.team;
	player->profile=playerall.profile;
	player->strategy=playerall.strategy;
	player->maxlevel=playerall.maxlevel;
	player->color=playerall.color;
	player->cv=playerall.cv;
	player->nplanets=playerall.nplanets;
	player->nships=playerall.nships;
	player->nbuildships=playerall.nbuildships;
	player->gold=playerall.gold;
	player->balance=playerall.balance;
	player->lastaction=playerall.lastaction;
	player->ndeaths=playerall.ndeaths;
	player->nkills=playerall.nkills;
	player->points=playerall.points;
	player->modified=playerall.modified;
	player->ttl=playerall.ttl;


	/****/


	players[playerall.id].kplanets=kp;
	memcpy(&players[playerall.id].ksectors,&ks,sizeof(struct HeadIntIList));

	players[playerall.id].ttl=2000;
	players[playerall.id].modified=SENDOBJUNMOD;
	

	buf+=nbytes;
	tbytes+=nbytes;
      }
      break;

    case SENDPLAYERMOD:
      {
	struct PlayerMod pmod;
	int pid;

	nbytes=sizeof(struct PlayerMod);
	memcpy(&pmod,buf,nbytes);
	pid=pmod.id;
#if DEBUG
	if(debugcomm1){
	  fprintf(stdout,"recv. SENDPLAYERMOD %d %d %d\n",pid,pmod.maxlevel,GetTime());
	}
#endif
	players[pid].maxlevel=pmod.maxlevel;
	players[pid].nplanets=pmod.nplanets;
	players[pid].nships=pmod.nships;
	players[pid].nbuildships=pmod.nbuildships;
	players[pid].gold=pmod.gold;
	players[pid].ndeaths=pmod.ndeaths;
	players[pid].nkills=pmod.nkills;
	players[pid].points=pmod.points;
	players[pid].ttl=2000;
	players[pid].modified=SENDOBJUNMOD;

	buf+=nbytes;
	tbytes+=nbytes;
      }
      break;

    case SENDMESS:
      {
	struct NetMess mess;

	nbytes=sizeof(struct NetMess);
	memcpy(&mess,buf,nbytes);
	buf+=nbytes;
	tbytes+=nbytes;

	switch(mess.id){
	case NMPLANETDISCOVERED:
	  {
	    Object *obj,*pnt;
	    int i;
	    char text[MAXTEXTLEN];
	    obj=SelectObj(&listheadobjs,mess.a);
	    pnt=SelectObj(&listheadobjs,mess.b);
	    
	    if(obj!=NULL && pnt!=NULL){
	      for(i=0;i<=gnplayers+1;i++){
		if( (i!=obj->player) && ((players[obj->player].team==players[i].team) || GameParametres(GET,GENEMYKNOWN,0)) ){
		  if(GetProc()==players[i].proc){
		    if(IsInIntList((players[i].kplanets),pnt->id)==0){
		      players[i].kplanets=Add2IntList((players[i].kplanets),pnt->id);
		      snprintf(text,MAXTEXTLEN,"(%s) PLANET %d discovered",players[obj->player].playername,pnt->id);
		      if(!Add2TextMessageList(&listheadtext,text,obj->id,i,0,100,0)){
			Add2CharListWindow(&gameloglist,text,0,&windowgamelog);
		      }
		    }
		  }
		}
	      }
	    }
	    else{
	      fprintf(stderr,"ERROR ReadObjsFromBuffer(): obj or planet NULL %p %p\n",
		      (void *)obj,(void *)pnt);
	      fprintf(stderr,"\tmessage discarded\n");
	    }
	  }
	  break;
	case NMPLANETLOST:
	  {
	    char text[MAXTEXTLEN];
	    if(GetProc()==players[mess.a].proc){
	      snprintf(text,MAXTEXTLEN,"PLANET %d LOST",mess.b);
	      if(!Add2TextMessageList(&listheadtext,text,mess.b,mess.a,0,100,2)){
		Add2CharListWindow(&gameloglist,text,0,&windowgamelog);
	      }
	    }
	  }
	  break;
	default:
	  fprintf(stderr,"ERROR. ReadObjsFromBuffer() SENDMESS. Not implemented %d\n",mess.id);
	  exit(-1);
	  break;
	}
      }
      break;
      
    case SENDEND:
      break;
    default:
      fprintf(stderr,"ERROR ReadObjsfromBuffer() header id %d unknown\n",header.id);
      exit(-1);
      break;
    }

    if(nobj!=NULL){
      nobj->trace=FALSE;
      if(nobj->mode==LANDED){ /*accel must be zero */
	nobj->accel=0;
      }

      if(proc!=players[nobj->player].proc){
	nobj->ttl=0;
      }
      
      if(cv==nobj){
	habitat.type=cv->habitat;
	habitat.obj=cv->in;
      }
      /* check for pilots */
      if((nobj->items & ITPILOT)){

	/* when landed */
	if(nobj->mode==LANDED){
	  EjectPilotsObj(&listheadobjs,nobj);
	  nobj->items=(nobj->items)&(~ITPILOT);
	}

	/* when destroyed */
	if(nobj->modified==SENDOBJDEAD){
	  EjectPilotsObj(&listheadobjs,nobj);
	}
      }

      /* --check for pilots */

    }
  }
  return(tbytes);
}/* int ReadObjsfromBuffer(char *buf){ */


void SendTextMessage(char *mess){
  int l;
  l=strlen(mess);
  if(l>MAXTEXTLEN)l=MAXTEXTLEN;
#if DEBUG
  if(debugcomm1){
    fprintf(stdout,"Sending: %s\n",mess);
  }
#endif
  memcpy(textmen0.text,mess,l);
  textmen0.n=l;
  textmen0.time=-1;
}

int PendingTextMessage(void){
  if(textmen1.time>0)return(1);
  return(0);
}

void GetTextMessage(char *mess){
  int l;
  l=strlen(textmen1.text);
  if(l>MAXTEXTLEN)l=MAXTEXTLEN;
  memcpy(mess,textmen1.text,l);
  memcpy(mess+l,"\0",1);
  textmen1.time--;
}

void TextMessage(int action){

  switch(action){
  case 0:
    break;
  case 1:
    break;
  case 2:
    break;
  default:
    break;
  }
}

int CopyObjOrdersfromBuffer(Object *obj0,char *buf0){

  /*
    Copy the orders of the object obj from the buffer
    returns:
    the number of bytes writed.
   */
  struct Order order;
  char *buf;
  int nbytes,tbytes;
  int i;
  int norders;
  int nordread=0;

  tbytes=0;
  buf=buf0;

  /* reading norders*/
  
  nbytes=sizeof(int);   /*  norders */
  memcpy(&norders,buf,nbytes);
  buf+=nbytes;
  tbytes+=nbytes;
  
  DelAllOrder(obj0);
  
  /* rest of orders */
  
  obj0->lorder=NULL;
  obj0->norder=0;
  nbytes=sizeof(struct Order);

  for(i=0;i<norders;i++){
    memcpy(&order,buf,nbytes);
    AddOrder(obj0,&order);
    buf+=nbytes;
    tbytes+=nbytes;
    nordread++;
#if COMMDEBUG 
    fprintf(stdout,"%d %d %d %d\n", 
	   order.priority,order.id,order.time,order.g_time); 
    fprintf(stdout,"%g %g %g %g\n%g %g %g %g\n", 
	   order.a,order.b,order.c,order.d, 
	   order.e,order.f,order.g,order.h); 
#endif
  }
#if COMMDEBUG 
  fprintf(stdout,"n orders readed:%d\n",nordread); 
#endif
  
  return(tbytes);
}



int SetModified(Object *obj,int mode){
  /*
    version 01
    returns:
    0 if the obj is modified to mode
    1 if its modified to other mode
    2 not modified, error
   */
  int sw=0;
  int swmod=0;

/* object modify types, used in transmission buffer */

/* #define SENDOBJUNMOD   32 /\* obj unmodified, don't send *\/ */
/* #define SENDOBJMOD0    33 /\* obj modified, send only position *\/ */
/* #define SENDOBJMOD     34 /\* obj modified, send more information *\/ */
/* #define SENDOBJAALL    35 /\* obj modified, send almost all info *\/ */
/* #define SENDOBJALL     36 /\* obj modified, send all info *\/ */
/* #define SENDOBJKILL    37 /\* delete the object  *\/ */
/* #define SENDOBJNEW     38 /\* new obj *\/ */
/* #define SENDOBJDEAD    39 /\* don't send and remove *\/ */
/* #define SENDOBJPLANET  40 /\* send all info planet *\/ */
/* #define SENDOBJSTR     41 /\* send a text message *\/ */
/* #define SENDOBJSEND     42 /\* object just sended *\/ */
/* #define SENDOBJNOTSEND 43 /\*don't send *\/ */

  if(obj==NULL)return(2);

  if(mode==obj->modified)return(0);

#if DEBUG
  if(GetProc()!=players[obj->player].proc){
    if(obj->type==ASTEROID){
      if(debugcomm1){
	fprintf(stdout,"SetModified():PROC ASTEROID Debug Warning :different proc, trying to change mode (%d) %d to %d\n",
	       obj->id,obj->modified,mode);
      }
    }
  }
#endif

  if(GetProc()!=players[obj->player].proc){
    /* mode allowed for different processes */
    switch(mode){
    default:
      break;
    }
#if DEBUG    
    if(debugcomm1){
      fprintf(stdout,"SetModified():Debug Warning. different proc PROC: %d, trying to change mode (%d) %d to %d time: %d\n",
	     players[obj->player].proc,obj->id,
	     obj->modified,mode,GetTime());
      swmod=1;
    }
#endif
  }

  if(GameParametres(GET,GMODE,0)==LOCAL){
#if DEBUG
    if(debugcomm1){
      fprintf(stdout,"Warning: SetModified() in local mode. player:%d type: %d \n",obj->player,obj->type);
      obj->modified=SENDOBJUNMOD;
    }
#endif
    return(0);
  }

#if DEBUG
  if(mode==SENDOBJDEAD && obj->modified!= SENDOBJSEND && obj->type==ASTEROID){
    if(debugcomm1){
      fprintf(stdout,"SetModified():ASTEROID Debug Warning forbidden. trying to change mode (%d) %d to %d\n",
	     obj->id,obj->modified,mode);
    }
  }
#endif

  /* forbidden changes */ 

  if(mode==SENDOBJNEW){ /* SENDOBJNEW only in NewObj() */
#if DEBUG
    if(debugcomm1){
      fprintf(stdout,"SetModified(): Debug Warning forbidden.trying to change mode (%d) %d to %d\n",
	     obj->id,obj->modified,mode);
    }
#endif
    return(2);
  }
  if(0&&mode==SENDOBJNOTSEND){ /* SENDOBJNOTSEND only in NewObj() */
#if DEBUG
    if(debugcomm1){
      fprintf(stdout,"SetModified(): Debug Warning forbidden. trying to change mode (%d) %d to %d\n",
	     obj->id,obj->modified,mode);
    }
#endif
    return(2);
  }
  if(0&&mode==SENDOBJDEAD){ /* SENDOBJDEAD only in exceptions below */
#if DEBUG
    if(debugcomm1){
      fprintf(stdout,"SetModified(): Debug Warning forbidden.trying to change mode (%d) %d to %d\n",
	     obj->id,obj->modified,mode);
    }
#endif
    return(2);
  }

/* --forbidden changes */ 


/******** PLANETS AND PROJECTILES ******/
  switch(obj->type){
  case PLANET:
    switch(obj->modified){
    case SENDOBJUNMOD:
      switch(mode){
      case SENDOBJUNMOD:
	break;
      case SENDOBJMOD0:
      case SENDOBJMOD:
      case SENDOBJAALL:
      case SENDOBJALL:
      case SENDOBJPLANET:
	obj->modified=SENDOBJPLANET;
	break;
      default:
#if DEBUG
	if(debugcomm1){
	  fprintf(stdout,"Debug Warning 0 SetModified()PLANET:trying to change mode (%d) %d to %d\n",
		 obj->id,obj->modified,mode);
	}
#endif
	break;
      }
      break;
    case SENDOBJPLANET:
      switch(mode){

      case SENDOBJMOD0:
      case SENDOBJMOD:
      case SENDOBJAALL:
      case SENDOBJALL:
      case SENDOBJPLANET:
	break;
      case SENDOBJSEND:
	obj->modified=mode;
	break;
      default:
#if DEBUG
	if(debugcomm1){
	  fprintf(stdout,"Debug Warning 1 SetModified()PLANET:trying to change mode (%d) %d to %d\n",
		 obj->id,obj->modified,mode);
	}
#endif
	  break;
      }
      break;
    case SENDOBJSEND:
      switch(mode){
      case SENDOBJUNMOD:
	obj->modified=mode;
	break;
      default:
#if DEBUG
	if(debugcomm1){
	  fprintf(stdout,"Debug Warning 2 SetModified()PLANET:trying to change mode (%d) %d to %d\n",
		 obj->id,obj->modified,mode);
	}
#endif
	break;
      }
      
      break;
    default:
#if DEBUG
      if(debugcomm1){
	fprintf(stdout,"Debug Warning 3 SetModified()PLANET:state not allowed (%d) %d to %d\n",
	       obj->id,obj->modified,mode);
      }
#endif
      break;
    }
    
    break;
  case PROJECTILE:
    switch(obj->modified){

    case SENDOBJUNMOD:
	obj->modified=SENDOBJNOTSEND;
      break;

    case SENDOBJNEW:
      switch(mode){
      case SENDOBJMOD0:
      case SENDOBJMOD:
      case SENDOBJAALL:
      case SENDOBJALL:
	break;
      case SENDOBJKILL:
#if DEBUG
	if(obj->type!=PROJECTILE){
	  if(debugcomm1){
	    fprintf(stdout,"SetModified(): id %d (%d) from NEW to DEAD\n",obj->id,obj->type);
	  }
	}
#endif
	obj->modified=SENDOBJDEAD;
	break;
      case SENDOBJSEND:
      case SENDOBJDEAD:
      case SENDOBJNOTSEND:
	obj->modified=mode;
	break;
      default:
#if DEBUG
	if(debugcomm1){
	  fprintf(stdout,"SetModified():Debug Warning 1. PROJECTILE:trying to change mode (%d) %d to %d\n",
		 obj->id,obj->modified,mode);
	}
#endif
	return(2);
	break;
      }
      break;

    case SENDOBJSEND:
      switch(mode){
      case SENDOBJDEAD:
      case SENDOBJSEND:
      case SENDOBJNOTSEND:
	obj->modified=mode;
	break;
      case SENDOBJUNMOD:
      case SENDOBJMOD0:
	if(obj->type==PROJECTILE){
	  if(obj->subtype==MISSILE){
	    obj->modified=mode;
	  }
	  else{
#if DEBUG
	    if(debugcomm1){
	      fprintf(stdout,"Debug Warning 2 SetModified()PROJECTILE:trying to change mode (%d) %d to %d\n",
		     obj->id,obj->modified,mode);
	    }
#endif
	    return(2);
	  }
	}
	else{
	  obj->modified=mode;
	}
	break;
	
      default:
#if DEBUG
	if(debugcomm1){
	  fprintf(stdout,"Debug Warning 3 SetModified()PROJECTILE:trying to change mode (%d) %d to %d\n",
		 obj->id,obj->modified,mode);
	}
#endif
	return(2);
	break;
      }
      
      break;

    case SENDOBJNOTSEND:
      switch(mode){
      case SENDOBJMOD0:
      case SENDOBJMOD:
      case SENDOBJAALL:
      case SENDOBJALL:
	break;
      case SENDOBJKILL:
      case SENDOBJDEAD:
	obj->state=0;
	obj->modified=SENDOBJDEAD;
	break;
      default:
#if DEBUG
	if(debugcomm1){
	  fprintf(stdout,"Debug Warning 3 SetModified()PROJECTILE:trying to change mode (%d) %d to %d\n",
		 obj->id,obj->modified,mode);
	}
#endif
	return(2);
	break;
      }
      break;

    case SENDOBJDEAD:
      break;

    default:
#if DEBUG
      if(debugcomm1){
	printf("Debug Warning 4 SetModified():PROJECTILE (%d) with modified: %d trying to change to %d\n",
	       obj->id,obj->modified,mode);
      }
#endif
      break;
    }
    break;

  /****** SHIPS AND ASTEROIDS *******************/
  case SHIP:
  case ASTEROID:
    
    switch(obj->modified){
      
    case SENDOBJUNMOD:
      
      switch(mode){
      case SENDOBJUNMOD:  /* HERE check what use this */
#if DEBUG
	if(debugcomm1){
	  printf("Debug Warning 5 SetModified():trying to change mode (%d) %d to %d\n",
		 obj->id,obj->modified,mode);
	}
#endif
	obj->modified=mode;
	break;
      case SENDOBJMOD0:
      case SENDOBJMOD:
      case SENDOBJAALL:
      case SENDOBJALL:
      case SENDOBJKILL:
      case SENDOBJPLANET:
	obj->modified=mode;
	break;
      case SENDOBJSEND:    /* HERE why send unmod objs */
#if DEBUG
	if(debugcomm1){
	  printf("Debug Warning 6 SetModified():trying to change mode (%d type:%d) %d to %d\n",
		 obj->id,obj->type,obj->modified,mode);
	}
#endif
	obj->modified=mode;
	break;
      default:
#if DEBUG
	if(debugcomm1){
	  printf("Debug Warning 7 SetModified():trying to change mode (%d) %d to %d\n",
		 obj->id,obj->modified,mode);
	}
#endif
	return(2);
	break;
      }
      
      
      break;
    case SENDOBJMOD0:
    case SENDOBJMOD:
    case SENDOBJAALL:
    case SENDOBJALL:
      switch(mode){

      case SENDOBJMOD0:
      case SENDOBJMOD:
      case SENDOBJAALL:
      case SENDOBJALL:
	if(obj->modified<mode){ /* HERE dangerous */
	  obj->modified=mode;
	}
	
	break;
      case SENDOBJKILL:
      case SENDOBJSEND:
	obj->modified=mode;
	break;
	
      default:
#if DEBUG
      if(debugcomm1){
	printf("Debug Warning 8 SetModified():trying to change mode (%d) %d to %d\n",
	       obj->id,obj->modified,mode);
      }
#endif
	return(2);
	break;
      }
      break;
      
    case SENDOBJKILL:
      switch(mode){
      case SENDOBJMOD0:
      case SENDOBJMOD:
      case SENDOBJAALL:
      case SENDOBJALL:
	break;
      case SENDOBJSEND:
	obj->modified=mode;
	break;
      case SENDOBJKILL:
	break;
      default:
#if DEBUG
	if(debugcomm1){
	  printf("Debug Warning 9 SetModified():trying to change mode (%d)type: %d (%d) mod:  %d to %d\n",
		 obj->id,obj->type,obj->subtype,obj->modified,mode);
	}
#endif
	return(2);
	break;
      }
      break;
      
    case SENDOBJNEW:
      switch(mode){
      case SENDOBJMOD0:
      case SENDOBJMOD:
      case SENDOBJAALL:
      case SENDOBJALL:
	break;
	
      case SENDOBJKILL:
      case SENDOBJDEAD:
	obj->modified=SENDOBJDEAD;
	break;
      case SENDOBJSEND:
	obj->modified=SENDOBJSEND;
	break;
      default:
#if DEBUG
	if(debugcomm1){
	  printf("Debug Warning 10 SetModified():trying to change mode (%d) %d to %d\n",
		 obj->id,obj->modified,mode);
	}
#endif
	return(2);
	break;
      }
      break;
      
    case SENDOBJDEAD:
      switch(mode){
      case SENDOBJMOD0:
      case SENDOBJMOD:
      case SENDOBJAALL:
      case SENDOBJALL:
      case SENDOBJKILL:
      case SENDOBJNOTSEND:
	break;
      default:
#if DEBUG
	if(debugcomm1){
	  printf("Debug Warning 11 SetModified():trying to change mode (%d) %d to %d\n",
		 obj->id,obj->modified,mode);
	}
#endif
	return(2);
	break;
      }
      break;
      
    case SENDOBJPLANET:
      switch(mode){
      case SENDOBJSEND:
	obj->modified=mode;
	break;
      default:
#if DEBUG
	if(debugcomm1){
	  printf("Debug Warning 12 SetModified():trying to change mode (%d) %d to %d\n",
		 obj->id,obj->modified,mode);
	}
#endif
	return(2);
	break;
      }
      break;
      
    case SENDOBJSTR:   /* HERE this line is reached ?? */
      switch(mode){
      case SENDOBJSEND:
	obj->modified=mode;
	break;
      default:
#if DEBUG
      if(debugcomm1){
 	printf("Debug Warning 13 SetModified():trying to change mode (%d) %d to %d\n",
	       obj->id,obj->modified,mode);
      }
#endif
	return(2);
	break;
      }
      break;
      
    case SENDOBJSEND:
      switch(mode){
      case SENDOBJUNMOD:
      case SENDOBJDEAD:
      case SENDOBJSEND:
      case SENDOBJNOTSEND:
	obj->modified=mode;
	break;
      default:
#if DEBUG
	if(debugcomm1){
	  printf("Debug Warning 14 SetModified(SENDOBJSEND):trying to change mode (%d) %d to %d\n",
		 obj->id,obj->modified,mode);
	}
#endif
	return(2);
	break;
      }
      break;
    case SENDOBJNOTSEND:
      switch(mode){
      case SENDOBJMOD0:
      case SENDOBJMOD:
      case SENDOBJAALL:
      case SENDOBJALL:
	break;
      case SENDOBJKILL:
      case SENDOBJDEAD:
	obj->modified=SENDOBJDEAD;
	break;
      case SENDOBJNOTSEND:
	break;
      default:
#if DEBUG
      if(debugcomm1){
	printf("Debug Warning 15 SetModified():trying to change mode (%d) %d to %d\n",
	       obj->id,obj->modified,mode);
      }
#endif
	return(2);
	break;
      }
      break;
    default:
      fprintf(stderr,"SetModified(): ERROR: obj mode  %d not known\n",
	      obj->modified);
      exit(-1);
      break;
    }
    break;
  default:
    break;
  }

  if(swmod){
    fprintf(stdout,"\t SetModified(): obj %d modified: to %d  mode: %d time: %d\n",
	   obj->id,
	   obj->modified,mode,GetTime());
  }

  return(sw);
}

int SetModifiedAll(struct HeadObjList *lh,int type,int mode,int force){
  /*
    Reset the modified variable on all objects of type type to mode mode
    return the number of objects modified
  */

  struct ObjList *ls;
  int n=0;
  int proc=0;


  proc=GetProc();
  ls=lh->next;
  while(ls!=NULL){
    if(proc!=players[ls->obj->player].proc){ls=ls->next;continue;}

    if(ls->obj->type==type || type==ALLOBJS){
      if(force==TRUE){
	ls->obj->modified=mode;
      }
      else{
	SetModified(ls->obj,mode);
      }
      n++;
      
    }
    ls=ls->next;
  }
  return(n);
}


int CheckModifiedPre(struct HeadObjList *lh,int proc){
  /*
    Check the modified parameter of all the objects.
    return number of objects modified.
  */

  struct ObjList *ls;
  Object *obj;
  int n=0;

  
  ls=lh->next;
  /*  printf("======\n"); */
  while(ls!=NULL){
    obj=ls->obj;

    if(proc!=players[obj->player].proc){
      if(obj->state<=0){
	if(obj->modified!=SENDOBJNEW && 
	   obj->modified!=SENDOBJNOTSEND && 
	   obj->modified!=SENDOBJUNMOD){
#if DEBUG
	  if(debugcomm1){
	    fprintf(stderr,"ERROR CheckModifiedPre(): PROC: %d obj %d (%d) player: %d type %d stype %d mod: %d time: %d ... ignoring...\n",
		 players[obj->player].proc,obj->id,obj->pid,
		   obj->player,obj->type,obj->subtype,obj->modified, GetTime());
	  }
#endif
	}
      }
      ls=ls->next;continue;
    
    }

    switch(obj->type){
      
    case PROJECTILE:
    case ASTEROID:
    case SHIP:

      if(obj->type==PROJECTILE){

	if(obj->subtype!=MISSILE){
	  if(obj->modified!=SENDOBJNEW && 
	     obj->modified!=SENDOBJNOTSEND && 
	     obj->modified!=SENDOBJDEAD){
	    SetModified(obj,SENDOBJNOTSEND);
	  }
	}
      }
      
      switch(obj->modified){
      case SENDOBJUNMOD:
	if(obj->x!=obj->x0 || obj->y!=obj->y0 || obj->ang_v!=0){
	  SetModified(obj,SENDOBJMOD);
	  n++;
	}
	if(obj->state<=0){
	  SetModified(obj,SENDOBJKILL);
	  n++;
	}
	break;
      case SENDOBJMOD0:
      case SENDOBJMOD:
      case SENDOBJALL:
      case SENDOBJAALL:
	if(obj->state<=0){
	  SetModified(obj,SENDOBJKILL);
	  n++;
	}
	break;
      case SENDOBJNEW:
	if(obj->state<=0){
	  SetModified(obj,SENDOBJDEAD);/* don't send */
	  n++;
	}
	break;
      case SENDOBJKILL:
	obj->state=0;
	break;
      case SENDOBJDEAD:
	obj->state=0;
	break;
      case SENDOBJNOTSEND:
	if(obj->state<=0){
	  SetModified(obj,SENDOBJDEAD);/* don't send */
	  n++;
	}	
	break;
      default:
	fprintf(stderr,"CheckModifiedPre() mode %d not implemented. Exiting...\n",obj->modified);
	exit(-1);
	break;
      }
      break;
    case PLANET:
      if(obj->ttl<=0)SetModified(obj,SENDOBJPLANET);
      break;
    case TRACE:
      break;
    default:
      fprintf(stdout,"Warning:CheckModifiedPre() must no reach this line\n");
      break;
    }
    ls=ls->next;
  }
  return n;
}


int CheckModifiedPost(struct HeadObjList *lh,int proc){
  /*
    version 01 27Dic2010
    Checking after send objects.
    Set modified parameter from SEND to UNMOD, DEAD, NOTSEND
    return number of objects modified.
  */

  struct ObjList *ls;
  Object *obj;
  int n=0;


  ls=lh->next;
  /*  printf("======\n"); */
  while(ls!=NULL){
    obj=ls->obj;

    if(obj->life<=0)obj->state=0;
    if(obj->modified==SENDOBJDEAD)obj->state=0;

    if(obj->state<=0){
      if(obj->modified==SENDOBJSEND || obj->modified==SENDOBJNOTSEND){
	obj->modified=SENDOBJDEAD;
      }
    }

    if(proc!=players[obj->player].proc){
      if(obj->type==PROJECTILE){ /* HERE not neccesary */
	if(obj->state<=0){
	  SetModified(obj,SENDOBJDEAD);
	}
      }
      ls=ls->next;continue;
    }
    
    switch(obj->type){
    case SHIP:
    case ASTEROID:
    case PROJECTILE:
      switch(obj->modified){
      case SENDOBJUNMOD:
      case SENDOBJMOD0:
      case SENDOBJMOD:
      case SENDOBJALL:
      case SENDOBJAALL:
      case SENDOBJNEW:
      case SENDOBJKILL:
      case SENDOBJDEAD:
	break;
      case SENDOBJSEND:
	if(obj->state>0){
	  if(obj->type==PROJECTILE){
	    if(obj->subtype!=MISSILE){
	      SetModified(obj,SENDOBJNOTSEND); /* projectiles are send only once */
	      n++;
	    }
	    else{
	    SetModified(obj,SENDOBJUNMOD);
	    n++;
	    }
	  }
	  else{
	    SetModified(obj,SENDOBJUNMOD);
	    n++;
	  }
	}
	else{
	  SetModified(obj,SENDOBJDEAD);
	  n++;
	}
	break;
      case SENDOBJNOTSEND:
	if(obj->state<=0 || obj->life<=0){
	  SetModified(obj,SENDOBJDEAD);/* remove it */
	  n++;
	}
	break;
      default:
	fprintf(stderr,"CheckModifiedPost() mode %d not implemented\n",obj->modified);
	exit(-1);
	break;
      }
      break;
    case PLANET:
      if(obj->modified==SENDOBJSEND){ 
 	SetModified(obj,SENDOBJUNMOD);
	obj->sw=0;
 	n++; 
      }
      break;
    case TRACE:
      break;
    default:
      fprintf(stdout,"Warning:CheckModifiedPost() must no reach this line\n");
      break;
    }

#if DEBUG
    if(obj->modified==SENDOBJSEND){
      if(debugcomm1){ 
	fprintf(stderr,"ERROR SetModifiedPost() %d type:%d stype:%d mod:%d\n",
		obj->id,obj->type,obj->subtype,obj->modified);
	exit(-1);
      }
    }
#endif

    ls=ls->next;
  }
  return n;
}


void Setttl0(struct HeadObjList *lh){
  /*
    version 01 (031210)
    All ships with ttl <=0 will be send. 
    If the object must be send, set ttl to 0
    if it must not be never send, set ttl to 1024

    SERVER:
      Send near enemies. Reasign its ttl.
      send allies if ttl<=0.

    CLIENT:
    send ships with ttl<=0.

    return:
    void
   */

  struct ObjList *ls;
  Object *obj=NULL;
  int proc,gmode,gcooperative,otherproc, genemyknown;
  int sw;

  proc=GetProc();
  gmode=GameParametres(GET,GMODE,0);
  gcooperative=GameParametres(GET,GCOOPERATIVE,0);
  genemyknown=GameParametres(GET,GENEMYKNOWN,0);

  ls=lh->next;
  while(ls!=NULL){
    if(proc!=players[ls->obj->player].proc){
      ls=ls->next;continue;
    }
    obj=ls->obj;
      
    if(obj->type==PROJECTILE && obj->subtype==EXPLOSION){
      obj->ttl=1024; /* explosion never are send */

      if(obj->modified!=SENDOBJNOTSEND){
	SetModified(obj,SENDOBJNOTSEND);
      }
      ls=ls->next;continue;
    }

    /* ttl code */
    switch(gmode){
    case SERVER:
    /* server   */ 
    /* send data if an ship of another proccesor is near */
      sw=0;
      switch (obj->type){
      case PROJECTILE:
      case ASTEROID:
      case SHIP:

	if(obj->ttl<=0){ /* objects ready to send */
	  sw=1;

	  if(gcooperative==TRUE && players[obj->player].control==HUMAN){
	    /* send human controled ships periodically */
	    if(obj->habitat==H_SPACE){
	      SetModified(obj,SENDOBJMOD0);
	      obj->ttl=0;
	      sw=0;
	    }
	    if(obj->ttl<=MINTTL){/* send now */
	      obj->ttl=0;
	      sw=0;
	    }
	  }
	  if(genemyknown){
	    /* if enemy is known send ships in outer space periodically */
	    if(obj->habitat==H_SPACE){
	      SetModified(obj,SENDOBJMOD0);
	      obj->ttl=0;
	      sw=0;
	    }
	    if(obj->ttl<=MINTTL){/* send now */
	      obj->ttl=0;
	      sw=0;
	    }
	  }

	  if(sw){ /* reassign  ttl */
	    otherproc=OtherProc(lh,proc,obj);   /* double buffer */
	    switch(otherproc){ /*  */
	    case 0:  /* (4r,inf) r: radar range*/
	      obj->ttl=150+obj->id%20; /* (90) don't send */
	      break;
	    case 1:  /* (3r,4r) */
	      obj->ttl=58+obj->id%12;    /* don't send */
	      break;
	    case 2:  /* (1.5r,3r) */
	      obj->ttl=16;  /* don't send */
	      break;
	    case 3:  /* (900p,1.5r) */
	      if(obj->ttl<0){
		obj->ttl=-8; /* send */
		SetModified(obj,SENDOBJMOD0);
	      }
	      break;
	    case 4:  /* (0,900p) */
	      if(obj->type==SHIP && obj->subtype==PILOT && obj->mode==LANDED){  
		/* less than zero tll means: send now and set ttl to -ttl (in Settl())*/
	       	obj->ttl=-(90+obj->id%20);
	       	SetModified(obj,SENDOBJMOD0); 
	      }
	      else{
		obj->ttl=0;
		SetModified(obj,SENDOBJMOD);
	      }
	      break;
	    case 5:
	      obj->ttl=-(90+obj->id%20);
	      SetModified(obj,SENDOBJMOD0);
	      break;
	    default:
	      obj->ttl=0;
	      SetModified(obj,SENDOBJMOD);
	      break;
	    }
	  }
	}
	break;

      case PLANET:
	break;

      default:
	break;
      }

      break;
    case CLIENT:
      /* if(obj->type==SHIP && obj->subtype==PILOT && obj->mode==LANDED){ */
      /* 	obj->ttl=-(90+obj->id%20); */
      /* } */
      break;
    default:
      /* PRODUCTION */
      fprintf(stderr,"ERROR 1 :setttl0() gmode %d unknown, mode: %d, obj type: %d player: %d id: %d pid: %d\n",
	      gmode,obj->modified,obj->type,obj->player,obj->id,obj->pid);
      exit(-1);
      break;
    }

    /* --ttl code */

    if(obj->ttl<=0){
      SetModified(obj,SENDOBJMOD0);
    }
    
    switch (obj->modified){
    case SENDOBJUNMOD:
    case SENDOBJMOD0:  /* Object modified send only position */
    case SENDOBJMOD:   /* Object modified  */
    case SENDOBJAALL:

      break;
    case SENDOBJSEND: /* just sended HERE must not happen BUG[96] ignoring*/      
      fprintf(stderr,"ERROR 2 :setttl0() mode %d unknown, obj type: %d player: %d id: %d pid: %d proc: %d\n",
	      obj->modified,obj->type,obj->player,obj->id,obj->pid,players[obj->player].proc);
      break;
      /* Send this now */
    case SENDOBJALL:
    case SENDOBJKILL:  /* send these objects */
    case SENDOBJNEW: /* new object. */
    case SENDOBJPLANET:
    case SENDOBJSTR:
      obj->ttl=0;
      break;
    case SENDOBJNOTSEND: /* don't send these objects*/
    case SENDOBJDEAD:
      obj->ttl=1024; /* don't send dead objects */
      break;
    default:
      /* PRODUCTION Quitar el exit ignorar?? */
      fprintf(stderr,"ERROR 3 :setttl0() mode %d unknown, obj type: %d player: %d id: %d pid: %d proc: %d\n",
	      obj->modified,obj->type,obj->player,obj->id,obj->pid,players[obj->player].proc);
      exit(-1);
      break;
    }
    ls=ls->next;
  }
  return;
}


void Setttl(struct HeadObjList *lh,int n){
  /*
    version 0.1
    Adjust ttl parameter used to decide what object send: 
    if ttl == 0 object is just sended (modified must be SENDOBJSEND).
    input param n:
    if n>=0 all objects ttl is set to n

    SERVER:
    ally ships: Reasign its ttl.
    send planets periodically.

    CLIENT:
    ships: Reasign its ttl.
    send planets periodically.


    return:
    void
   */

  struct ObjList *ls;
  Object *obj=NULL;
  int proc;
  int otherproc;
  int gcooperative;
  int genemyknown;

  proc=GetProc();
  gcooperative=GameParametres(GET,GCOOPERATIVE,0);
  genemyknown=GameParametres(GET,GENEMYKNOWN,0);

  if(n>=0){ /* all objects are set to ttl=n */
    ls=lh->next;
    while(ls!=NULL){
      if(proc!=players[ls->obj->player].proc){
	ls=ls->next;continue;
      }
      ls->obj->ttl=n;
      ls=ls->next;
    }
    return;
  }  /*--n<0 */


  ls=lh->next;
  while(ls!=NULL){
      
    if(proc!=players[ls->obj->player].proc){
      ls=ls->next;continue;
    }
    obj=ls->obj;

    if(obj->ttl<=0){ /* objects just sended */
      if(obj->ttl<=-4){
	obj->ttl=-obj->ttl;
	ls=ls->next;continue;
      }


      switch (obj->type){
      case PROJECTILE:
      case ASTEROID:
      case SHIP:

	/* only calc ttl for computer controled ships */
	if((gcooperative==TRUE && players[obj->player].control==HUMAN)||  genemyknown){
	
	  otherproc=OtherProc(lh,proc,obj);
	  switch(otherproc){ /*  */
	    
	  case 0:  /* (4r,inf) */
	    obj->ttl=150+obj->id%20; /* (old value: 90)*/
	    if(obj->mode==LANDED && obj->type==SHIP && obj->subtype==TOWER)obj->ttl*=2; 
	    break;
	  case 1:  /* (3r,4r) */
	    obj->ttl=56+obj->id%20; 
	    if(obj->mode==LANDED && obj->type==SHIP && obj->subtype==TOWER)obj->ttl*=2; 
	    break;
	  case 2:  /* (1.5r,3r) */
	    obj->ttl=32;
	    if(obj->mode==LANDED && obj->type==SHIP && obj->subtype==TOWER)obj->ttl*=2; 
	    break;
	  case 3:  /* (900p,1.5r) */
	    obj->ttl=16;  /* 8 */
	    if(obj->mode==LANDED && obj->type==SHIP && obj->subtype==TOWER)obj->ttl*=2; 
	    break;
	  case 4:  /* (0,900p) */
	    if(obj->type==SHIP && obj->subtype==PILOT && obj->mode==LANDED){ 
	      obj->ttl=(90+obj->id%20);
	    } 
	    else{
	      obj->ttl=2;/* 0 */
	      if(obj->mode==LANDED)obj->ttl=8;
	      /* SetModified(obj,SENDOBJMOD); */
	    }
	    break;
	  case 5:
	    obj->ttl=(150+obj->id%20); /* old value: 90*/
	    if(obj->mode==LANDED && obj->type==SHIP && obj->subtype==TOWER)obj->ttl*=2; 
	    break;
	  default:
	    break;
	  }
	}
	break;
      case PLANET:
	obj->ttl=400+(obj->id%100);
	break;
      default:
	break;
      }
    }
    ls=ls->next;
  }
  return;
}


void LoadBuffer(int order,struct Buffer *buffer,int mode){
  /*
    version 01
   */

  struct MessageHeader messh;
  int i;
  int nkp;
  int proc;
  struct IntList *list;
  int fd;

  if(buffer==NULL)return;

  buffer->n=0; /* reset buffer */
 
  proc=GetProc();

  /* main header */

  messh.id=order;
  messh.nobjs=0;
  messh.nbytes=0;
  CopyMessHeader2Buffer(buffer,&messh);

  /* rest of data */

  switch(mode){


    /******* SERVER *******************/
  case SERVER:
 
    switch(order){
    case OTSENDPING:  /* not used */
      break;
    case OTSENDOBJS:    /* send modified objects */
      CopyObjs2Buffer(buffer,listheadobjs);
      break;
    case OTSENDSAVE:    /* sendallobjects */
      
      break;
    case OTSENDLOAD:    /* load a game */

      fprintf(stdout,"LOAD\n");
      
      /* checking the file */
      if((fd=open(savefile,O_RDONLY))==-1){
	fprintf(stdout,"CommServer()[OTSENDLOAD]: Can't open the file: %s\n",savefile);
	exit(-1);
      }
      close(fd);

      break;
    case OTSENDKILL:    /* kill client */   
      
      /* OK */
      break;
    case OTSENDEND:      /* no more messages */ 
      break;
    default:
      break;
    }
    break;


    /******* CLIENT *******************/
  case CLIENT:
    switch(order){
    case OTSENDPING:  /* not used */
      break;
    case OTSENDOBJS:    /* send modified objects */
      CopyObjs2Buffer(buffer,listheadobjs);
      break;
    case OTSENDSAVE:    /* sendallobjects */
      /* objects */ 
      CopyObjs2Buffer(buffer,listheadobjs);

      /* global variables */
      messh.id=SENDGLOBAL;
      messh.nobjs=0;
      messh.nbytes=0;
      CopyMessHeader2Buffer(buffer,&messh);

      CopyGlobal2Buffer(buffer);

      /* Loading buffer with known sectors and planets */
      for(i=0;i<GameParametres(GET,GNPLAYERS,0)+2;i++){
	if(proc!=players[i].proc)continue;


	/* player */
	
	messh.id=SENDPLAYER;
	messh.nobjs=0;
	messh.nbytes=0;

	CopyMessHeader2Buffer(buffer,&messh);

	CopyPlayer2Buffer(buffer,&players[i]);

	/* list of planets */

	/* header */
	nkp=CountIntList(players[i].kplanets);
	messh.id=SENDPLANETLIST;
	messh.nobjs=nkp;
	messh.nbytes=0;
	CopyMessHeader2Buffer(buffer,&messh);

	CopyInt2Buffer(buffer,&i);

	if(nkp>0){
	  list=players[i].kplanets;

	  /* planets id */
	  while(list!=NULL){
	    CopyInt2Buffer(buffer,&(list->id));
	    list=list->next;
	  }
	}

	/* list of sectors */
	/* header */
	messh.id=SENDSECTORLIST;
	messh.nobjs=players[i].ksectors.n;
	messh.nbytes=0;

	CopyMessHeader2Buffer(buffer,&messh);

	CopyInt2Buffer(buffer,&i);

	if(players[i].ksectors.n>0){
	  list=players[i].ksectors.list;
	  while(list!=NULL){
	    CopyInt2Buffer(buffer,&(list->id));
	    list=list->next;
	  }
	}
      }
      	/* --Loading buffer with known sectors and planets */
	
      /* ending message */	

      messh.id=SENDEND;
      messh.nobjs=0;
      messh.nbytes=0;
      CopyMessHeader2Buffer(buffer,&messh);
      break;
    case OTSENDLOAD:    /* load a game */
      /* ok */
      break;
    case OTSENDKILL:    /* kill client */   

      /* OK */
      break;
    case OTSENDEND:      /* no more messages */ 
      break;
    default:
      break;
    }
    break;
  }
}


int ServerProcessBuffer(struct Buffer *buffer){
  struct MessageHeader mess;
  char *buf;
  int nbytes;
  int order;
  int i;

  int id;
  struct Player *player;
  struct PlayerAll playerall;

  int playerid;
  int nkp,nks;
  int gkplanets;
  struct IntList *kp;
  struct HeadIntIList ks;

  gkplanets=GameParametres(GET,GKPLANETS,0);

  nbytes=sizeof(struct MessageHeader);

  buf=buffer->data;
  memcpy(&mess,buf,nbytes);
  order=mess.id;
  buf+=nbytes;

  switch(order){
  case OTSENDPING:
    break;
  case OTSENDOBJS:     /* send modified objects */
    ReadObjsfromBuffer(buf);
    break;
  case OTSENDSAVE:     /* sendallobjects */
    fprintf(stdout,"OTSENDSAVE\n");
    nbytes=ReadObjsfromBuffer(buf);
    buf+=nbytes;

    do{
      nbytes=sizeof(struct MessageHeader);
      memcpy(&mess,buf,nbytes);
      order=mess.id;
      buf+=nbytes;

      switch(order){
      case SENDGLOBAL:

	nbytes=sizeof(struct Global);
	memcpy(&gclient,buf,nbytes);
#if TEST
	printf("SENDGLOBAL\n");
	printf("Global variables:\n");      
	printf("\tactual: %d\n",gclient.actual_player);
	printf("\tobj id: %d\n",gclient.g_objid);
	printf("\tproj id: %d\n",gclient.g_projid);
	printf("\tship_c: %d\n",gclient.ship_c);
	printf("\tcv: %d\n",gclient.cv);
	printf("\thabitat type: %d\n",gclient.habitat_type);
	printf("\thabitat planet: %d\n",gclient.habitat_id);
	for(i=0;i<4;i++){
	  printf("fobj[%d]=%d\n",i,gclient.fobj[i]);
	}
#endif
	buf+=nbytes;
	break;

      case SENDPLAYER:
	fprintf(stdout,"SENDPLAYER\n");
	nbytes=sizeof(struct PlayerAll);
	memcpy(&playerall,buf,nbytes); 
	buf+=nbytes;
	player=&players[playerall.id];

	/* DelIntIList(&(players[playerall.id].ksectors)); */
	kp=player->kplanets;
	memcpy(&ks,&player->ksectors,sizeof(struct HeadIntIList));


	strncpy(player->playername,playerall.playername,MAXTEXTLEN);
	player->id=playerall.id;
	player->pid=playerall.pid;
	player->proc=playerall.proc;
	player->control=playerall.control;
	player->team=playerall.team;
	player->profile=playerall.profile;
	player->strategy=playerall.strategy;
	player->maxlevel=playerall.maxlevel;
	player->color=playerall.color;
	player->cv=playerall.cv;
	player->nplanets=playerall.nplanets;
	player->nships=playerall.nships;
	player->nbuildships=playerall.nbuildships;
	player->gold=playerall.gold;
	player->balance=playerall.balance;
	player->lastaction=playerall.lastaction;
	player->ndeaths=playerall.ndeaths;
	player->nkills=playerall.nkills;
	player->points=playerall.points;
	player->modified=playerall.modified;
	player->ttl=playerall.ttl;


	players[playerall.id].kplanets=kp;
	memcpy(&players[playerall.id].ksectors,&ks,sizeof(struct HeadIntIList));

	break;

      case SENDPLANETLIST:
	nkp=mess.nobjs;

	memcpy(&playerid,buf,sizeof(int));
	buf+=sizeof(int);

	fprintf(stdout,"\trecv PLAYER: %d\n",playerid);

	DelIntList((players[playerid].kplanets));
	players[playerid].kplanets=NULL;/*kp; */

	for(i=0;i<nkp;i++){
	  memcpy(&id,buf,sizeof(int));
	  buf+=sizeof(int);
	  players[playerid].kplanets=Add2IntList((players[playerid].kplanets),id);
	}
	break;

      case SENDSECTORLIST:	
	nks=mess.nobjs;
	/* copying sectors ids to buffer*/

 	memcpy(&playerid,buf,sizeof(int)); 
 	buf+=sizeof(int); 

	players[playerid].ksectors.n=0;
	players[playerid].ksectors.n0=0;
	players[playerid].ksectors.list=NULL;
	for(i=0;i<NINDEXILIST;i++){
	  players[playerid].ksectors.index[i]=NULL;
	}

	for(i=0;i<nks;i++){
	  memcpy(&id,buf,sizeof(int));
	  buf+=sizeof(int);
	  if(gkplanets==FALSE){
	    Add2IntIList(&(players[playerid].ksectors),id);
	  }
	}
	break;
      case SENDEND:
      break;
      default:
	fprintf(stderr,"ERROR in ServerProcessBuffer() order %d unknown\n",order);
	exit(-1);

	break;
      }
      
    }while(order!=SENDEND);
    break;

  case OTSENDLOAD:     /* load game */
    break;
  case OTSENDKILL:     /* kill client */   
    return(order);
    break;
  case OTSENDEND:       /* no more messages */
    break;
  default:
    fprintf(stderr,"ERROR unknown message header id:%d\n",order);
    exit(-1);
    break;
  }
  return(0);
}


int ClientProcessBuffer(struct Buffer *buffer){
  struct MessageHeader mess;
  char *buf;
  int nbytes;
  int order;


  nbytes=sizeof(struct MessageHeader);

  buf=buffer->data;
  memcpy(&mess,buf,nbytes);
  order=mess.id;
  buf+=nbytes;

  switch(order){
  case OTSENDPING:
    break;
  case OTSENDOBJS:     /* send modified objects */
    ReadObjsfromBuffer(buf);
    break;
  case OTSENDSAVE:     /* sendallobjects */
    break;
  case OTSENDLOAD:     /* load game */
    return(0);
    break;
  case OTSENDKILL:     /* kill client */   
      return(order);
    break;
  case OTSENDEND:       /* no more messages */
    break;
  default:
    fprintf(stderr,"ERROR ClientProcessBuffer():unknown message header id:%d\n",order);
    exit(-1);
    break;
  }
  return(0);
}

int CopyMessHeader2Buffer(struct Buffer *buffer,  struct MessageHeader *messh){
  int nbytes;

  nbytes=sizeof(struct MessageHeader);
  if(buffer->n+nbytes>buffer->size){
    int newsize;
    newsize=buffer->size+1024;
    buffer->data=realloc(buffer->data,newsize*sizeof(char));
    if(buffer->data==NULL){ 
      fprintf(stderr,"ERROR in malloc Copyfile2Buf()\n");
      exit(-1);
    } 
    buffer->size=newsize;
  }

  memcpy(buffer->data+buffer->n,messh,nbytes);
  buffer->n+=nbytes;
  return(nbytes);
}

int CopyGlobal2Buffer(struct Buffer *buffer){
  /* 
     copy the global game variables to buffer buf
  */
  struct Global global;
  int nbytes;
  int i;

  global.actual_player=actual_player0;
  global.g_objid=g_objid;
  global.g_projid=g_projid;
  global.ship_c=0;
  if(ship_c!=NULL)
    global.ship_c=ship_c->id;
  global.cv=0;
  if(cv!=NULL){
    global.cv=cv->id;
  }
  global.habitat_type=habitat.type;
  global.habitat_id=0;
  /*  global.habitat_pid=0; */
  if(habitat.obj!=NULL){
    global.habitat_id=habitat.obj->id;
    /*    global.habitat_pid=habitat.obj->id; */
  }
  for(i=0;i<4;i++){
    global.fobj[i]=fobj[i];
  }

  nbytes=sizeof(struct Global);


  if(buffer->n+nbytes>buffer->size){
    int newsize;
    newsize=buffer->size+1024;
    buffer->data=realloc(buffer->data,newsize*sizeof(char));
    if(buffer->data==NULL){ 
      fprintf(stderr,"ERROR in malloc Copyfile2Buf()\n");
      exit(-1);
    } 
    buffer->size=newsize;
  }

  memcpy(buffer->data+buffer->n,&global,nbytes);
  buffer->n+=nbytes;  
  return (nbytes);  
}

int CopyPlayer2Buffer(struct Buffer *buffer,  struct Player *player){
  int nbytes;
  struct PlayerAll *playerall;

  nbytes=sizeof(struct PlayerAll);
  if(buffer->n+nbytes>buffer->size){
    int newsize;
    newsize=buffer->size+1024;
    buffer->data=realloc(buffer->data,newsize*sizeof(char));
    if(buffer->data==NULL){ 
      fprintf(stderr,"ERROR in malloc Copyfile2Buf()\n");
      exit(-1);
    } 
    buffer->size=newsize;
  }


  playerall=(struct PlayerAll *)(buffer->data+buffer->n);

  strncpy(playerall->playername,player->playername,MAXTEXTLEN);
  
  playerall->id=player->id;
  playerall->pid=player->pid;
  playerall->proc=player->proc;
  playerall->control=player->control;
  playerall->team=player->team;
  playerall->profile=player->profile;
  playerall->strategy=player->strategy;
  playerall->maxlevel=player->maxlevel;
  playerall->color=player->color;
  playerall->cv=player->cv;
  playerall->nplanets=player->nplanets;
  playerall->nships=player->nships;
  playerall->nbuildships=player->nbuildships;
  playerall->gold=player->gold;
  playerall->balance=player->balance;
  playerall->lastaction=player->lastaction;
  playerall->ndeaths=player->ndeaths;
  playerall->nkills=player->nkills;
  playerall->points=player->points;
  playerall->modified=player->modified;
  playerall->ttl=player->ttl;
	
  buffer->n+=nbytes;
  return(nbytes);
}


int CopyPlayerMod2Buffer(struct Buffer *buffer,  struct Player *player){
  int nbytes;

  struct PlayerMod pmod;

  nbytes=sizeof(struct PlayerMod);

  if(buffer->n+nbytes>buffer->size){
    int newsize;
    newsize=buffer->size+1024;
    buffer->data=realloc(buffer->data,newsize*sizeof(char));
    if(buffer->data==NULL){ 
      fprintf(stderr,"ERROR in malloc Copyfile2Buf()\n");
      exit(-1);
    } 
    buffer->size=newsize;
  }

  pmod.id=player->id;
  pmod.maxlevel=player->maxlevel; 
  pmod.nplanets=player->nplanets;
  pmod.nships=player->nships;
  pmod.nbuildships=player->nbuildships;
  pmod.gold=player->gold;
  pmod.ndeaths=player->ndeaths;
  pmod.nkills=player->nkills;
  pmod.points=player->points;

  memcpy(buffer->data+buffer->n,&pmod,nbytes);
  buffer->n+=nbytes;
  return(nbytes);
}


int CopyOrder2Buffer(struct Buffer *buffer,  struct Order *order){
  int nbytes;

  nbytes=sizeof(struct Order);
  if(buffer->n+nbytes>buffer->size){
    int newsize;
    newsize=buffer->size+1024;
    buffer->data=realloc(buffer->data,newsize*sizeof(char));
    if(buffer->data==NULL){ 
      fprintf(stderr,"ERROR in malloc Copyfile2Buf()\n");
      exit(-1);
    } 
    buffer->size=newsize;
  }

  memcpy(buffer->data+buffer->n,order,nbytes);
  buffer->n+=nbytes;
  return(nbytes);
}



int CopyInt2Buffer(struct Buffer *buffer,int *i){
  int nbytes;

  nbytes=sizeof(int);
  if(buffer->n+nbytes>buffer->size){
    int newsize;
    newsize=buffer->size+1024;
    buffer->data=realloc(buffer->data,newsize*sizeof(char));
    if(buffer->data==NULL){ 
      fprintf(stderr,"ERROR in malloc Copyfile2Buf()\n");
      exit(-1);
    } 
    buffer->size=newsize;
  }

  memcpy(buffer->data+buffer->n,i,nbytes);
  buffer->n+=nbytes;
  return(nbytes);

}


int AddObjOrders2Buffer(struct Buffer *buffer,Object *obj){
  /*
    version01 (021210)
    Add the orders of the object *obj to buffer
    returns:
    the number of bytes writed.
   */
  struct ListOrder *lo;
  int norders=0;
  int nbytes,tbytes;
  int i,n,no;


  tbytes=0;

  n=0;   /* counting n. of orders. */
  lo=obj->lorder;
  while(lo!=NULL){
    n++;
    lo=lo->next;
  }
  
  no=obj->norder;
  if(no!=n){
    fprintf(stderr,"ERROR SendPlayerOrders(): number of orders don't match norder\n" );
    fprintf(stderr,"\tnor: %d norder: %d \n",n,obj->norder);
    exit(-1);
  }
  
  nbytes=sizeof(int);  /* number of orders */
  CopyInt2Buffer(buffer,&n);
  tbytes+=nbytes;
  
  /* list of orders */
  lo=obj->lorder;
  i=0;
  while(lo!=NULL){
    nbytes=sizeof(struct Order);
    CopyOrder2Buffer(buffer,&lo->order);
    tbytes+=nbytes;
    norders++;    
    i++;

    lo=lo->next;
  }
  if(i!=n){
    fprintf(stderr,"ERROR SendPlayerOrders(): number of orders don't match norder\n" );
    fprintf(stderr,"\tnor: %d norder: %d \n",i,n);
    exit(-1);
  }
  
  return(tbytes);
}



int NetMess(struct NetMess *mess,int action){
  /*
    A list of messages to be sended between client and server.
    returns:
    1 if the operation success
    0 if no data is readed or writed.
*/

  static int cont=0;
  static struct ListNetMess head;
  static int sw=0;
  struct ListNetMess *l,*lnewm;
  int ret,n;

  if(sw==0){
    head.next=NULL;
    sw++;
  }

  ret=0;

  switch(action){
  case NMADD:
    if(mess==NULL)return(0);
    
    lnewm=malloc(sizeof(struct ListNetMess));
    if(lnewm==NULL){ 
      fprintf(stderr,"ERROR in malloc (NetMess)\n"); 
      exit(-1); 
    }
    
    lnewm->next=head.next;
    lnewm->mess.id=mess->id;
    lnewm->mess.a=mess->a;
    lnewm->mess.b=mess->b;
    head.next=lnewm;

    cont++;
    ret=1;
    break;
  case NMREAD:
    if(head.next==NULL){
      if(cont!=0){
	fprintf(stderr,"ERROR in NetMess(): no data and cont=%d\n",cont);
      }
      ret=0;
    }
    else{
      mess->id=head.next->mess.id;
      mess->a=head.next->mess.a;
      mess->b=head.next->mess.b;
      lnewm=head.next;
      head.next=head.next->next;
      free(lnewm);
      cont--;
      ret=1;
    }
    break;
  case NMCLEAN:
    fprintf(stderr,"ERROR NetMess(): action %d not implemented\n",action);
    ret=0;
    break;
  case NMCOUNT:
    ret=cont;
    break;
  case NMPRINT:
    n=0;
    l=head.next;
    while(l!=NULL){
      printf("%d %d %d\n",l->mess.id,l->mess.a,l->mess.b);
      n++;
      l=l->next;
    }
    ret=n;
    break;
  default:
    fprintf(stderr,"ERROR NetMess(): action %d not implemented\n",action);
    exit(-1);
    break;
  }

  return(ret);
}

int CopyNetMess2Buffer(struct Buffer *buffer,  struct NetMess *mess0){
  /*
    Copy the list of messages to buffer, ready to be sended between client and server.
    returns:
    the number of bytes writed.
*/

  int nbytes;
  struct NetMess mess;

  nbytes=sizeof(struct NetMess);

  if(buffer->n+nbytes>buffer->size){
    int newsize;
    newsize=buffer->size+1024;
    buffer->data=realloc(buffer->data,newsize*sizeof(char));
    if(buffer->data==NULL){ 
      fprintf(stderr,"ERROR in malloc Copyfile2Buf()\n");
      exit(-1);
    } 
    buffer->size=newsize;
  }

  mess.id=mess0->id;
  mess.a=mess0->a;
  mess.b=mess0->b;

  memcpy(buffer->data+buffer->n,&mess,nbytes);
  buffer->n+=nbytes;
  return(nbytes);
}

