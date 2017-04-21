/* mail.c */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "cgic.h"
#include "db.h"
#include "mail_sub.h"
#include "mime.h"
#include "base64.h"
#include "lang.h"

int get_it_out(char *s, char *bg, char *ed);

int get_para(char *uid, char *des, char *mid);
int show_mail(const char *uid, const char *des, const char *mid);
int show_body(char *body, long body_len, 
	      const char *uid, const char *des, const char *mid);

int cgiMain() {
  char uid[MAXID+1],des[40],mid[40];


  cgiHeaderContentType("text/html");

  if (get_para(uid, des, mid)==0)
    {
      if (show_mail(uid, des, mid)==-1) goto err_out;
      return 0;
    }

 err_out:

  fprintf(cgiOut, "<html><head>\n");
  fprintf(cgiOut,"<meta http-equiv=Content-Type "
          "content=\"text/html; charset=gb2312\">\n");
  fprintf(cgiOut, "<title>mail</title></head>\n");

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
int get_para(char *r_uid, char *r_des, char *r_mid)
{
  char uid[MAXID+1],des[40],mid[40];
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

  if (db_flash_active(uid, des)==-1) return -1;

  if (set_proc_uid(uid)==-1) return -1;


  strcpy(r_uid, uid);
  strcpy(r_des, des);
  strcpy(r_mid, mid);


  return 0;
}


/*----------------------------------------+
  show_mail():
  ret:
    0   succ
    -1  fail
+----------------------------------------*/
int show_mail(const char *uid, const char *des, const char *mid)
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
	  warn(W_DB_ERR);
	  return -1;
	}
      
    }


  if (show_body(t_buf, t_bufsize, uid, des, mid)==-1) return -1;
  
  free(t_buf);


  if (db_update_unread(uid, folder, mid)==-1) return -1;
  
  return 0;
}


