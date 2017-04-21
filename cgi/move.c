/* move.c */

#include <stdio.h>
#include <time.h>
#include "cgic.h"
#include "db.h"
#include "mail_sub.h"
#include "lang.h"

#define MIDSIZE 40
#define MID(i) (m_mid+MIDSIZE*i)

int move();

int cgiMain() {
  char uid[MAXID+1],des[40];

  cgiHeaderContentType("text/html");
 
  if (move()==-1)
    {
      fprintf(cgiOut, "<html><head>\n");
      fprintf(cgiOut,"<meta http-equiv=Content-Type "
	      "content=\"text/html; charset=gb2312\">\n");
      fprintf(cgiOut, "<title>move</title></head>\n");

      fprintf(cgiOut, "<body bgcolor=#99c8ff text=#0033cc>\n");

      fprintf(cgiOut, "<p align=center>　</p>\n");
      fprintf(cgiOut, "<p align=center><font size=6><b>"
	      M_SYSINFO"</b></font></p>\n");

      fprintf(cgiOut, "<p align=center>　</p>\n");
      fprintf(cgiOut, "<p align=center>　</p>\n");

      fprintf(cgiOut, "<p align=center><font color=#FF0000>"
	      "%s</font></p>\n", g_info);

      fprintf(cgiOut, "<p align=center>　</p>\n"); 

      fprintf(cgiOut, "</body></html>\n");
    }

  return 0;
}


