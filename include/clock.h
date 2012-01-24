 /*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyright (C) 2006-2012  MRevenga
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

#ifndef _CLOCK_
#define _CLOCK_

#include <sys/time.h>
#include <stdio.h>

#define CL_CLEAR 0
#define CL_START 1
#define CL_STOP 2
#define CL_SUM 3
#define CL_READ 4


#define decr_timeval(t1, t2) \
do { \
  if ((t1)->tv_usec < (t2)->tv_usec) { \
    (t1)->tv_sec -= (t2)->tv_sec + 1; \
    (t1)->tv_usec = (t1)->tv_usec + 1000000 - (t2)->tv_usec; \
  } else { \
    (t1)->tv_sec -= (t2)->tv_sec; \
    (t1)->tv_usec -= (t2)->tv_usec; \
  } \
} while (0)


struct Clock{
  double time;
  struct timeval starttime;
};

double Clock(int id,int action);

#endif
