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
#include <string.h>
#include "help.h"
#include "spacecomm.h" 


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
		  "-soundvol n\t where n is the sound effects volume. (0..100)\n" );
  (void) fprintf( stdout, 
		  "-musicvol n\t where n is the music volume. (0..100)\n" );
  (void) fprintf( stdout, 
		  "-nosound\t sound disabled.\n" );
  (void) fprintf( stdout, 
		  "-nomusic\t music disabled.\n" );
  (void) fprintf( stdout, 
		  "-name playername where playername is the name of the player.\n" );
  (void) fprintf( stdout, 
		  "-font font\t change the default font by font.\n" );
  (void) fprintf( stdout, 
		  "-nomenu \t Starts automatically, without menu.\n" );
  (void) fprintf( stdout, 
		  "-lang [en,es,it] Game language. English(default), spanish and italian supported.\n" );
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
		  "-enemyknown\t All enemies are known.\n" );
  (void) fprintf( stdout, 
		  "-noenemyknown\t All enemies are unknown (default).\n" );
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
		  "left-right arrows turn left-right/manual mode.\n");
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
		  "Ctrl-w\t\t Write a meesage to other players.\n");
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




void PrintWarnings(char *version){
  fprintf(stderr,"**************************************************************\n");
  fprintf(stderr,"WARNING: This is the development version of SpaceZero (version %s)\nIt can contain bugs.\nNet and saved games can be incompatible with the official release.\nMaybe SpaceZero doesn't work properly.\n", version);

  if(sizeof(int)!=4){
    fprintf(stderr,"\nWARNING: size of int %d bytes\n, maybe SpaceZero doesn't work properly\n",(int)sizeof(int));
  }
  fprintf(stderr,"\nWARNING: Communication between 64 and 32 bits machines not so tested,\nmaybe SpaceZero doesn't work properly.\n");


  fprintf(stderr,"**************************************************************\n");
}


