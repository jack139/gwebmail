/* part.c */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "cgic.h"
#include "db.h"
#include "mail_sub.h"
#include "mime.h"
#include "base64.h"
#include "lang.h"

int get_para(char *uid, char *des, char *mid, int *part);
int show_part(const char *uid, const char *des, const char *mid, int part);
int show_body(char *body, long body_len, 
	      const char *uid, const char *des, const char *mid, int part);

int cgiMain() {
  char uid[MAXID+1],des[40],mid[40];
  int part;


  if (get_para(uid, des, mid, &part)==0)
    {
      if (show_part(uid, des, mid, part)!=-1) return 0;
    }
  

  fprintf(cgiOut, "HTTP/1.1 200 OK\n");
  fprintf(cgiOut, "Server: Apache 1.3.9\n");
  fprintf(cgiOut, "Content-Type: text/html\n\n");

  fprintf(cgiOut, "<html><head>\n");
  fprintf(cgiOut,"<meta http-equiv=Content-Type "
	  "content=\"text/html; charset=gb2312\">\n");

  fprintf(cgiOut, "<body bgcolor=#99c8ff text=#0033cc>\n");

  fprintf(cgiOut, "<p align=center>¡¡</p>\n");
  fprintf(cgiOut, "<p align=center><font size=6><b>"
	  M_SYSINFO"</b></font></p>\n");

  fprintf(cgiOut, "<p align=center>¡¡</p>\n");
  fprintf(cgiOut, "<p align=center>¡¡</p>\n");

  fprintf(cgiOut, "<p align=center><font color=#FF0000>"
	  "%s</font></p>\n", g_info);
  
  fprintf(cgiOut, "<p align=center>¡¡</p>\n"); 

  fprintf(cgiOut, "</body></html>\n");

  return 0;
}


/*---------------------------------------
 get_para(): 
 para:   
   r_uid   retrive uid
   r_des   retrive md5_id
 ret:                               
   0       success                     
   -1      fail
----------------------------------------*/
int get_para(char *r_uid, char *r_des, char *r_mid, int *r_part)
{
  char uid[MAXID+1],des[40],mid[40],temp[50];
  int result;


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

  result=cgiFormStringNoNewlines("mid", mid, 35);
  if (result!=cgiFormSuccess)
    {
      warn(W_CGI_ERR);
      return -1;
    }

  result=cgiFormStringNoNewlines("part", temp, 20);
  if (result!=cgiFormSuccess)
    {
      warn(W_CGI_ERR);
      return -1;
    }
  *r_part=atoi(temp);
  

  if (db_flash_active(uid, des)==-1)
    {
      warn(W_USER_FOOL);
      return -1;
    }

  if (set_proc_uid(uid)==-1)
    {
      warn(W_SYS_ERR);
      return -1;
    }

  strcpy(r_uid, uid);
  strcpy(r_des, des);
  strcpy(r_mid, mid);


  return 0;
}


/*----------------------------------------+
  show_part():
  ret:
    0   succ
    -1  fail
+----------------------------------------*/
int show_part(const char *uid, const char *des, const char *mid, int part)
{
  char temp[300], folder[30];
  long ttt, t_bufsize, real_size;
  char *t_buf, *ptr;
  
  
  sprintf(temp, "select folder_n,sender,recv,tm,unread,title,size"
	  " from mail where uid='%s' and mail_id='%s'", uid, mid);
  if (db_exec(temp, 1)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }

  strcpy(folder, g_buf[0][0]);

  if (strcmp(folder, INBOX_NAME)==0)
    {
      
      ttt=atol(g_buf[0][6])+1024;
      t_bufsize=ttt;
      t_buf=(char *)malloc(ttt);
      if (t_buf==NULL)
	{
	  warn(W_NO_MEM);
	  return -1;
	}


      real_size=get_mail_by_POP3(uid, mid, t_buf, ttt, 0);
      if (real_size==-1)
	{
	  free(t_buf);
	  return -1;
	}

    }
  else
    {
      ttt=atol(g_buf[0][6])+1024;
      t_bufsize=ttt;
      t_buf=(char *)malloc(ttt);
      if (t_buf==NULL)
	{
	  warn(W_NO_MEM);
	  return -1;
	}


      if (db_get_content(uid, mid, t_buf)==-1)
	{
	  warn(W_SYS_ERR);
	  return -1;
	}

    }

  /*
  ptr=strstr(t_buf, "\r\n\r\n");
  if (ptr==NULL)
    {
      warn("ÓÊ¼þÓÐ´íÎó£¡");
      free(t_buf);
      return -1;
    }
  */  
  
  if (show_body(t_buf, t_bufsize, uid, des, mid, part)==-1) return -1;
  
  free(t_buf);

  return 0;
}


/*--------------------------------------------------
  show_body()
---------------------------------------------------*/
int show_body(char *body, long body_len,
	      const char *uid, const char *des, const char *mid, int part)
{
  char *p, *bg, *ed;
  char boundary[200];
  long ttt;
  int all_e, i, h_text, h_attach;
  char *p1, *p2, *p3, *p4, *ptt1, *ptt2;
  time_t tim;

  all_e=prepare_entry(body, body_len);

  //  prepare_order(all_e, &h_text, &h_attach);


  fprintf(cgiOut, "HTTP/1.0 200 OK\n");
  fprintf(cgiOut, "Server: Apache 1.3.9\n");
  fprintf(cgiOut, "Content-Type: application/octet-stream; "
  	  "name=\"%s\"\n", mime_e[part].content_x);
  fprintf(cgiOut, "Content-Disposition: inline; "
  	  "filename=\"%s\"\n\n", mime_e[part].content_x);


  p1=mime_e[part].bg;
  p2=p1+mime_e[part].len;
  *p2='\0';

  p3=p1;

  p4=strstr(p3, "\r\n.\r\n");
  if (p4==NULL) p4=p2;
  else *p4='\0';


  if (mime_e[part].transfer_encoding==CODE_7BIT)
    {
      fprintf(cgiOut, p3);
    }
  else if (mime_e[part].transfer_encoding==CODE_B64)
    {
      char *p; 
      long l, ll;

      p=(char *)malloc(mime_e[part].len+1024);
      if (p==NULL)
	{
	  warn("no enough memory!");
	  return -1;
	}

      ll=giveup_spc(p3, D_SPC_AND_CRLF);

      l=b64_decode(p3, ll, p);

      fwrite(p, 1, l, cgiOut);
      free(p);
    }
  else
    {
      ptt1=p3;
	  
      while(ptt1<p4)
	{
	  unsigned int lll, ll;
	  ptt2=strchr(ptt1,'\n');
	  if (ptt2!=NULL) *ptt2='\0';

	  ll=giveup_spc(ptt1, D_CRLF);

	  lll=strlen(ptt1);
	  if (ptt1[lll-1]=='=') { ptt1[lll-1]='\0'; ll--; }
	      
	  qp_decode(ptt1, ll, sock_buf);
	  fprintf(cgiOut, "%s", sock_buf);
	      
	  if (ptt2==NULL) break;
	  
	  ptt1=ptt2+1;
	      
	}
    }
  
  return 0;
  
}

