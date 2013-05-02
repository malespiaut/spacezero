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
		version 0.84 April 2013
****/

#ifndef _STATISTICS_
#define _STATISTICS_


#include "general.h"

struct Stats{
  int time;
  float level[MAXNUMPLAYERS+2];
  int nplanets[MAXNUMPLAYERS+2];
};


void InitStatistics(void);

void AddStatistics(struct Stats *s);

void fprintStatistics(FILE *fp);
void fscanfStatistics(FILE *fp);

void PrintStatistics(void);

void DrawStatistics(GdkPixmap *pixmap,Rectangle *r);


#endif
