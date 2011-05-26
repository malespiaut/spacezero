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

#ifndef _GENERAL_
#define _GENERAL_

#ifndef TRUE
#define FALSE 0
#define TRUE 1
#endif

//#define GDK_DISABLE_DEPRECATED

#define CELLON 1  /* HERE include in code */

/*  compile options: */
#define SOUND 1  // compile with sound
#define DEBUG 0
#define TEST 0  // test stuff
/*  --compile options: */

#define MODI(a) ((a) > 0 ? (a) : (-a))
#define PI 3.14159265



#define TEXTMENMAXLEN 48
#define PLAYERNAMEMAXLEN 32
/* window properties */
#define DEFAULTWIDTH 1024//900//1024//800//1024//1152
#define DEFAULTHEIGHT 500//600 //500 //550//700//768
#define LXFACTOR 1024  /* normalization factor for surface planets */
#define LYFACTOR 500   /* normalization factor for surface planets */
#define PANEL_HEIGHT 20

#define CLOSEDUNIVERSE 0
#define NUMSTARS 200
#define GRAVITATION_RANGE2 4000000  /* maximo alcance de la gravedad */
#define G .1 /* gravitation constant */
#define DT .42
#define RESOURCEFACTOR 1.2 /* velocity factor for getting resource from planets */
#define COSTFACTOR 1.0     /* multiplicative factor to ships cost */
#define DAMAGEFACTOR 1.0;  /* multiplicative factor to weapon damage */

#define PLANETSKNOWN 0
#define ENEMIESKNOWN 0
#define SLOWMACHINE 0


/* default options */
#define NUMPLAYERS 2
#define NUMGALAXIES 1
#define NUMPLANETS 30 /* num. planets in galaxy */
#define ULX 100000//200000L /* universe size */
#define ULY 100000//200000L /* universe size */


/* max and min option values */
#define MAXNUMPLAYERS 8
#define MINNUMPLAYERS 2
#define MAXNUMGALAXIES 10
#define MINNUMGALAXIES 1
#define MAXNUMPLANETS 200 /* num. planets in galaxy */
#define MINNUMPLANETS 2 /* num. planets in galaxy */
#define MAXULX 1000000//200000L
#define MINULX 10000//200000L

#define MAXNUMTEAMS MAXNUMPLAYERS

/* player control */
#define HUMAN 0
#define COMPUTER 1


/* mode games*/
#define LOCAL 0
#define SERVER 1
#define CLIENT 2

/* navigation modes */
#define ABSOLUTE 0
#define RELATIVE 1


#define RADAR_RANGE 2000
#define RADAR_RANGE2 4000000
#define SECTORSIZE 1410 // sqrt(RADAR_RANGE2/2)

/* FILES */
#define SAVEDIR ".spacezero/"
#define SAVETMPFILE "/tmp/tmpspacezero"
#define SAVEFILE0 "save"
#define SAVEFILE1 "savecl"
#define SAVEFILENET "saven"
#define SAVEFILESINGLE SAVEFILE0 
#define RECORDFILE "record"
#define OPTIONSFILE "options"
#define DATADIR0 "./dat/"
#define DATADIR1 "/usr/local/share/spacezero/" /* tar.gz */
//#define DATADIR1 "/usr/share/games/spacezero/" /* DEB pkg*/



/* planet properties */
#define MINPLANETMASS 35000
#define MAXPLANETMASS 160000
#define MINPLANETGOLD 2000
#define MAXPLANETGOLD 8000
#define MAXPLANETHEIGHT 0.3*DEFAULTHEIGHT
#define FACTORLANDZONESIZE 0.2*DEFAULTWIDTH
#define LANDZONEMINSIZE 120
#define LANDZONEMINX 175

/* miscelanea */
#define MINTTL -128 /* with less value ships are forgotten*/

/* Game parametres */


