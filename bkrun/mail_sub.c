/* mail.c */


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <pwd.h>
#include <shadow.h>
#include <fcntl.h>
#include "mail_sub.h"

#define SPOOL_PATH "/var/spool/mail"

#define IOSIZE (BLOCKSIZE*16)

char sock_buf[SOCKSIZE+1];
char io_buf[IOSIZE+1];

/*---------------------------------------------------+
 init socket to host
-----------------------------------------------------*/
int init_sock(const char *host, const char *port)
{
  int s;

  s=connectTCP(host, port);
  if (s==-1) return -1;

  return s;
}



/*--------------------------------------------------
  read respose from host
---------------------------------------------------*/
long read_respose(int s, char *buf, long sz, const char *end_tag)
{
  time_t tt;
  long buflen;
  ssize_t n;

  time(&tt);
  buf[0]='\0';
  buflen=0;

  fcntl(s, F_SETFL, O_NONBLOCK);

  while(time(NULL)-tt<TIMEOUT) 
    {
      if ((n=read(s, io_buf, IOSIZE))>0)
	{
	  if (n+buflen>=sz) break;
	  strcat(buf, io_buf);
	  buflen+=n;
	  buf[buflen]='\0';
	  if (strstr(buf, end_tag)) break;
	}
    }

  return buflen;
}

/*-------------------------------------------------
   write command to host 
--------------------------------------------------*/
int write_command(int s, const char *buf, long sz)
{
  long offset=0L;

  fcntl(s, F_SETFL, O_FSYNC);

  while(sz>IOSIZE)
    {
      write(s, buf+offset, IOSIZE);
      offset+=IOSIZE;
      sz-=IOSIZE;
    }
  write(s, buf+offset, sz);

  //  fsync(s);

  return 0;
}


/*----------------------------------------------+
  get_a_part() get a part string tag by paras
+-----------------------------------------------*/
char *get_a_part(const char *src, 
		const char *bg_tag, const char*end_tag, long *length)
{
  char *ptr, *ptr2;

  ptr=strstr(src, bg_tag);
  if (ptr==NULL) return NULL;
  
  ptr2=strstr(ptr+strlen(bg_tag), end_tag);
  if (ptr2==NULL) return NULL;
  
  *length=(long)(ptr2-ptr);

  return (ptr);
}


char *get_a_bit_part(const unsigned char *src,     size_t src_l,
		     const unsigned char *bg_tag,  size_t bg_l,
		     const unsigned char *end_tag, size_t end_l,
		     long *length)
{
  unsigned char *ptr, *ptr2;

  ptr=(unsigned char *)memmem(src, src_l, bg_tag, bg_l);
  if (ptr==NULL) return NULL;

  ptr2=(unsigned char *)memmem(ptr+bg_l, 
			       (size_t)(src_l-(ptr-src)-bg_l), 
			       end_tag, end_l);
  if (ptr2==NULL) return NULL;

  *length=(long)(ptr2-ptr);

  return (ptr);
}


/*------------------------------------------+
  filt_str()
+-------------------------------------------*/
char *filt_str(char *str)
{
  char *ptr;

  for(ptr=str; *ptr; ptr++)
    {
      switch(*ptr)
	{
	case '<':
	case '>':
	  *ptr=' ';
	  break;
	}
    }

  return str;
}


/*-------------------------------------------------------------
  giveup_spc()
-------------------------------------------------------------*/
long giveup_spc(char *s, int flag)
{
  char *p1, *p2;

  for(p1=s, p2=s; *p1; p1++)
    {
      switch(flag)
	{
	case D_SPC_AND_CRLF:
	  if (*p1==' ' || *p1=='\r' || *p1=='\n') continue;
	  else
	    {
	      *p2=*p1;
	      p2++;
	    }
	  break;

	case D_SPC:
          if (*p1==' ') continue;
          else
            {
              *p2=*p1;
              p2++;
            }
          break;

	case D_CRLF:
          if (*p1=='\r' || *p1=='\n') continue;
          else
            {
              *p2=*p1;
              p2++;
            }
          break;
	  
	default:
	  *p2=*p1;
	  p2++;
	}
   }      

  *p2='\0';

  return (long)(p2-s);
}
	  

