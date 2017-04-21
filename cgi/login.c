/* login.c */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "cgic.h"
#include "db.h"
#include "lang.h"

char g_debug[100];

int check_login(char *uid, char *des);

int cgiMain() {
  char uid[MAXID+1],des[40];
  int ret;


  cgiHeaderContentType("text/html");
  fprintf(cgiOut, "<html><head>\n");
  fprintf(cgiOut,"<meta http-equiv=Content-Type "
	  "content=\"text/html; charset=gb2312\">\n");

  if ((ret=check_login(uid,des))==0)
    {
      fprintf(cgiOut, "<title>"M_TITLE"</title></head>\n");
      fprintf(cgiOut, "<frameset FRAMEBORDER=0 FRAMESPACING=0 rows=60,*>\n");

      fprintf(cgiOut, "<frame scrolling=no noresize src=/mailhead.html>\n");

      fprintf(cgiOut, "<frameset FRAMEBORDER=0 FRAMESPACING=1 cols=105,*>\n");
      fprintf(cgiOut, "<frame noresize "
	      "src=/cgi-bin/listmenu?uid=%s&des=%s>\n",uid,des);
      fprintf(cgiOut, "<frame name=mainwindow "
	      "src=/cgi-bin/folderlist?uid=%s&des=%s>\n",uid,des);
      fprintf(cgiOut, "</frameset>\n");

      fprintf(cgiOut, "</frameset>\n");

      fprintf(cgiOut, "</html>\n");

    }
  else
    {
      fprintf(cgiOut, "<title>"M_LOGFAIL"</title></head>\n");
      fprintf(cgiOut, "<body bgcolor=#99c8ff text=#0033cc>\n");

      fprintf(cgiOut, "<p align=center>　</p>\n");
      fprintf(cgiOut, "<p align=center><font size=6><b>"
	      M_SYSINFO"</b></font></p>\n");

      fprintf(cgiOut, "<p align=center>　</p>\n");
      fprintf(cgiOut, "<p align=center>　</p>\n");

      fprintf(cgiOut, "<p align=center><font color=#FF0000>"
	      "%s</font></p>\n", g_info);

      fprintf(cgiOut, "<p align=center>　</p>\n");

      fprintf(cgiOut, "<p align=center><a href=/cgi-bin/login0>"
	      M_RELOG"</a></p>"); 
      fprintf(cgiOut, "</body></html>\n");

    }

  return 0;

}

/*---------------------------------------
 check_login(): check user's login                    
 para:   none
 ret:                               
   0       success                     
   -1      fail

----------------------------------------*/
int check_login(char *r_uid, char *r_des)
{
  char uid[MAXID+1], pwd[50], *cp, temp[50];
  int result, re;
  time_t tt;
  char des[40];


  result=cgiFormStringNoNewlines("uid", uid, MAXUID+1);
  if (result!=cgiFormSuccess && result!=cgiFormEmpty)
    {
      strcpy(g_info, W_CGI_ERR);
      return -1;
    }

  result=cgiFormStringNoNewlines("domain", temp, MAXCID+1);
  if (result!=cgiFormSuccess && result!=cgiFormEmpty)
    {
      strcpy(g_info, W_CGI_ERR);
      return -1;
    }
  strcat(uid, ".");
  strcat(uid, temp);

  result=cgiFormStringNoNewlines("pwd", pwd, 21);
  sprintf(g_debug, "(%s) (%s)", uid, pwd);
  if (result!=cgiFormSuccess && result!=cgiFormEmpty)
    {
      strcpy(g_info, W_CGI_ERR);
      return -1;
    }

  /*
  strcpy(uid, "gt");
  strcpy(pwd, "");
  */

  for(cp=uid; *cp; cp++) { *cp=tolower(*cp); }

  /* check login from db */
  if (db_login(uid,pwd)!=0) return -1;

  /* generate id and insert into active list*/
  time(&tt);
  my_md5(uid, tt, des); 
  if (db_insert_active(uid, tt, des)!=0) return -1; 

  strcpy(r_uid, uid);
  strcpy(r_des, des);

  return 0;
}


  






