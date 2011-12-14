 /*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyleft (C) 2006-2011  M.Revenga
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

#include "general.h"
#include "save.h"

#ifndef _HELP_
#define _HELP_

#define ARG_0 0
#define ARG_h 1
#define ARG_g 2
#define ARG_n 3
#define ARG_p 4
#define ARG_t 5
#define ARG_l 6
#define ARG_s 7
#define ARG_c 8
#define ARG_ip 9
#define ARG_port 10
#define ARG_name 11
#define ARG_sound 12
#define ARG_music 13
#define ARG_k 14
#define ARG_cooperative 15
#define ARG_compcooperative 16
#define ARG_queen 17
#define ARG_pirates 18
#define ARG_font 19
#define ARG_geom 20

struct Parametres{
  int ngalaxies;
  int nplanets;
  int nplayers;
  int nteams;
  int ul;
  int kplanets;
  int sound;
  int music;
  int cooperative;
  int compcooperative;
  int queen;
  int pirates;
  int server;
  int client;
  char IP[MAXTEXTLEN];
  int port;
  int port2;
  char playername[MAXTEXTLEN];
  char font[MAXTEXTLEN];
  char geom[MAXTEXTLEN];
};

struct Validargum{
    char cad[MAXTEXTLEN];
    int id;
  };

void PrintWarnings(char *version);
void PrintArguments(struct Parametres param,char *title);
int SearchArg(char *target,struct Validargum *v);
void Usage(char *ver,char *l_rev);
int Arguments(int argc,char *argv[],struct Parametres *,char *optfile);
void SetDefaultParamValues(struct Parametres *par);
int CheckArgs(struct Parametres p);
int GetGeom(char *geom,int *w,int *h);

#endif