/*--------------------------------------------------
  show_body()
---------------------------------------------------*/
int show_body(char *body, long body_len,
	      const char *uid, const char *des, const char *mid)
{
  char *p, *bg, *ed;
  char boundary[200];
  long ttt;
  int all_e, i, h_text, h_attach;
  char *p1, *p2, *p3, *p4;

  
  all_e=prepare_entry(body, body_len);

  /*
  printf("all_e=%d<br>", all_e);
  for(i=0; i<all_e; i++)
    {
      printf("<br>%d ==================================<br>", i);
      printf("Content-Type=%02X<br>"
	     "Content-Transfer-Encoding=%02X<br>"
	     "Content-X=%s<br>"
	     "face=%d  papa=%d<br>"
	     "bg=%X len=%ld<br>",
	     mime_e[i].content_type,
	     mime_e[i].transfer_encoding,
	     mime_e[i].content_x,
	     mime_e[i].face,
	     mime_e[i].papa,
	     mime_e[i].bg,
	     mime_e[i].len);  
      printf("=====================================<br>");
    }
  */

  prepare_order(all_e, &h_text, &h_attach);

  /*
  printf("text: %d<br>", h_text);
  i=h_text;
  while(i!=-1)
    {
      printf("id=%d, next=%d<br>", i, mime_e[i].next);
      i=mime_e[i].next;
    }

  printf("attach: %d<br>", h_attach);
  i=h_attach;
  while(i!=-1)
    {
      printf("id=%d, next=%d<br>", i, mime_e[i].next);
      i=mime_e[i].next;
    }
  */

  if (h_text!=-1)
    {
      char *ptt1, *ptt2;
      
      p1=mime_e[h_text].bg;
      p2=p1+mime_e[h_text].len-1;
      *p2='\0';
      
      if (mime_e[h_text].content_type==TE_HTML)
	{
          p3=p1;

          p4=strstr(p3, "\r\n.\r\n");
          if (p4==NULL) p4=p2;
          else *p4='\0';

	  if (mime_e[h_text].transfer_encoding==CODE_7BIT)
	    {
	      get_it_out(p3, "</body", ">");
	      get_it_out(p3, "</BODY", ">");
	      get_it_out(p3, "</html", ">");
	      get_it_out(p3, "</HTML", ">");

	      fprintf(cgiOut, p3);
	    }
	  else
	    {
	      ptt1=p3;
	      
	      while(ptt1<p4)
		{
		  ptt2=strchr(ptt1,'\n');
		  if (ptt2!=NULL) *ptt2='\0';
		  if (mime_e[h_text].transfer_encoding==CODE_B64)
		    {
		      long ll;

		      ll=giveup_spc(ptt1, D_SPC_AND_CRLF);
		      b64_decode(ptt1, ll, sock_buf);

		      get_it_out(p3, "</body", ">");
		      get_it_out(p3, "</BODY", ">");
		      get_it_out(p3, "</html", ">");
		      get_it_out(p3, "</HTML", ">");

		      fprintf(cgiOut, "%s", sock_buf);
		    }
		  else
		    {
		      unsigned int lll,ll;

		      ll=giveup_spc(ptt1, D_CRLF);

		      lll=strlen(ptt1);
		      if (ptt1[lll-1]=='='){ ptt1[lll-1]='\0'; ll--; }

		      qp_decode(ptt1, ll, sock_buf);

		      get_it_out(p3, "</body", ">");
		      get_it_out(p3, "</BODY", ">");
		      get_it_out(p3, "</html", ">");
		      get_it_out(p3, "</HTML", ">");

		      fprintf(cgiOut, "%s", sock_buf);
		    }
		  
		  if (ptt2==NULL) break;

		  ptt1=ptt2+1;
		  
		}
	    }
	}
      else
	{

	  fprintf(cgiOut, "<html><head>\n");
	  fprintf(cgiOut,"<meta http-equiv=Content-Type "
		  "content=\"text/html; charset=gb2312\">\n");
	  fprintf(cgiOut, "<title>mail</title></head>\n");
	  fprintf(cgiOut, "<body bgcolor=#FFFFFF>\n");


	  p3=p1;
	  
	  p4=strstr(p3, "\r\n.\r\n");
	  if (p4==NULL) p4=p2;
	  else *p4='\0';

	  ptt1=p3;
	  
	  while(ptt1<p4)
	    {
	      ptt2=strchr(ptt1, '\n');
	      if (ptt2!=NULL) *ptt2='\0';

	      if (mime_e[h_text].transfer_encoding==CODE_B64)
		{
		  long ll;

		  ll=giveup_spc(ptt1, D_SPC_AND_CRLF);
		  b64_decode(ptt1, ll, sock_buf);
		  fprintf(cgiOut, "<p><font color=#000000>%s"
			  "</font></p>\n", sock_buf);
		}
	      else if (mime_e[h_text].transfer_encoding==CODE_QP)
		{
		  unsigned int lll,ll;

		  ll=giveup_spc(ptt1, D_CRLF);

		  lll=strlen(ptt1);
		  if (ptt1[lll-1]=='=') { ptt1[lll-1]='\0'; ll--; }

		  qp_decode(ptt1, ll, sock_buf);
		  fprintf(cgiOut, "<p><font color=#000000>%s"
                          "</font></p>\n", sock_buf);
		}
	      else
		{
		  fprintf(cgiOut, "<p><font color=#000000>%s"
                          "</font></p>\n", ptt1);
		}

	      if (ptt2==NULL) break;

	      ptt1=ptt2+1;
	    }
	  
	}

    }

  if (h_attach!=-1)
    {
      fprintf(cgiOut, "\n<div align=center><center>"
	      "<table border=0 width=100%%>\n");

      i=h_attach; 
      while(i!=-1)
	{
	  fprintf(cgiOut, "<tr bgcolor=#99c8ff>\n");
	  filt_subject(mime_e[i].content_x);
	  fprintf(cgiOut, "<td><a href=/cgi-bin/nph-part?uid=%s&des=%s&"
		  "mid=%s&part=%d><font color=#0033cc size=3>"
		  M_ATTACH": %s</font></a></td><tr>\n", 
		  uid, des, mid, i, mime_e[i].content_x);
	  i=mime_e[i].next;
	}
    }
  fprintf(cgiOut, "</table></center></div>\n");

  fprintf(cgiOut, "</body></html>");

  return 0;

  
}

int get_it_out(char *s, char *bg, char *ed)
{
  long tll;
  int ll;
  char *ppp;

  ppp=get_a_part(sock_buf, bg, ed, &tll);
  if (ppp)
    for(ll=0; ll<=tll; ll++) ppp[ll]=' ';

  return 0;
}






