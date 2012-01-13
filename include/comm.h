 /*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyleft (C) 2006-2012  MRevenga
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

#ifndef _COMM_
#define _COMM_ 

#include <stdint.h>
//#define BUFFERSIZE 128*4096
#define BUFFERSIZE 4096


struct MessageHeader{
  uint id;      /* identification of the order */
  uint nobjs;   /* number of objects to send */
  uint nbytes; /* number of bytes to send */
};

struct Message{
  struct MessageHeader header;  /* header of the message */
  char *buf;                    /* buffer of data */
};

struct Buffer{
  char *data;
  uint n;      /* position of read write  */
  uint size;   /* size of the buffer */
};


int send_buf(int sfd,char *buf,int nbytes);
int recv_buf(int sfd,char *buf);
int send_message(int sfd,struct Message *mess);
int recv_message(int sfd,struct Message *mess);

int SendFile(int fd,int sfd);
int RecvFile(int fd,int sfd);
int CopyFile2Buf(int fd,struct Buffer *buf);
int CopyBuf2File(char *buf,int fd,int n);

int SendBuffer(int,struct Buffer *buffer);
int RecvBuffer(int,struct Buffer *buffer);


#endif
