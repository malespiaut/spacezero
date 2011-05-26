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

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "help.h"
#include "spacecomm.h"




void PrintArguments(struct Parametres param,char *title){

  printf("%s\n",title);

  printf("\tNUM GALAXIES: %d\n",param.ngalaxies);
  printf("\tNUM PLANETS: %d\n",param.nplanets);
  printf("\tNUM PLAYERS: %d\n",param.nplayers);
  if(param.kplanets){
    printf("\tPlanets are known by all the players.\n");
  }
  printf("\tplayer name: %s\n",param.playername);
  printf("\tknown planets: %d\n",param.kplanets);
  printf("\tsound: %d\n",param.sound);
  printf("\tmusic: %d\n",param.music);
  printf("\tcooperative mode: %d\n",param.cooperative);
  printf("\tcomputer cooperative mode: %d\n",param.compcooperative);
  printf("\tQueen mode: %d\n",param.queen);
  printf("\tpirates: %d\n",param.pirates);

  printf("\tUniverse size: %d\n",param.ul);
  printf("\tSERVER: %d\n",param.server);
  printf("\tCLIENT: %d\n",param.client);
  printf("\tIP: %s\n",param.IP);
  printf("\tPORT: %d\n",param.port);
  printf("\tfont type: %s\n",param.font);
  printf("\twindow geometry: %s\n",param.geom);
}


