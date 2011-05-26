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

#include "objects.h"

#define GET 0
#define SET 1



float Random(int);
float Sqrt(int n);
void delay(int time);
void AddGold(struct Player *,int player,int n);
int GetControl(struct Player *,int player);
int GetPlayerProc(struct Player *,int player);
int GameParametres(int option,int param,int value);

int Proc(int option,int value);
int GetProc(void);
int SetProc(int value);
int NProc(int option,int value);
int GetNProc(void);
int SetNProc(int value);

int sTime(int action,int t);
int GetTime(void);
void SetTime(int t);
void IncTime(void);

void DelCharFromCad(char *cad,char *filter);
