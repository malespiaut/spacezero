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


#include "clock.h"

double Clock(int id,int action){
  /* 
     ten clocks 
  */
  static struct Clock clock[10]; 
  struct timeval now;

  if(id>9)return(0);
  if(id<0)return(0);

  switch(action){
  case CL_CLEAR:
    clock[id].time=0;
    break;
  case CL_START:
    gettimeofday(&clock[id].starttime,NULL);
    break;
  case CL_STOP:
    gettimeofday(&now,NULL);
    decr_timeval(&now,&clock[id].starttime);
    clock[id].time+= (double) (now.tv_sec+now.tv_usec/1.0e6);
      break;
  case CL_READ:
    return(clock[id].time);
    break;
  default:
    return(0);
    break;
  }
  return(0);
}

