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
  printf("\tsound vol: %d\n",param.soundvol);
  printf("\tmusic vol: %d\n",param.musicvol);
  printf("\tcooperative mode: %d\n",param.cooperative);
  printf("\tcomputer cooperative mode: %d\n",param.compcooperative);
  printf("\tQueen mode: %d\n",param.queen);
  printf("\tpirates: %d\n",param.pirates);
  printf("\tenemy known: %d\n",param.enemyknown);

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
    version 01
    funcion Arguments(). 
    Check the options file.
    Check the number of command line arguments.
  */
  

  int i;
  char arg[32]="";

  struct Validargum validarg[]={{"h",ARG_h},{"g",ARG_g},{"n",ARG_n},
				{"p",ARG_p},{"t",ARG_t},{"l",ARG_l},
				{"s",ARG_s},{"c",ARG_c},{"ip",ARG_ip},
				{"port",ARG_port},{"name",ARG_name},
				{"nosound",ARG_sound},{"nomusic",ARG_music},
				{"soundvol",ARG_soundvol},{"musicvol",ARG_musicvol},
				{"k",ARG_k},{"font",ARG_font},{"geom",ARG_geom},
				{"cooperative",ARG_cooperative},
				{"compcooperative",ARG_compcooperative},
				{"queen",ARG_queen},
				{"pirates",ARG_pirates},
				{"nopirates",ARG_nopirates},
				{"enemyknown",ARG_enemyknown},
				{"noenemyknown",ARG_noenemyknown},
				{"nomenu",ARG_nomenu},
				{"fontlist",ARG_fontlist},
				{"",ARG_0}};
  int narg=0;
  FILE *fp;
  int fsw=0;
#if DEBUG
  int debug=0;
#endif

  /* default values */
  SetDefaultParamValues(par);
  
  /*******  options file values ******/
#if DEBUG
  if(debug)printf("reading options file\n");