int Arguments(int argc,char *argv[],struct Parametres *par,char *optfile){
  /*
    funcion Arguments(). 
    Check the options file.
    Check the number of command line arguments.
  */
  

  int i;
  char arg[25]="";

  struct Validargum validarg[]={{"h",ARG_h},{"g",ARG_g},{"n",ARG_n},
				{"p",ARG_p},{"t",ARG_t},{"l",ARG_l},
				{"s",ARG_s},{"c",ARG_c},{"ip",ARG_ip},
				{"port",ARG_port},{"name",ARG_name},
				{"nosound",ARG_nosound},{"nomusic",ARG_nomusic},
				{"k",ARG_k},{"font",ARG_font},{"geom",ARG_geom},
				{"cooperative",ARG_cooperative},
				{"compcooperative",ARG_compcooperative},
				{"queen",ARG_queen},
				{"nopirates",ARG_nopirates},
				{"",ARG_0}};
  int narg=0;
  FILE *fp;
  int value;
  int fsw=0;
#if DEBUG
  int debug=0;
#endif

/* default values */
  par->ngalaxies=NUMGALAXIES;
  par->nplanets=NUMPLANETS;
  par->nplayers=-1;
  par->nteams=2;
  par->ul=ULX;
  par->kplanets=0;
  par->sound=TRUE;
  par->music=TRUE;

  par->cooperative=FALSE;
  par->compcooperative=FALSE;
  par->queen=FALSE;

  par->pirates=TRUE;

  par->server=FALSE;
  par->client=FALSE;
  par->IP=DEFAULT_IP;
  par->port=DEFAULT_PORT;
  par->port2=DEFAULT_PORT+1;
  strcpy(par->playername,"");
  strcpy(par->font,"");
  strcpy(par->geom,"");


  /* reading options file values */
#if DEBUG
  if(debug)printf("reading options file\n");
#endif
  if((fp=fopen(optfile,"rt"))==NULL){

    if((fp=fopen(optfile,"wt"))==NULL){
      fprintf(stdout,"No puede abrirse el archivo: %s\n", optfile);
      exit(-1);
    }
    /* default options */   /* check also SetDefaultOptions() in graphics.c */
    fprintf(fp,"%d %d %d %d %d %d %d %d %d\n",
	    FALSE,FALSE,FALSE,NUMPLANETS,NUMPLAYERS,ULX,FALSE,FALSE,FALSE);
    fclose(fp);

    if((fp=fopen(optfile,"rt"))==NULL){
      fprintf(stdout,"No puede abrirse el archivo: %s", optfile);
      exit(-1);
    }
  }

  if(fscanf(fp,"%d",&value)!=1){ /* universe known */
    perror("fscanf");
    fsw=1;
  }
  if(value==0 || value==1){
    if(value==1)par->kplanets=1;
#if DEBUG
    if(debug)printf("\tknown planets:%d\n",value); 
#endif
  }
  else{
    fsw=2;
#if DEBUG
    if(debug)fprintf(stderr,"\tuk: ERROR\n"); 
#endif
  }


  if(fscanf(fp,"%d",&value)!=1){ /* music off */
    perror("fscanf");
    fsw=1;
  }
  if(value==0 || value==1){
    if(value==1)par->music=0;
#if DEBUG
    if(debug)printf("\tmusic OFF:%d\n",value);
#endif
  }
  else{
    fsw=3;
    par->music=0;
#if DEBUG
    if(debug)fprintf(stderr,"\tmusic OFF:ERROR\n");
#endif
  }

  if(fscanf(fp,"%d",&value)!=1){ /* sound off */
    perror("fscanf");
    fsw=1;
  }
  if(value==0 || value==1){
    if(value==1){
      par->sound=0;
      par->music=0;
    }
#if DEBUG
    if(debug)printf("\tsound OFF:%d\n",value); 
#endif
  }
  else{
    fsw=4;
    par->sound=0;
    par->music=0;
#if DEBUG
    if(debug)fprintf(stderr,"\tsound OFF:ERROR\n"); 
#endif
  }
  
  if(fscanf(fp,"%d",&value)!=1){ /* number of planets */
    perror("fscanf");
    fsw=1;
  }
  if(value<MINNUMPLANETS || value>MAXNUMPLANETS){
    fsw=5;
#if DEBUG
    if(debug)fprintf(stderr,"\tnplanets:ERROR\n");
#endif
  }
  else{
#if DEBUG
    if(debug)printf("\tnplanets:%d\n",value);
#endif
    par->nplanets=value;
  }


  if(fscanf(fp,"%d",&value)!=1){ /* number of players */
    perror("fscanf");
    fsw=1;
  }
  if(value<MINNUMPLAYERS || value>MAXNUMPLAYERS){
    fsw=6;
#if DEBUG
    if(debug)fprintf(stderr,"\tnplayers:ERROR\n");
#endif
  }
  else{
#if DEBUG
    if(debug)printf("\tnplayers:%d\n",value);
#endif
    par->nplayers=value;
  }
  
  
  if(fscanf(fp,"%d",&value)!=1){ /* Universe size */
    perror("fscanf");
    fsw=1;
  }
  if(value<MINULX || value>MAXULX){
    fsw=7;
#if DEBUG
    if(debug)fprintf(stderr,"\tuniverse size:ERROR\n"); 
#endif
  }
  else{
#if DEBUG
    if(debug)printf("\tuniverse size:%d\n",value); 
#endif
    par->ul=value;
  }
  if(par->nplayers>par->nplanets)fsw=8;


  if(fscanf(fp,"%d",&value)!=1){ /* cooperative mode on off */
    perror("fscanf");
    fsw=1;
  }
  if(value==0 || value==1){
#if DEBUG
    if(debug)printf("\tcooperative:%d\n",value);
#endif
    if(value==1){
      par->cooperative=1;
    }
  }
  else{
    fsw=9;
#if DEBUG
    if(debug)fprintf(stderr,"\tcooperative:ERROR\n");
#endif
  }


  if(fscanf(fp,"%d",&value)!=1){ /* computer cooperative mode on off */
    perror("fscanf");
    fsw=1;
  }
  if(value==0 || value==1){
#if DEBUG
    if(debug)printf("\tcomputer cooperative:%d\n",value); 
#endif
    if(value==1){
      par->compcooperative=1;
    }
  }
  else{
    fsw=10;
#if DEBUG
    if(debug)fprintf(stderr,"\tcomputer cooperative:ERROR\n"); 
#endif
  }

  if(fscanf(fp,"%d",&value)!=1){ /* Queen mode on off */
    perror("fscanf");
    fsw=1;
  }
  if(value==0 || value==1){
#if DEBUG
    if(debug)printf("\tQueen mode:%d\n",value);
#endif
    if(value==1){
      par->queen=1;
    }
    
  }
  else{
#if DEBUG
    if(debug)fprintf(stderr,"\tQueen mode:ERROR\n");
#endif
    fsw=11;
  }


  fclose(fp);

  if(fsw){
    fprintf(stderr,"(%d)Warning: Incorrect values in options file %s\n",fsw,optfile);
    fprintf(stderr,"\t Ignoring file.\n");
    fprintf(stderr,"\t Setting default values.\n");

    par->ngalaxies=NUMGALAXIES;
    par->nplanets=NUMPLANETS;
    par->nplayers=NUMPLAYERS;
    par->ul=ULX;
    par->kplanets=FALSE;
    par->sound=TRUE;
    par->music=TRUE;
    par->cooperative=FALSE;
    par->compcooperative=FALSE;
    par->queen=FALSE;
    par->pirates=TRUE;

    if(fsw==3){
      par->music=FALSE;
    }
    if(fsw==4){
      par->sound=FALSE;
      par->music=FALSE;
    }
  }

  /* reading options file values */


  /* command line values */
  for(i=0;i<argc;i++){
    if(*argv[i]=='-'){
      strncpy(arg,&argv[i][1],24);
      narg=SearchArg(arg,&validarg[0]);
      if(narg<0){ 
 	printf("\ninvalid option -%s\n",arg); 
 	printf("\ntry 'spacezero -h' for help\n"); 
 	printf("\nExiting ...\n"); 
 	exit(-1); 
      } 

      switch(narg){
      case ARG_h:/*'h': show help */
	return(ARG_h);
	break;
      case ARG_g:/*'g': number of galaxies */
	if(i+1<argc){
	  par->ngalaxies=atoi(argv[i+1]);
	  i++;
	}
	else{
	  return(ARG_g);
	}
	break;
      case ARG_n:/*'n': number of planets */
	if(i+1<argc){
	  par->nplanets=atoi(argv[i+1]);
	  i++;
	}
	else{
	  return(ARG_n);
	}
	break;
      case ARG_p:/*'p': number of players */
	/*	printf("mierda\n"); */
	if(i+1<argc){
	  par->nplayers=atoi(argv[i+1]);
	  i++;
	}
	else{
	  return(ARG_p);
	}
	break;
      case ARG_t:/* 't': team, not used */
	break;
      case ARG_l:/*'l': size of universe */
	if(i+1<argc){
	  par->ul=atoi(argv[i+1]);
	  i++;
	}
	else{
	  return(ARG_l);
	}
	break;
      case ARG_s:/*'s': server */
	par->server=TRUE;
	par->client=FALSE;
	break;
      case ARG_c:/*'c': client */
	par->server=FALSE;
	par->client=TRUE;
	break;
      case ARG_ip:/*'ip': ip of the server */
	if(i+1<argc){
	  par->IP=(argv[i+1]);
	  i++;
	}
	else{
	  return(ARG_ip);
	}
	break;
      case ARG_port:/*'port': port used for communication */
	if(i+1<argc){
	  par->port=atoi(argv[i+1]);
	  par->port2=atoi(argv[i+1])+1;
	  i++;
	}
	else{
	  return(ARG_port);
	}
	break;
      case ARG_name: /* name:  player name */
 	if(i+1<argc){
	  strncpy(par->playername,argv[i+1],32);
	  i++;
	}
	else{
	  return(ARG_name);
	}
	break;
      case ARG_nosound: /* nosound */
	par->sound=0;
	par->music=0;
	break;

      case ARG_nomusic: /* nomusic */
	par->music=0;
	break;
      case ARG_k: /*k: known planets */
	par->kplanets=1;
	break;
      case ARG_font: /* font type  */
 	if(i+1<argc){
	  strncpy(par->font,argv[i+1],128);
	  i++;
	}
	else{
	  return(ARG_font);
	}
	break;
      case ARG_geom: /* window geometry */
 	if(i+1<argc){
	  strncpy(par->geom,argv[i+1],32);
	  i++;
	}
	else{
	  return(ARG_geom);
	}
	break;

      case ARG_cooperative: /* cooperative mode */
	par->cooperative=1;
	break;
      case ARG_compcooperative: /* computer cooperative mode */
	par->compcooperative=1;
	break;
      case ARG_queen: /* queen mode on */
	par->queen=1;
	break;
      case ARG_nopirates:
	par->pirates=FALSE;
	break;

      default:
	printf("\ninvalid option -%s\n",arg);
	printf("\ntry 'spacezero -h' for help\n");
	printf("\nExiting ...\n");
	exit(-1);
	break;
      }
    }
  }
  return(0);
}  /* --Arguments()  */

