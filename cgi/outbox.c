/* outbox.c */

#include <stdio.h>
#include <time.h>
#include "cgic.h"
#include "db.h"
#include "lang.h"
#include "html.h"

int outbox();

int cgiMain() {

  cgiHeaderContentType("text/html");

  if (outbox()==-1)
    {
      fprintf(cgiOut, "<html><head>\n");
      fprintf(cgiOut,"<meta http-equiv=Content-Type "
	      "content=\"text/html; charset=gb2312\">\n");
      fprintf(cgiOut, "<title>outbox</title></head>\n");

      fprintf(cgiOut, "<body bgcolor=#99c8ff text=#0033cc>\n");

      fprintf(cgiOut, "<p align=center>&nbsp;</p>\n");
      fprintf(cgiOut, "<p align=center><font size=6><b>"
	      M_SYSINFO"</b></font></p>\n");

      fprintf(cgiOut, "<p align=center>&nbsp;</p>\n");
      fprintf(cgiOut, "<p align=center>&nbsp;</p>\n");

      fprintf(cgiOut, "<p align=center><font color=#FF0000>"
	      "%s</font></p>\n", g_info);

      fprintf(cgiOut, "<p align=center>&nbsp;</p>\n"); 

      fprintf(cgiOut, "</body></html>\n");
    }

  return 0;
}


/*---------------------------------------
 outbox(): outbox                 
 ret:                               
   0       success                     
   -1      fail
/----------------------------------------*/
int outbox()
{
  char uid[MAXID+1],des[40],mid[40],way[10];
  int result;
  char t_recv[110],t_subject[210],temp[300];
  char *html_file, *template;
  char t1[200], t2[200];

 
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

  result=cgiFormStringNoNewlines("way", way, 10);
  if (result!=cgiFormSuccess)
    {
      warn(W_CGI_ERR);
      return -1;
    }

  switch(way[0])
    {
    case 'r':
    case 'f':
      result=cgiFormStringNoNewlines("mid", mid, 40);
      if (result!=cgiFormSuccess)
	{
	  warn(W_CGI_ERR);
	  return -1;
	}

      sprintf(temp, "select sender, title from mail"
	      " where uid='%s' and mail_id='%s'", uid, mid);
      if (db_exec(temp, 1)==-1)
	{
	  warn(W_DB_ERR);
	  return -1;
	}

      if (way[0]=='r')
	{
	  strcpy(t_recv, g_buf[0][0]);
	  sprintf(t_subject, "Re: %s", g_buf[0][1]);
	}
      else
	{
	  t_recv[0]='\0';
	  sprintf(t_subject, "Fw: %s", g_buf[0][1]);
	}
      break;

    case 'n':
      strcpy(t_recv, "");
      strcpy(t_subject, "");
      break;

    default:
      warn(W_SYS_ERR);
      return -1;
    }


  html_file = html_Load(TEMPLATE_PATH, "/outbox.html");

  html_File(html_file, "", "<%$bg0$%>");

  template = html_GetTemplate(html_file, "<%$bg0$%>", "<%$ed0$%>");
  sprintf(t1, "%s/send", CGI_BIN);
  sprintf(t2, "%s@%s", get_user_name(uid), get_domain_name(uid));
  html_FillTemplate(template, 
		      "act", t1, 
		      "uid", uid,
		      "des", des, 
		      "addr", t2,
		      NULL);
  free(template);

  html_File(html_file, "<%$ed0$%>", "<%$bg1$%>");

  template = html_GetTemplate(html_file, "<%$bg1$%>", "<%$ed1$%>");
  html_FillTemplate(template, 
		      "from", t2,
		      "rcpt", t_recv,
		      "title", t_subject,
		      NULL);
  free(template);

  html_File(html_file, "<%$ed1$%>", "");

  free(html_file);

  return 0;
}