/*---------------------------------------
 move(): move mail 
 para:   
   r_uid   retrive uid
   r_des   retrive md5_id
 ret:                               
   0       success                     
   -1      fail
----------------------------------------*/
int move()
{
  char uid[MAXID+1],des[40],src_f[21],dest_f[21];
  int m_all, check_all;
  char *m_mid;
  int result, i, is_inbox;
  char temp[300];
  int t_mails, t_unread;
  long t_size;
  
  result=cgiFormStringNoNewlines("uid", uid, MAXID+1);
  if (result!=cgiFormSuccess)
    {
      warn(W_CGI_ERR);
      return -1;
    }

  result=cgiFormStringNoNewlines("des", des, 35);
  if (result!=cgiFormSuccess)
    {
      warn(W_CGI_ERR);
      return -1;
    }

  if (db_flash_active(uid,des)==-1) return -1;

  if (set_proc_uid(uid)==-1) return -1;

  result=cgiFormStringNoNewlines("all", temp, 10);
  if (result!=cgiFormSuccess)
    {
      warn(W_CGI_ERR);
      return -1;
    }

  if ((m_all=atoi(temp))<0)
    {
      warn(W_SYS_ERR);
      return -1;
    }
  
  result=cgiFormStringNoNewlines("src", src_f, 21);
  if (result!=cgiFormSuccess)
    {
      warn(W_CGI_ERR);
      return -1;
    }

  result=cgiFormStringNoNewlines("dest", dest_f, 21);
  if (result!=cgiFormSuccess)
    {
      warn(W_CGI_ERR);
      return -1;
    }

  sprintf(temp, "select count(*) from folder where"
	  " uid='%s' and folder_n='%s'", uid, src_f);
  if (db_exec(temp,1)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }
  if (atoi(g_buf[0][0])==0)
    {
      warn(W_NO_FOLDER);
      return -1;
    }

  sprintf(temp, "select count(*) from folder where"
	  " uid='%s' and folder_n='%s'", uid, dest_f);
  if (db_exec(temp,1)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }
  if (atoi(g_buf[0][0])==0)
    {
      warn(W_NO_FOLDER2);
      return -1;
    }
     
  m_mid=(char *)malloc(MIDSIZE*(m_all+1));

  /*
    get and check which mail to move .
    if the mail to be moved, MID(i)=mail_id,
    or MID(i)=NULL.
  */
  result=cgiFormCheckboxSingle("checkall");
  if (result==cgiFormSuccess) check_all=1;
  else check_all=0;

  for(i=1; i<=m_all; i++)
    {
      if (check_all==0)
	{
	  sprintf(temp, "check%-d", i);
	  result=cgiFormCheckboxSingle(temp);
	  if (result!=cgiFormSuccess)
	    {
	      *MID(i)='\0';
	      continue;
	    }
	}

      sprintf(temp, "mid%-d", i);
      result=cgiFormStringNoNewlines(temp, MID(i), MIDSIZE);
      if (result!=cgiFormSuccess)
	{
	  warn(W_CGI_ERR);
	  return -1;
	}
    }

  if (strcmp(src_f, INBOX_NAME)==0) is_inbox=1;
  else is_inbox=0;

  /* 
     move mails
  */
      
  if (strcmp(dest_f, INBOX_NAME)==0)
    {
      warn(W_SYS_ERR);
      return -1;
    }
      
  t_mails=0;
  t_unread=0;
  t_size=0L;
  for (i=1; i<=m_all; i++)
    {
      long tt_sz;

      if (*MID(i)=='\0') continue;
      
      sprintf(temp, "select unread,size from mail where"
	      " uid='%s' and mail_id='%s'", uid, MID(i));
      if (db_exec(temp, 1)<1)
	{
	  warn(W_DB_ERR);
	  return -1;
	}
      
      if (atoi(g_buf[0][0])==1) t_unread++;
      
      tt_sz=atol(g_buf[0][1]);
      t_size+=tt_sz;
      t_mails++;


      sprintf(temp, "update mail set folder_n='%s'"
	      " where uid='%s' and mail_id='%s'", dest_f, uid, MID(i));
      if (db_exec(temp, 0)==-1)
	{
	  warn(W_DB_ERR);
	  return -1;
	}


      if (is_inbox)
	{
	  char *ptr1, *ptr2, *ptr3, *p, ch;
	  long real_size, tt;

#define UPDATESIZE 1024*64

	  ptr1=(char *)malloc(tt_sz+2048);
	  if (ptr1==NULL)
	    {
	      warn(W_NO_MEM);
	      return -1;
	    }

	  ptr2=(char *)malloc((1024+UPDATESIZE)*2);
	  if (ptr2==NULL)
	    {
	      warn(W_NO_MEM);
	      free(ptr1);
	      return -1;
	    }

	  ptr3=ptr2+(1024+UPDATESIZE);

	  real_size=get_mail_by_POP3(uid, MID(i), ptr1, tt_sz+2048, 1);
	  if (real_size==-1)
	    {
	      free(ptr1);
	      free(ptr2);
	      return -1;
	    }
	  
	  //	  printf("%s<br>", ptr1);
	  
	  tt=real_size;
	  p=ptr1;
	  if (tt>UPDATESIZE)
	    {
	      do
		{
		  long l_size;

		  if (tt>UPDATESIZE) l_size=UPDATESIZE;
		  else l_size=tt;

		  memcpy(ptr3, p, l_size);
		  ptr3[l_size]='\0';
		  
		  if (db_prepare(ptr3)==NULL)
		    { free(ptr1); free(ptr2); return -1; }
		  sprintf(ptr2, "update mail set content=concat(content,'%s')"
			  " where uid='%s' and mail_id='%s'", 
			  ptr3, uid, MID(i));
		  //		  printf("<br>%s<br>", ptr2);
		  if (db_exec(ptr2, 0)==-1)
		    {
		      puts(ptr2);
		      warn(W_DB_ERR);
		      free(ptr1); free(ptr2);
		      return -1;
		    }
		  p+=(UPDATESIZE);
		  tt-=(UPDATESIZE);
		}
	      while(tt>0);
	    }
	  else
	    {
	      memcpy(ptr3, p, tt);
	      ptr3[tt]='\0';

	      if (db_prepare(ptr3)==NULL) return -1;
	      sprintf(ptr2, "update mail set content='%s'"
		      " where uid='%s' and mail_id='%s'", ptr3, uid, MID(i));
	      if (db_exec(ptr2, 0)==-1)
		{
		  puts(ptr2);
		  warn(W_DB_ERR);
		  free(ptr1);
		  free(ptr2);
		  return -1;
		}
	    }
	  free(ptr1);
	  free(ptr2);
	}		
    }



  sprintf(temp, "update folder set mails=mails-%d, unread=unread-%d,"
	  "size=size-%ld where uid='%s' and folder_n='%s'", 
	  t_mails, t_unread, t_size, uid, src_f);
  if (db_exec(temp, 0)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }
   
  sprintf(temp, "update folder set mails=mails+%d, unread=unread+%d,"
	  "size=size+%ld where uid='%s' and folder_n='%s'",
	  t_mails, t_unread, t_size, uid, dest_f);
  if (db_exec(temp, 0)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }


  free(m_mid);


  fprintf(cgiOut, "<html><head>\n");
  fprintf(cgiOut, "<meta HTTP-EQUIV=refresh "
	  "CONTENT=\"0; URL=/cgi-bin/folder?box=%s&uid=%s&des=%s\">\n",
	  src_f, uid, des);
  fprintf(cgiOut, "<meta http-equiv=Content-Type "
	  "content=\"text/html; charset=gb2312\">\n");
  fprintf(cgiOut, "<title></title></head><body></body></html>\n");


  return 0;
}