int BuscarCad(char *target ,char *cad[],int n){
  int i;
  int status;
  status=-1;
  for(i=0;i<n;i++){
    if(strncmp(target,cad[i],24)==0){
      return(i);
    }
  }
  return(-1);
}


void Usage(char *ver,char *l_rev){
     
  /*
   *      usage() Little help. Show the command line parameters.
   */
  
    
  (void) fprintf( stdout, 
		  "Usage is: spacezero [GENERAL OPTIONS] [GAME OPTIONS]\n" );
  (void) fprintf( stdout, 
		  "          spacezero [GENERAL OPTIONS] -s [MULTIPLAYER OPTIONS] [GAME OPTIONS]\n" );
  (void) fprintf( stdout, 
		  "          spacezero [GENERAL OPTIONS] -c [MULTIPLAYER OPTIONS]\n" );
  (void) fprintf( stdout, 
		  "          spacezero -h\n" );
  (void) fprintf( stdout, 
		  "\n-s\t\t server mode (multiplayer).\n" );
  (void) fprintf( stdout, 
		  "-c\t\t client mode (multiplayer).\n" );
  (void) fprintf( stdout, 
		  "-h\t\t this help.\n" );
  (void) fprintf( stdout, 
		  "\nGENERAL OPTIONS:\n" );
  (void) fprintf( stdout, 
		  "-nosound\t sound disabled.\n" );
  (void) fprintf( stdout, 
		  "-nomusic\t music disabled.\n" );
  (void) fprintf( stdout, 
		  "-name playername where playername is the name of the player.\n" );
  (void) fprintf( stdout, 
		  "-f font\t\t change the default font by font.\n" );
  (void) fprintf( stdout, 
		  "\nGAME OPTIONS:\n" );
  (void) fprintf( stdout, 
		  "-geom WIDTHxHEIGHT define the size of the main window.\n" );
  (void) fprintf( stdout, 
		  "-n n\t\t where n is the number of planets.(default %d)\n",NUMPLANETS);
  (void) fprintf( stdout, 
		  "-p n\t\t where n is the number of players.(default %d)\n",NUMPLAYERS);
  (void) fprintf( stdout, 
		  "-g n\t\t where n is the number of galaxies.(default %d)\n",NUMGALAXIES);
  (void) fprintf( stdout, 
		  "-l n\t\t where n is the size of the Universe.\n" );
  (void) fprintf( stdout, 
		  "-k\t\t planets are known by all the players.\n" );
  (void) fprintf( stdout, 
		  "-cooperative\t cooperative mode (all humans allied).\n" );
  (void) fprintf( stdout, 
		  "-compcooperative computer cooperative mode (all computers allied).\n" );
  (void) fprintf( stdout, 
		  "-queen\t\t Queen mode.\n" );
  (void) fprintf( stdout, 
		  "-nopirates\t Dont add a team of pirates.\n" );
  (void) fprintf( stdout, 
		  "\nMULTIPLAYER OPTIONS:\n" );
  (void) fprintf( stdout, 
		  "-ip IP\t\t where IP is the ip of the server.(default %s)\n",DEFAULT_IP);
  (void) fprintf( stdout, 
		  "-port n\t\t where n and n+1 are the tcp ports used.(default %d)\n",DEFAULT_PORT);
  (void) fprintf( stdout, 
		  "\nKeyboard controls:\n==================\n");
  (void) fprintf( stdout, 
		  "up arrow\t accelerate.\n");
  (void) fprintf( stdout, 
		  "left,right arrows turn left right.\n");
  (void) fprintf( stdout, 
		  "space\t\t fire.\n");
  (void) fprintf( stdout, 
		  "tab\t\t change to next ship.\n");
  (void) fprintf( stdout, 
		  "Ctrl-tab\t change to previous ship.\n");
  (void) fprintf( stdout, 
		  "PageUp\t\t change to next planet.\n");
  (void) fprintf( stdout, 
		  "PageDown\t change to previous planet.\n");
  (void) fprintf( stdout, 
		  "Home\t\t change to first ship in outer space.\n");
  (void) fprintf( stdout, 
		  "1 2 3\t\t choose weapon.\n");
  (void) fprintf( stdout, 
		  "Ctrl-[f1f2f3f4]\t mark a ship to be selected.\n");
  (void) fprintf( stdout, 
		  "f1 f2 f3 f4\t select a previous marked ship.\n");
  (void) fprintf( stdout, 
		  "f5\t\t show a ship list.\n");
  (void) fprintf( stdout, 
		  "f6\t\t show game statistics.\n");
  (void) fprintf( stdout, 
		  "o\t\t enter in order menu.\n");
  (void) fprintf( stdout, 
		  "Esc\t\t exit from order menu. Exit from manual mode.\n");
  (void) fprintf( stdout, 
		  "m\t\t show space map.\n");
  (void) fprintf( stdout, 
		  "a i\t\t automatic-manual mode.\n");
  (void) fprintf( stdout, 
		  "n\t\t window, ship mode view.\n");
  (void) fprintf( stdout, 
		  "p\t\t pause game\n");
  (void) fprintf( stdout, 
		  "Ctrl-s\t\t save the game.\n");
  (void) fprintf( stdout, 
		  "Ctrl-l\t\t load the saved game.\n");
  (void) fprintf( stdout, 
		  "Ctrl-q\t\t quit game.\n");
  (void) fprintf( stdout, 
		  "\nIn map view:\n------------\n");
  (void) fprintf( stdout, 
		  "z Z\t\t zoom in out.\n");
  (void) fprintf( stdout, 
		  "arrow keys\t move map.\n");
  (void) fprintf( stdout, 
		  "space\t\t center map in the actual ship.\n");
  (void) fprintf( stdout, 
		  "mouse pointer\t show coordinates.\n");
  (void) fprintf( stdout, 
		  "l\t\t show-hide labels.\n");

  (void) fprintf( stdout,
		  "\nversion: %s",ver);
  (void) fprintf( stdout, 
		  "\nlast revision: %s\n",l_rev);
  (void) fprintf( stdout, "Please, send bugs and suggestions to: mrevenga at users.sourceforge.net\n");

}       /* -- funtion usage */