#endif
  if((fp=fopen(optfile,"rt"))==NULL){

    if((fp=fopen(optfile,"wt"))==NULL){
      fprintf(stdout,"I cant create the file: %s\n", optfile);
      exit(-1);
    }
    /* file doesn't exists */
    /* default options */   /* check also SetDefaultOptions() in graphics.c */
    fclose(fp);
    SaveParamOptions(optfile,par);

    if((fp=fopen(optfile,"rt"))==NULL){
      fprintf(stdout,"I cant open the file: %s", optfile);
      exit(-1);
    }
    fclose(fp);
  }

  if(LoadParamOptions(optfile,par)){
    /* some error in options file. Setting default options */
    fprintf(stderr,
	    "ERROR in options file: incorrect version. Overwriting with default options.\n");
    SaveParamOptions(optfile,par);
    if(LoadParamOptions(optfile,par)){
      fprintf(stderr,
	      "ERROR in options file: unknown error. Exiting...\n");   
      exit(-1);
    }
  }

  /****** checking options *******/

  /* universe known */
  if(par->kplanets!=0 && par->kplanets!=1){
    fsw=2;
#if DEBUG
    if(debug)fprintf(stderr,"\tuk: ERROR\n"); 
#endif
    
  }

  /* sound */
  
  if(par->music<0 && par->music>1){
    fsw=3;
    par->music=0;
#if DEBUG
    if(debug)fprintf(stderr,"\tmusic OFF:ERROR\n");
#endif
  }

  if(par->sound<0 && par->sound>1){
    fsw=4;
    par->sound=0;
    par->music=0;
#if DEBUG
    if(debug)fprintf(stderr,"\tsound OFF:ERROR\n"); 
#endif
  }

  if(par->musicvol<0 || par->musicvol>100){
    fsw=3;
    par->musicvol=100;
#if DEBUG
    if(debug)fprintf(stderr,"\tmusic OFF:ERROR\n");
#endif
  }

  if(par->soundvol<0 || par->soundvol>100){
    fsw=4;
    par->soundvol=100;
    par->musicvol=100;
#if DEBUG
    if(debug)fprintf(stderr,"\tsound OFF:ERROR\n"); 
#endif
  }


  if(par->nplanets<MINNUMPLANETS || par->nplanets>MAXNUMPLANETS){
    fsw=5;
#if DEBUG
    if(debug)fprintf(stderr,"\tnplanets:ERROR\n");
#endif
  }

  if(par->nplayers<MINNUMPLAYERS || par->nplayers>MAXNUMPLAYERS){
    fsw=6;
#if DEBUG
    if(debug)fprintf(stderr,"\tnplayers:ERROR\n");
#endif
  }
  
  if(par->ul<MINULX || par->ul>MAXULX){
    fsw=7;
#if DEBUG
    if(debug)fprintf(stderr,"\tuniverse size:ERROR\n"); 
#endif
  }

  if(par->nplayers>par->nplanets)fsw=8;

  if(par->cooperative!=0 && par->cooperative!=1){
    fsw=9;
#if DEBUG
    if(debug)fprintf(stderr,"\tcooperative:ERROR\n");
#endif
  }

  if(par->compcooperative!=0 && par->compcooperative!=1){
    fsw=10;
#if DEBUG
    if(debug)fprintf(stderr,"\tcomputer cooperative:ERROR\n"); 
#endif
  }

  if(par->queen!=0 && par->queen!=1){
#if DEBUG
    if(debug)fprintf(stderr,"\tQueen mode:ERROR\n");
#endif
    fsw=11;
  }


  if(fsw){
    fprintf(stderr,"(%d)Warning: Incorrect values in options file %s\n",fsw,optfile);
    fprintf(stderr,"\t Ignoring file.\n");
    fprintf(stderr,"\t Setting default values.\n");

    if(fsw==3){
      par->music=0;
      par->musicvol=100;
    }
    if(fsw==4){
      par->sound=0;
      par->music=0;
      par->soundvol=100;
      par->musicvol=100;
    }
  }
  /*******  --options file values ******/


  /*************** command line values ******************/
  for(i=0;i<argc;i++){
    if(*argv[i]=='-'){
      strncpy(arg,&argv[i][1],25);strncpy(&arg[24],"\0",1);
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
	  //	  par->IP=(argv[i+1]);
	  strncpy(par->IP,argv[i+1],MAXTEXTLEN);strncpy(&par->IP[MAXTEXTLEN-1],"\0",1);
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
	  strncpy(&par->playername[31],"\0",1);
	  i++;
	}
	else{
	  return(ARG_name);
	}
	break;
      case ARG_sound: /* nosound */
	par->sound=0;
	par->music=0;
	break;

      case ARG_music: /* nomusic */
	par->music=0;
	break;

      case ARG_soundvol: /* sound volume */
	if(i+1<argc){
	  par->soundvol=atoi(argv[i+1]);
	  i++;
	}
	else{
	  return(ARG_soundvol);
	}
	break;

      case ARG_musicvol: /* music volume */
	if(i+1<argc){
	  par->musicvol=atoi(argv[i+1]);
	  i++;
	}
	else{
	  return(ARG_musicvol);
	}
	break;

      case ARG_k: /*k: known planets */
	par->kplanets=1;
	break;
      case ARG_font: /* font type  */
 	if(i+1<argc){
	  strncpy(par->font,argv[i+1],128);
	  strncpy(&par->font[127],"\0",1);
	  i++;
	}
	else{
	  return(ARG_font);
	}
	break;
      case ARG_geom: /* window geometry */
 	if(i+1<argc){
	  strncpy(par->geom,argv[i+1],32);
	  strncpy(&par->geom[31],"\0",1);
	  //HERE check param values.
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
      case ARG_pirates:
	par->pirates=TRUE;
	break;
      case ARG_nopirates:
	par->pirates=FALSE;
	break;
      case ARG_enemyknown:
	par->enemyknown=TRUE;
	break;
      case ARG_noenemyknown:
	par->enemyknown=FALSE;
	break;
      case ARG_nomenu:
	par->menu=FALSE;
	break;
      case ARG_fontlist:
	par->fontlist=TRUE;
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
  if(par->nplayers==-1){
    if(par->server==FALSE && par->client==FALSE){
      par->nplayers=NUMPLAYERS;
    }
    if(par->server==TRUE){
      par->nplayers=NUMPLAYERS;
    }
    if(par->client==TRUE){
      par->nplayers=1;
    }
  }

  return(0);
}  /* --Arguments()  */


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
		  "-soundvol n\t where n is the sound volume. (0..100)\n" );
  (void) fprintf( stdout, 
		  "-musicvol n\t where n is the music volume. (0..100)\n" );
  (void) fprintf( stdout, 
		  "-nosound\t sound disabled.\n" );
  (void) fprintf( stdout, 
		  "-nomusic\t music disabled.\n" );
  (void) fprintf( stdout, 
		  "-name playername where playername is the name of the player.\n" );
  (void) fprintf( stdout, 
		  "-f font\t\t change the default font by font.\n" );
  (void) fprintf( stdout, 
		  "-nomenu \t Starts automatically, without menu.\n" );
  (void) fprintf( stdout, 
		  "\nGAME OPTIONS:\n" );
  (void) fprintf( stdout, 
		  "-geom WIDTHxHEIGHT define the size of the main window.\n" );
  (void) fprintf( stdout, 
		  "-n n\t\t where n is the number of planets.(%d..%d, default %d)\n",MINNUMPLANETS,MAXNUMPLANETS,NUMPLANETS);
  (void) fprintf( stdout, 
		  "-p n\t\t where n is the number of players.(%d..%d, default %d)\n",MINNUMPLAYERS,MAXNUMPLAYERS,NUMPLAYERS);
  (void) fprintf( stdout, 
		  "-g n\t\t where n is the number of galaxies.(%d..%d, default %d)\n",MINNUMGALAXIES,MAXNUMGALAXIES,NUMGALAXIES);
  (void) fprintf( stdout, 
		  "-l n\t\t where n is the size of the Universe.(%d..%d, default %d)\n",MINULX,MAXULX,ULX);
  (void) fprintf( stdout, 
		  "-k\t\t planets are known by all the players.\n" );
  (void) fprintf( stdout, 
		  "-cooperative\t cooperative mode (all humans allied).\n" );
  (void) fprintf( stdout, 
		  "-compcooperative computer cooperative mode (all computers allied).\n" );
  (void) fprintf( stdout, 
		  "-queen\t\t Queen mode.\n" );
  (void) fprintf( stdout, 
		  "-pirates\t Add a team of pirates (default).\n" );
  (void) fprintf( stdout, 
		  "-nopirates\t Don't add a team of pirates.\n" );
  (void) fprintf( stdout, 
		  "-enemyknown\t All anemies are known.\n" );
  (void) fprintf( stdout, 
		  "-noenemyknown\t All anemies are unknown (default).\n" );
  (void) fprintf( stdout, 
		  "\nMULTIPLAYER OPTIONS:\n" );
  (void) fprintf( stdout, 
		  "-ip IP\t\t where IP is the ip of the server.(default %s)\n",DEFAULT_IP);
  (void) fprintf( stdout, 
		  "-port n\t\t where n and n+1 are the tcp ports used.(default %d)\n",DEFAULT_PORT);
  (void) fprintf( stdout, 
		  "\nKeyboard controls:\n==================\n");
  (void) fprintf( stdout, 
		  "up arrow\t accelerate/manual mode.\n");
  (void) fprintf( stdout, 
		  "left/right arrows turn left/right.\n");
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
		  "f7\t\t show game messages log.\n");
  (void) fprintf( stdout, 
		  "o\t\t enter in order menu.\n");
  (void) fprintf( stdout, 
		  "Esc\t\t cancel actual order.\n\t\t close info windows.\n");
  (void) fprintf( stdout, 
		  "m\t\t show space map.\n");
  (void) fprintf( stdout, 
		  "up/down arrows\t manual/automatic mode.\n");

  (void) fprintf( stdout, 
		  "Ctrl +/-\t volume up/down.\n");

  (void) fprintf( stdout, 
		  "Ctrl-n\t\t window, ship mode view.\n");
  (void) fprintf( stdout, 
		  "Ctrl-p\t\t pause game\n");
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
		  "space\t\t center map on current ship.\n");
  (void) fprintf( stdout, 
		  "mouse pointer\t show coordinates.\n");
  (void) fprintf( stdout, 
		  "l\t\t show-hide labels.\n");
  (void) fprintf( stdout, 
		  "left mouse \t Select the nearest ship.\nbutton\n");
  (void) fprintf( stdout, 
		  "right mouse \t Send the selected ships to that point.\nbutton\n");
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
  if(p.ngalaxies<MINNUMGALAXIES){
    printf("Number of galaxies must be >= %d\n",MINNUMGALAXIES);
    return(1);
  }
  if(p.ngalaxies>MAXNUMGALAXIES){
    printf("Number of galaxies must be <= %d\n",MAXNUMGALAXIES);
    return(1);
  }
  if(p.nplayers<MINNUMPLAYERS){
    printf("Number of players must be > %d\n",MINNUMPLAYERS);
    return(1);
  }
  if(p.nplanets<p.nplayers){
    printf("Number of planets must be >= number of players\n");
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

  if(p.port<49152 || p.port>65535){
    printf("Invalid port: %d. Must be between (49152,65535).\n",p.port);
    return(1);
  }

  if(p.soundvol<0){
    printf("sound volume must be >= 0\n");
    return(1);
  }
  if(p.soundvol>100){
    printf("sound volume must be <= 100\n");
    return(1);
  }

  if(p.musicvol<0){
    printf("music volume must be >= 0\n");
    return(1);
  }
  if(p.musicvol>100){
    printf("music volume must be <= 100\n");
    return(1);
  }


  /* TODO */
  /***** ip *****/ 

  /***** geom *****/ 

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
    >0 if therea are some error in the structure of the cad geom.
   */

  char str[24];
  int sw;
  int len;
  char *pointer;
  char *endptr=NULL;

  //  printf("GetGeom(): %s\n",geom);
  *w=DEFAULTWIDTH;
  *h=DEFAULTHEIGHT;
  len=strlen(geom);

  if(len==0){
    /* no option given */
    return(1);
  }

  if(len>9){
    fprintf(stderr,"WARNING: invalid option geom. Too high. Settting geometry to default: %dx%d.\n",DEFAULTWIDTH,DEFAULTHEIGHT);
    return(2);
  }
  sw=0;
  pointer=strchr(geom,'x');

  if(pointer==NULL){
    pointer=strchr(geom,':');
  }

  if(pointer){
    len=strlen(geom)-strlen(pointer);
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
    sw++;
  }

  if(sw){
    fprintf(stderr,"WARNING: -geom option bad formed. Using default values.\n");
    *w=DEFAULTWIDTH;
    *h=DEFAULTHEIGHT;
  }

  if(*w<640 || *w>1680 || *h<312 || *h>1050){
    if(*w<640)*w=640;
    if(*w>1680)*w=1680;
    if(*h<312)*h=312;
    if(*h>1050)*h=1050;
    fprintf(stderr,"WARNING: geom values reach their limit. Setting geometry to limit values:(640,1680)x(312,1050).\n");
      return(1);
  }
  return(0);  
}


