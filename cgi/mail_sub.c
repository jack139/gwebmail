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
#include "base64.h"
#include "lang.h"

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


/*----------------------------------------------------+
  get_mail_by_POP3()
+----------------------------------------------------*/
long get_mail_by_POP3(const char *uid, const char *mid, char *buf, 
		     long buf_size, int del_it)
{
  int sock, m_mails;
  long t_size, ttt, ret=-1;
  int  i, j, f_cnt, ll;
  char temp[300], pwd[50], m_uidl[50];
  FILE *fp;
  char *p;


  if (db_get_pwd(uid, pwd)==-1) return -1;

  sock=init_POP3(POP3_HOST);
  if (sock==-1)
    {
      warn(W_MSR_ERR);
      return -1;
    }
  
  read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
  if (sock_buf[0]!='+')
    {
      warn(W_MSR_ERR);
      goto quit_pop;
    }

  sprintf(temp, "USER %s\r\n", uid);
  write_command(sock, temp, strlen(temp));

  read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
  if (sock_buf[0]!='+') 
    {
      warn(W_MSR_ERR);
      goto quit_pop;
    }

  sprintf(temp, "PASS %s\r\n", pwd);
  write_command(sock, temp, strlen(temp));
  
  read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
  if (sock_buf[0]!='+')
    {
      warn(W_MSR_ERR);
      goto quit_pop;
    }
      
  sprintf(temp, "STAT\r\n");
  write_command(sock, temp, strlen(temp));
      
  read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
  if (sock_buf[0]!='+')
    {
      warn(W_MSR_ERR);
      goto quit_pop;
    }

  sscanf(sock_buf, "%s %d %ld", temp, &m_mails, &ttt);

  for(i=1; i<=m_mails; i++)
    {
      sprintf(temp, "uidl %d\r\n", i);
      write_command(sock, temp, strlen(temp));
      
      read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
      if (sock_buf[0]!='+')
	{
	  warn(W_MSR_ERR);
	  goto quit_pop;
	}

      sscanf(sock_buf, "%s %ld %s", temp, &ttt, m_uidl);

      if (strcmp(mid, m_uidl)!=0) continue;

      /* ready to retrieve whole mail */
      
      sprintf(temp, "list %d\r\n", i);
      write_command(sock, temp, strlen(temp));
      
      read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
      if (sock_buf[0]!='+')
	{
	  warn(W_MSR_ERR);
	  goto quit_pop;
	}
      
      sscanf(sock_buf, "%s %ld %ld", temp, &ttt, &t_size);

      /* check buf size */
      if (t_size>buf_size)
	{
	  buf_size=t_size+1024;
	  if (realloc(buf, buf_size)==NULL)
	    {
	      warn(W_NO_MEM);
	      goto quit_pop;
	    }
	}

      /*
      sprintf(temp, "RETR %d\r\n", i);
      write_command(sock, temp, strlen(temp));
      
      read_respose(sock, buf, buf_size, "\r\n.\r\n"); 
      if (buf[0]!='+')
	{
	  warn(W_MSR_ERR);
	  goto quit_pop;
	}
      */

      sprintf(temp, SPOOL_PATH"/%s", uid);
      fp=fopen(temp, "r");
      if (fp==NULL)
	{
	  warn("Cannot open spool file.");
	  goto quit_pop;
	}

      f_cnt=i;

      for(j=0; j<f_cnt; j++)
	{
	  while(1)
	    {
	      if (fgets(sock_buf, SOCKSIZE, fp)==NULL) break;
	      if (strncmp(sock_buf, "From ", 5)==0) break;
	    }
	}

      //      printf("%s<br>", sock_buf);

      p=buf;
      ll=strlen(sock_buf)-1;
      memcpy(sock_buf+ll, "\r\n", 2);
      ll+=2;

      memcpy(p, sock_buf, ll);
      p+=ll;

      while(1)
	{
	  if (fgets(sock_buf, SOCKSIZE, fp)==NULL) break;
	  if (strncmp(sock_buf, "From ", 5)==0) break;

	  //	  printf("%s<br>", sock_buf);

	  ll=strlen(sock_buf)-1;
	  sock_buf[ll]='\0';
	  memcpy(sock_buf+ll, "\r\n", 2);
	  ll+=2;

	  memcpy(p, sock_buf, ll);
	  p+=ll;
	}

      //      printf("<br>%s<br>", sock_buf);

      memcpy(p, "\r\n.\r\n", 5);
      *(p+5)='\0';

      fclose(fp);



      if (del_it)
	{
	  sprintf(temp, "DELE %d\r\n", i);
	  write_command(sock, temp, strlen(temp));

	  read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
	  if (sock_buf[0]!='+')
	    {
	      warn(W_MSR_ERR);
	      goto quit_pop;
	    }
	}


      break;
    }

  ret=buf_size;

 quit_pop:
  
  sprintf(temp, "quit\r\n");
  write_command(sock, temp, strlen(temp));

  read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
  
  close_POP3(sock);

  return ret;
}


/*-----------------------------------------------------
  filt_subject()
------------------------------------------------------*/
char *filt_subject(char *subject)
{
  char *p;
  int l;

  l=strlen(subject);

  if (strncasecmp(subject, "=?gb2312?B?", 11)==0)
    {
      p=strstr(subject+11, "?=");
      if (p==NULL) return subject;

      *p='\0';

      b64_decode(subject+11, (long)l, subject);
    }

  
  return subject;
}


/*---------------------------------------------------
  set_proc_uid()
  ret:
  0  succ
  -1 fail
-----------------------------------------------------*/
int set_proc_uid(char *user)
{
  return (setuid(0));
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
	 