#define DEFAULT    0
#define GULX       1        /* universe size */
#define GULY       2        /* universe size */
#define GWIDTH     3        /* window size */
#define GHEIGHT    4        /* window size */
#define GPANEL     5        /* shell panel size */
#define GNET       6        /* TRUE : NET , FALSE : LOCAL */
#define GMODE      7        /* LOCAL, SERVER, CLIENT */
#define GCOOPERATIVE     8  /* Cooperative mode game */
#define GCOMPCOOPERATIVE 9  /* Computer cooperative mode game */
#define GQUEEN     10        /* Queen mode game */
#define GPIRATES   11        /* Create pirates or not */
#define GNGALAXIES 12       /* number of galaxies */
#define GNPLAYERS  13        /* number of players */
#define GNPLANETS  14        /* number of planets */
#define GKPLANETS  15       /* TRUE FALSE planets known or unknown */ 
#define GMUSIC     16       /* TRUE FALSE game paused */
#define GSOUND     17       /* TRUE FALSE game paused */
#define GPAUSED    18       /* TRUE FALSE game paused */
#define GQUIT      19       /* 0,1,2 really quit? */


/*    Error codes for all functions */

enum SZ_Error{
/* Buyship() */
  SZ_OK,
  SZ_OBJNULL,
  SZ_UNKNOWNERROR, 
  SZ_OBJNOTLANDED,  
  SZ_NOTOWNPLANET , 
  SZ_NOENOUGHGOLD,
  SZ_NOTIMPLEMENTED
};



/*   --Error codes for all functions */

struct TextList{
  int color;
  char text[TEXTMENMAXLEN];  /* text */
  struct TextList *next;
};


/* messages among players */


struct TextMessage{
  int n;       /* size of the message */
  int source;  /* source player */
  int dest;    /* dest player   */
  int id;      /* id of the message */
  int print;   /* it is printed */
  int time;    /* time of the text */
  int value ;  /* relevance of the message */
  int duration; /* duration of the text*/
  char text[TEXTMENMAXLEN];  /* text */
};

struct TextMessageList{
  struct TextMessage info;
  struct TextMessageList *next;
};


struct Game{
  int width,height;    /* window size */
  int panel_height;    /* shell height */
  int ulx,uly;         /* Universe size */
  int net;             /* TRUE : NET , FALSE : LOCAL */
  int mode;            /* LOCAL, SERVER, CLIENT */
  int cooperative;
  int compcooperative;
  int queen;
  int pirates;
  int ngalaxies;       /* number of galaxies */
  int nplayers;        /* number of players */
  int nplanets;        /* number of planets */
  int kplanets;        /* TRUE FALSE planets known or unknown */ 
  int paused;          /* TRUE FALSE game paused */
  int music;           /* music ON OFF */
  int sound;           /* sound ON OFF */
  int quit;            /* 0,1,2 really quit? */
};


typedef struct{
  float x,y;
  float mod;
  float a;
}Vector;

typedef struct{
  int x,y;
}Point;

typedef struct{
  int x,y;
  int width,height;
}Rectangle;

typedef struct{
  Rectangle rect;
  int habitat; /* 0 means free space, 1, 2, ... planetid */
}Region;

struct Keys{
  int load,save;
  int left,right,up,down;  /* move ship */
  int tab;                 /* change to next ship */
  int may,ctrl,esc;
  int home;
  int Pagedown,Pageup;         /* next, previous planet */
  int f1,f2,f3,f4,f5,f6,f7,f8;   /* choose ship */
  int space;               /* fire */
  int enter;               /* enter command */
  int back;                /* delete last letter */ 
  int trace;               /* trace on/off */ 
  int mleft;               /* mouse left click*/
  int mright;              /* mouse right click*/
  int mdclick;             /* mouse double click*/
  int b;     /* buy ships */
  int s;     /* open charge  */
  int m;     /* show map   */
  int a;     /* activate-deactivate manual control */
  int n;     /* change navigation mode, no  */
  int i;
  int z;     /* zoom map */
  int l;     /* labels on/off */
  int o;     /* comand shell */
  int p;     /* pause the game */
  int number[10]; /* shell */
  int g;  /* shell */
  int x;  /* shell */
  int t;  /* shell */
  int r;  /* shell */
  int w;  /* shell */
  int e;  /* shell */
  int u;  /* shell */
  int h;
  int d;  /* debug */
  int q;  /* ^Q quit */
  int y;  /* yes */
};





#endif