void PrintWarnings(char *version){
  fprintf(stderr,"**************************************************************\n");
  fprintf(stderr,"WARNING: This is the development version of SpaceZero (version %s)\nIt can contain bugs.\nNet and saved games can be incompatible with the official release.\nMaybe SpaceZero doesn't work properly.\n", version);

  if(sizeof(int)!=4){
    fprintf(stderr,"\nWARNING: size of int %d bytes\n, maybe SpaceZero doesn't work properly\n",(int)sizeof(int));//HERE warning: format '%d' expects type 'int', but argument 3 has type 'long unsigned int'
  }
  fprintf(stderr,"\nWARNING: Communication between 64 and 32 bits machines not so tested,\nmaybe SpaceZero doesn't work properly.\n");


  fprintf(stderr,"**************************************************************\n");
}

void SetDefaultParamValues(struct Parametres *par){
  printf("Setting default param values\n");  
  par->ngalaxies=NUMGALAXIES;
  par->nplanets=NUMPLANETS;
  par->nplayers=2;
  par->nteams=2;
  par->ul=ULX;
  par->kplanets=0;
  par->sound=1;
  par->music=1;
  par->soundvol=100;
  par->musicvol=100;

  par->cooperative=FALSE;
  par->compcooperative=FALSE;
  par->queen=FALSE;

  par->pirates=TRUE;
  par->enemyknown=FALSE;
  par->menu=TRUE;

  par->server=FALSE;
  par->client=FALSE;
  strncpy(par->IP,DEFAULT_IP,MAXTEXTLEN);strncpy(&par->IP[MAXTEXTLEN-1],"\0",1);
  par->port=DEFAULT_PORT;
  par->port2=DEFAULT_PORT+1;
  strcpy(par->playername,"player"); // HERE set default
  
  strcpy(par->font,"6x13");
  strcpy(par->geom,"1024x550");
}

void SetDefaultUserKeys(struct Keys *keys){
  keys->fire.value=32;
  keys->turnleft.value=65361;
  keys->turnright.value=65363;
  keys->accel.value=65362;
  /*
    keys->automode.value=105; 
    keys->manualmode.value=97; 
  */
  keys->automode.value=65364;
  keys->manualmode.value=65362;
  keys->map.value=109;
  keys->order.value=111;
}
