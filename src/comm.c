 /*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyright(C) 2006-2011  M.Revenga
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "comm.h"


int send_buf(int sfd,char *buf,int nbytes){
  /* 
     copy nbytes from buf in the socket defined by sfd
     the first data send is the number of bytes nbytes as a header
     returns: the number of bytes copied except the header.
  */
  int writed,remainbytes;
  int *a;

  /*  printf("(sendbuf)sfd: %d\n",sfd); */

  a=&nbytes;

  /* send header: a int with the number of bytes */

  remainbytes=sizeof(int);

  while(remainbytes>0){
    writed =write(sfd,a,remainbytes);
    if(writed==-1){
      perror("send_buf() write");
    }
    /*    printf("writed %d bytes\n",writed); */
    if (writed == -1)return(-1);
    remainbytes -= writed;
    a += writed;
  }

  /* send the rest of the data */

  remainbytes=nbytes;
  /*  printf("sending %d bytes\n",remainbytes); */
  while(remainbytes>0){
    writed =write(sfd,buf,remainbytes);
    if (writed == -1)return(-1);
    /*    printf("send:  %s\n",buf); */
    remainbytes -= writed;
    buf += writed;
  }
  return(nbytes-remainbytes);
}

int recv_buf(int sfd,char *buf){
  /*
    copy nbytes from the socket sfd in buf. no realloc. be carefull
    nbytes is the number of bytes to be copied
    nbytes is the first int received as a header
    return: the number of bytes copied without including the header 

  */ 
  
  int nbytes;
  int a,*b;
  int n,nb;  
  char *c;
  
  n=0;
  c=buf;
  b=&a;

  nbytes=sizeof(int);

  /* reading the header, a int with the size of the rest of the message*/

  while(nbytes>0){
    if((nb=read(sfd,b,nbytes))== -1){
      perror("recv_buf() read 1");
      exit(-1);
      return(-1);
    }
    nbytes-=nb;
    b+=nb;
  }
  b=&a;

  nbytes=(int)*b;
  /*  printf("receiving %d bytes\n",nbytes); */
  /*  printf("(recv_men) numero de bytes:%d de tamano: %d\n",nbytes,nb); */

  while(nbytes>0){
    if((nb=read(sfd,buf,nbytes))== -1){
      perror("recv_buf() read 2");
      exit(-1);
    }
    /*    printf("received %d bytes of %d\n",nb,nbytes); */
    /*    printf("received:  %s\n",buf); */
    nbytes-=nb;
    buf+=nb;
    n+=nb;
  }

/*   printf("(recv_men) mensaje recibido: %d\n",buf[0]); */
  /*  *buf=0; */
  return(n);
}



int send_message(int sfd,struct Message *mess){
  int writed,remainbytes;
  char *buf;
  
  /* testing the header */

  if(mess->header.nbytes>BUFFERSIZE){
    fprintf(stderr,"warning: Message too large (%d bytes), truncate to %d\n",
	    mess->header.nbytes,BUFFERSIZE);
    mess->header.nbytes=BUFFERSIZE;
    /*    Print_Message(mess); */
  }


  /* sending the header */

  remainbytes=sizeof(struct MessageHeader);
  buf=(char *)mess;

  while(remainbytes>0){
    writed =write(sfd,buf,remainbytes);
    if(writed==-1){
      perror("send_message() write");
    }
    /*    printf("writed %d bytes\n",writed); */
    if (writed == -1){
      perror("send_message() write");
      exit(-1);
    }
    remainbytes -= writed;
    buf += writed;
  }

  /* sending the data */

  remainbytes=mess->header.nbytes;
  buf=mess->buf;

  /*  printf("sending %d bytes\n",remainbytes); */
  while(remainbytes>0){
    writed =write(sfd,buf,remainbytes);
    if (writed == -1)return(-1);
    /*    printf("send:  %s\n",buf); */
    remainbytes -= writed;
    buf += writed;
  }
  return(mess->header.nbytes-remainbytes);
}


int recv_message(int sfd,struct Message *mess){
  int nbytes;
  int n,nb;  
  char *buf;

  n=0;

  /*receive the header */
  buf=(char *)&(mess->header);
  nbytes=sizeof(struct MessageHeader);

  while(nbytes>0){
    if((nb=read(sfd,buf,nbytes))== -1){
      perror("recv_message() read");
      exit(-1);
    }
    nbytes-=nb;
    buf+=nb;
  }

  nbytes=mess->header.nbytes;
  buf=mess->buf;
  /*  printf("receiving %d bytes\n",nbytes); */
  /*  printf("(recv_men) numero de bytes:%d de tamaño: %d\n",nbytes,nb); */

  while(nbytes>0){
    if((nb=read(sfd,buf,nbytes))== -1){
      perror("recv_message() read");
      exit(-1);
    }
    /*    printf("received %d bytes of %d\n",nb,nbytes); */
    /*    printf("received:  %s\n",buf); */
    nbytes-=nb;
    buf+=nb;
    n+=nb;
  }

/*   buf=c; */
/*   printf("(recv_men) mensaje recibido: %d\n",buf[0]); */
  /*  *buf=0; */
  return(n);
}


