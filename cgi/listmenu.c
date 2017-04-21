/* listmenu.c */

#include <stdio.h>
#include <time.h>
#include "cgic.h"
#include "db.h"
#include "mail_sub.h"
#include "lang.h"
#include "html.h"


int check_active(char *uid, char *des);

int cgiMain() {
  char uid[MAXID+1],des[40];
  char *html_file, *template;
  char t1[200];
  int re, i;

  cgiHeaderContentType("text/html");

  if (check_active(uid, des)==0)
    {
/*
      sprintf(t1, "select folder_n from folder"
	      " where uid='%s' order by id", uid);
      re=db_exec(t1, 1);
      if (re==-1)
	{
	  warn(W_DB_ERR);
	  goto fail;
	}
*/

      html_file = html_Load(TEMPLATE_PATH, "/listmenu.html");

      html_File(html_file, "", "<%$bg0$%>");

      template = html_GetTemplate(html_file, "<%$bg0$%>", "<%$ed0$%>");

      sprintf(t1,"%s/folder?uid=%s&box="INBOX_NAME"&des=%s",CGI_BIN, uid, des);
      html_FillTemplate(template, "fcg", t1, 
		"target", "mainwindow", "name", M_INBOX, NULL);

      sprintf(t1,"%s/outbox?uid=%s&des=%s&way=n",CGI_BIN, uid, des);
      html_FillTemplate(template, "fcg", t1, 
		"target", "mainwindow", "name", M_SENDMAIL, NULL);

      sprintf(t1,"%s/folder?uid=%s&box="SENT_NAME"&des=%s",CGI_BIN, uid, des);
      html_FillTemplate(template, "fcg", t1, 
		"target", "mainwindow", "name", M_OUTBOX, NULL);
      
      sprintf(t1,"%s/folder?uid=%s&box="TRASH_NAME"&des=%s",CGI_BIN, uid, des);
      html_FillTemplate(template, "fcg", t1, 
		"target", "mainwindow", "name", M_DEL, NULL);

      sprintf(t1,"%s/folderlist?uid=%s&des=%s",CGI_BIN, uid, des);
      html_FillTemplate(template, "fcg", t1, 
		"target", "mainwindow", "name", M_FOLDER1, NULL);

      sprintf(t1,"%s/info?uid=%s&des=%s",CGI_BIN, uid, des);
      html_FillTemplate(template, "fcg", t1, 
		"target", "mainwindow", "name", M_SET, NULL);

      sprintf(t1,"/help.html");
      html_FillTemplate(template, "fcg", t1, 
		"target", "mainwindow", "name", M_HELP, NULL);

      sprintf(t1,"%s/logout?uid=%s&des=%s",CGI_BIN, uid, des);
      html_FillTemplate(template, "fcg", t1, 
		"target", "_top", "name", M_LOGOUT, NULL);
/*
      for (i=0; i<re; i++)
	{
	  if (!strcmp(g_buf[i][0], INBOX_NAME)) continue;
	  if (!strcmp(g_buf[i][0], SENT_NAME)) continue;
	  if (!strcmp(g_buf[i][0], TRASH_NAME)) continue;

	  sprintf(t1,"%s/folderuid=%s&?box=%s&des=%s",
		CGI_BIN, uid, g_buf[i][0], des);
	  html_FillTemplate(template, "fcg", t1, "name", g_buf[i][0], NULL);
	}
*/
      free(template);

      html_File(html_file, "<%$ed0$%>", "");

      free(html_file);

      return 0;
    }

 fail:
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
 check_active(): check user in active list                    
 para:   
   r_uid   retrive uid
   r_des   retrive md5_id
 ret:                               
   0       success                     
   1       cgi error                   
   2       login fail                 
----------------------------------------*/
int check_active(char *uid, char *des)
{
  int result;
  time_t tt;
  
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

  /* check active list in db */
  if (db_flash_active(uid,des)!=0) return -1;

  return 0;
}



