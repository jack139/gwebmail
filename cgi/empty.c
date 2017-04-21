/* empty.c */

#include <stdio.h>
#include <time.h>
#include "cgic.h"
#include "db.h"
#include "mail_sub.h"
#include "lang.h"

int empty();

int cgiMain() {

  cgiHeaderContentType("text/html");

  if (empty()==-1)
    {
      fprintf(cgiOut, "<html><head>\n");
      fprintf(cgiOut,"<meta http-equiv=Content-Type "
	      "content=\"text/html; charset=gb2312\">\n");
      fprintf(cgiOut, "<title>empty</title></head>\n");
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
 empty():  
 ret:                               
   0       success                     
   -1      fail
----------------------------------------*/
int empty()
{
  char uid[MAXID+1],des[40];
  int result;
  char sql_cmd[200];
  
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

  sprintf(sql_cmd,"update folder set mails=0, unread=0, size=0"
	  " where uid='%s' and folder_n='"TRASH_NAME"'", uid);
  if (db_exec(sql_cmd, 0)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }

  sprintf(sql_cmd, "delete from mail where "
	  "uid='%s' and folder_n='"TRASH_NAME"'", uid);
  if (db_exec(sql_cmd, 0)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }
  

  fprintf(cgiOut, "<html><head>\n");
  fprintf(cgiOut, "<meta HTTP-EQUIV=refresh "
	  "CONTENT=\"0; URL=/cgi-bin/folder?box="TRASH_NAME
	  "&uid=%s&des=%s\">\n", uid, des);
  fprintf(cgiOut, "<meta http-equiv=Content-Type "
	  "content=\"text/html; charset=gb2312\">\n");
  fprintf(cgiOut, "<title></title></head><body></body></html>\n");

  return 0;
}