int SendFile(int fd,int sfd){
  int nbytes=0;

  struct Buffer buffer;

  buffer.data=malloc(BUFFERSIZE*sizeof(char));
  if(buffer.data==NULL){ 
    fprintf(stderr,"ERROR in malloc (Sendfile)\n"); 
    exit(-1); 
  } 
  buffer.size=BUFFERSIZE;
  buffer.n=0;

  nbytes=CopyFile2Buf(fd,&buffer);
  /*  printf("file size: %d\n",nbytes); */
  /*  printf("(sendfile)sfd: %d\n",sfd); */
  send_buf(sfd,buffer.data,nbytes);

  free(buffer.data);
  return(0);
}


int RecvFile(int fd,int sfd){
  int nbytes=0;
  char *buf;

  struct Buffer buffer;

  buffer.data=malloc(BUFFERSIZE*sizeof(char));
  if(buffer.data==NULL){ 
    fprintf(stderr,"ERROR in malloc (Sendfile)\n"); 
    exit(-1); 
  } 
  buffer.n=0;
  buffer.size=BUFFERSIZE;
  buf=buffer.data;

  printf("Receiving file. buffer size: %d\n",buffer.size);

  if((nbytes=RecvBuffer(sfd,&buffer))==-1){
    perror("RecvFile() recv_buf");
    exit(-1);
  }
  buf=buffer.data;
  nbytes=CopyBuf2File(buf,fd,nbytes);

  free(buffer.data);  
  return(nbytes);
}

int CopyFile2Buf(int fd,struct Buffer *buffer){
  /*
    copy the file fd to the buffer buffer reallocating if necessary
    returns:
    the number of bytes copied.
   */

  int nbytes=0;
  char *buf;

  buf=buffer->data+nbytes;

  while(read(fd,buf,1)>0){
    nbytes++;
    buf++;
    if(nbytes==buffer->size-1){
      int newsize;
      newsize=(int)(buffer->size*1.1);
      buffer->data=realloc(buffer->data,newsize*sizeof(char));
      if(buffer->data==NULL){ 
	fprintf(stderr,"ERROR in malloc Copyfile2Buf()\n");
	exit(-1);
      } 
      buffer->size=newsize;
      buf=buffer->data+nbytes;
      //      printf("Realloc(cfile2b) %d\n",buffer->size);
    }
  }
  printf("NBYTES:%d\n",nbytes);
  return(nbytes);
}

int CopyBuf2File(char *buf,int fd,int nbytes){
  int remainbytes=nbytes;
  int bw,nb=0;

  while(remainbytes>0){
    bw=write(fd,buf,remainbytes);
    if(bw==-1){
      perror("CopyBuf2File() write");
      exit(-1);
    }
    remainbytes-=bw;
    nb+=bw;
    buf+=bw;
  }
  return(nb);
}


int SendBuffer(int sfd,struct Buffer *buffer){
  /* 
     copy all the buffer to sfd
     returns: the number of bytes copied.
  */
  int writed,remainbytes;
  char *buf;

  /* send the data */

  //  printf("Sending buffer %d\n",buffer->n);

  remainbytes=buffer->n;
  buf=buffer->data;
  /*  printf("sending %d bytes\n",remainbytes); */

  /* write the number of bytes to send */

  writed=write(sfd,&buffer->n,sizeof(int));
  if (writed == -1)return(-1);

  while(remainbytes>0){
    writed =write(sfd,buf,remainbytes);
    if (writed == -1)return(-1);
    /*    printf("send:  %s\n",buf); */
    remainbytes -= writed;
    buf += writed;
  }
  //  printf("Sended buffer\n");
  return(buffer->n-remainbytes);
}



int RecvBuffer(int sfd,struct Buffer *buffer){
  /*
    copy nbytes from the socket sfd in buf
    nbytes is the number of bytes to be copied
    nbytes is the first int received as a header
    return: the number of bytes copied without including the header 

  */ 
  
  int nbytes;
  int a,*b;
  int nb;  
  char *buf;
  
  b=&a;


  /* reading the header, a int with the size of the rest of the message*/
  nbytes=sizeof(int);
  while(nbytes>0){
    if((nb=read(sfd,b,nbytes))== -1){
      perror("recvBuffer() read 1");
      exit(-1);
    }
    nbytes-=nb;
    b+=nb;
  }
  b=&a;

  nbytes=(int)*b; /* bytes to receive */
  //  printf("RecvBuffer(): receiving %d bytes\n",nbytes); 
  /*  printf("(recv_men) numero de bytes:%d de tamano: %d\n",nbytes,nb); */

  if(nbytes>buffer->size){
    buffer->data=realloc(buffer->data,nbytes*sizeof(char));
    if(buffer->data==NULL){ 
      fprintf(stderr,"ERROR in malloc Copyfile2Buf()\n");
      exit(-1);
    } 
    buffer->size=nbytes;
    //    printf("Realloc(recvbuffer) %d\n",nbytes);
  }

  buffer->n=0;
  buf=buffer->data;

  while(nbytes>0){
    if((nb=read(sfd,buf,nbytes))== -1){
      perror("recvBuffer() read 2");
      exit(-1);
    }
    /*    printf("received %d bytes of %d\n",nb,nbytes); */
    /*    printf("received:  %s\n",buf); */
    nbytes-=nb;
    buf+=nb;
    buffer->n+=nb;
  }

/*   printf("(recv_men) mensaje recibido: %d\n",buf[0]); */
  /*  *buf=0; */
  return(buffer->n);
}



