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

#ifndef _LOCALES_
#define _LOCALES_

enum LocaleText{
  /* The  texts included in SpaceZero. */
  /* game menu */

  L_HOPTIONS,L_HMULTIPLAYEROPTIONS,L_HGENERALOPTIONS,L_HGAMEOPTIONS,L_HKEYBOARD,
  L_OPTIONS,L_STARTGAME,L_QUITGAME,L_GENERALOPTIONS,L_GAMEOPTIONS,L_KEYBOARD,L_MULTIPLAYEROPTIONS,L_DEFAULTOPTIONS,
  L_NAME,L_SVOL,L_MVOL,L_WINDOWGEOM,
  L_NUMPLAYERS,L_NUMPLANETS,L_NUMGALAXIES,L_SIZEUNIVERSE,L_PLANETKNOWN,L_PIRATES,L_ENEMIESKNOWN,L_COOPMODE,L_COMPMODE,L_QUEENMODE,
  L_SHOOT,L_TURNLEFT,L_TURNRIGHT,L_ACCEL,L_MANUALMODE,L_AUTOMODE,L_MAP,L_MORDER,
  L_IPADDRESS,L_PORT,L_STARTSERVER,L_CONNECTSERVER,

  /* windows menu */

  L_MSAVE,L_MLOAD,L_MOPTIONS,L_MABOUT,L_MHELP,

  /* game texts */

  L_ENERGY,L_STATE,L_EXPERIENCE,L_AMM,L_SECTOR,L_SECTORS,L_KILLS,L_ORDER,L_DISTANCE,L_TIME,L_RECORD,L_PLAYER,L_GOLD,L_NPLANETS,L_NSHIPS,L_DEATHS,L_POINTS,
  L_PLANET,L_RESOURCES,L_MASS,L_LEVEL,L_GAMESTATISTICS,L_PLANETS,L_SHIPS,L_dPLAYER,
  /* orders */

  L_GOTO,L_TAKEOFF,L_ORBIT,L_NOTHING,L_STOP,L_EXPLORE,L_RETREAT,

  /* messages */

  L_ENEMIESNEAR,L_UNDERATTACK,L_ASTEROIDSNEAR,L_ARRIVEDTO,L_RECEIVEDINFO,
  L_HELP,L_SHIPDESTROYED,L_QUEENDESTROYED,L_LOST,
  L_EJECTING,L_SAVEDINPLANET,L_PILOT,L_PIRATESATSECTOR,L_ASTEROIDSATSECTOR,
  L_GAMESAVED,L_GAMESAVEDTO,L_RESCUEDBY,L_DISCOVERED,
  
  /* order messages*/
  
  L_CANTSELLPILOTS,L_MUSTBELANDED,L_NOTALLOWED,L_PLANETORSPACESHIPUNKNOWN,L_GOINGTOSECTOR,L_GOINGTOPLANET,L_GOINGTOSPACESHIP,L_GOINGTOEXPLORE,L_TAKINGOFF,
  L_EXPLORER,L_FIGHTER,L_TOWER,L_SPACESHIPMUSTBELANDED,L_YOUHAVENOTGOLD,L_BUYED,L_SELECTED,L_SPACESHIPSSELECTED,
  L_DESTINYEQUALORIGIN,
  L_INTRODUCECOMMAND,L_SELLINGSPACESHIP,L_YOUCANUPGRADE,L_UPGRADETOLEVEL,
  
  /* Options window */

  L_CHANGESWILL,L_SOUNDANDMUSICWHENSAVE,L_KNOWNUNIVERSE,L_MUSICOFF,L_SOUNDOFF,L_NOFPLANETS,L_NOFPLAYERS,L_UNIVERSESIZE,L_NOFGALAXIES,L_COOPERATIVE,L_COMPUTER,L_QUEEN,L_SAVE,L_SETDEFAULT,L_CANCEL,

  /* new */
  L_SATELLITE,
  L_NOROOM,

  L_NUM};/* L_NUM contains the amount of entries in the enum. Keep it the last! */


int GetLocales(char *fname);
int GetLocaleNum(void);
char *GetLocale(enum LocaleText n);

#endif