int CheckArgs(struct Parametres p){
  /* 
     returns:
     1 if there are a non valid number
     0 if the the values are in range
   */

  if(p.nplanets<MINNUMPLANETS){
    printf("Number of planets must be > %d\n",MINNUMPLANETS);
    return(1);
  }
  if(p.nplanets>MAXNUMPLANETS){
    printf("Number of planets must be < %d\n",MAXNUMPLANETS);
    return(1);
  }
  if(p.nplayers<MINNUMPLAYERS){
    printf("Number of players must be > %d\n",MINNUMPLAYERS);
    return(1);
  }
  if(p.nplanets<p.nplayers){
    printf("Number of planets must be > number of players\n");
    return(1);
  }

  if(p.nplayers>MAXNUMPLAYERS){
    printf("number of players must be less than %d\n",MAXNUMPLAYERS);
    return(1);
  }
  if(p.ul<MINULX){
    printf("Size of Universe must be > %d\n",MINULX);
    return(1);
  }
  if(p.ul>MAXULX){
    printf("Size of Universe must be < %d\n",MAXULX);
    return(1);
  }

  return(0);
}


int SearchArg(char *target,struct Validargum *v){

  int i=0;

  while(v[i].id>0){
    if(strncmp(target,v[i].cad,24)==0){
      return(v[i].id);
    }
    i++;
  }
  return(-1);
}

