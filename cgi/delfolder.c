/* delfoler.c */

#include <stdio.h>
#include <time.h>
#include "cgic.h"
#include "db.h"
#include "mail_sub.h"
#include "lang.h"

int dofolder();

int cgiMain() {
  char uid[MAXID+1],des[40];


  if (dofolder()==-1)
    {
      cgiHeaderContentType("text/html");
      fprintf(cgiOut, "<html><head>\n");
      fprintf(cgiOut,"<meta http-equiv=Content-Type "
	      "content=\"text/html; charset=gb2312\">\n");
      fprintf(cgiOut, "<title>Del Folder</title></head>\n");

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
 dofolder(): 
 ret:                               
   0       success                     
   -1      fail
----------------------------------------*/
int dofolder()
{
  char uid[MAXID+1],des[40],sid[30], folder_n[30];
  int result;
  char temp[300];

  
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


  result=cgiFormStringNoNewlines("dfolder", folder_n, 30);
  if (result!=cgiFormSuccess && result!=cgiFormEmpty)
    {
      warn(W_CGI_ERR);
      return -1;
    }

  if (result==cgiFormEmpty)
    {
      warn(W_CGI_ERR);
      return -1;
    }

  if (db_if_hz_string(folder_n)==0)
    {
      warn(W_FLNA_ERR4);
      return -1;
    }


  if (strcmp(folder_n, INBOX_NAME)==0) goto okk;
  if (strcmp(folder_n, SENT_NAME)==0) goto okk;
  if (strcmp(folder_n, TRASH_NAME)==0) goto okk;


  sprintf(temp, "delete from folder"
	  " where uid='%s' and folder_n='%s'", uid, folder_n);
  if (db_exec(temp, 0)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }

  sprintf(temp, "delete from mail"
	  " where uid='%s' and folder_n='%s'", uid, folder_n);
  if (db_exec(temp, 0)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }

 okk:
  cgiHeaderContentType("text/html");
  fprintf(cgiOut, "<html><head>\n");
  fprintf(cgiOut, "<meta HTTP-EQUIV=refresh "
	  "CONTENT=\"0; URL=/cgi-bin/folderlist?uid=%s"
	  "&des=%s\">\n", uid, des);
  fprintf(cgiOut, "<meta http-equiv=Content-Type "
	  "content=\"text/html; charset=gb2312\">\n");
  fprintf(cgiOut, "<title></title></head><body></body></html>\n");

  return 0;
}