int GetGeom(char *geom,int *w,int *h){
  /*
    Gets the window geometry parametres from the cad geom.
    returns:
    In the pointers w and h the value of the window geometry from the cad geom.
    0 if there are no error
    -1,-2 if there are some error in the structure of the cad geom.
   */

  char str[24],str0[24];
  int sw;
  int len;
  char *pointer;
  char *endptr=NULL;

  if(strlen(geom)==0){
    *w=DEFAULTWIDTH;
    *h=DEFAULTHEIGHT;
    return(0);
  }
  sw=0;
  strncpy(str0,geom,24);
  strncpy(&str0[23],"\0",(size_t)1);
  pointer=strchr(str0,'x');

  if(pointer==NULL)
    pointer=strchr(str0,':');
 
  if(pointer){
    len=strlen(str0)-strlen(pointer);
    strncpy(str,geom,len);
    *w=(int)strtol(str,&endptr,10);
    if(*w==0 && str==endptr){
      sw++;
    }
    *h=strtol(pointer+1,&endptr,10);
    if(*h==0 && pointer+1==endptr){
      sw++;
    }
  }
  else{
    return(-1);
  }
  if(sw)return(-2);
  return(0);  
}


void PrintWarnings(char *version){
  fprintf(stderr,"**************************************************************\n");
  fprintf(stderr,"WARNING: This is the development version of SpaceZero (version %s)\nIt can contain bugs.\nNet and saved games can be incompatible with the official release.\nMaybe SpaceZero doesnt work properly.\n", version);

  if(sizeof(int)!=4){
    fprintf(stderr,"\nWARNING: size of int %d bytes\n, maybe SpaceZero doesnt work properly\n",sizeof(int));//HERE warning: format '%d' expects type 'int', but argument 3 has type 'long unsigned int'
  }
  fprintf(stderr,"\nWARNING: Communication between 64 and 32 bits machines not so tested,\nmaybe SpaceZero doesnt work properly.\n");


  fprintf(stderr,"**************************************************************\n");
}
