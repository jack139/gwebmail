/* showmail.c */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "cgic.h"
#include "db.h"
#include "lang.h"

int get_para(char *uid, char *des, char *mid);

int cgiMain() {
  char uid[MAXID+1],des[40],mid[40];


  cgiHeaderContentType("text/html");

  fprintf(cgiOut, "<html><head>\n");
  fprintf(cgiOut,"<meta http-equiv=Content-Type "
	  "content=\"text/html; charset=gb2312\">\n");

  if (get_para(uid, des, mid)==0)
    {
      fprintf(cgiOut, "<title>show mail</title></head>\n");
      fprintf(cgiOut, "<frameset FRAMEBORDER=0 FRAMESPACING=0 rows=30%,*>\n");
      fprintf(cgiOut, "<frame noresize "
              "src=/cgi-bin/mailbar?uid=%s&des=%s&mid=%s>\n",uid,des,mid);
      fprintf(cgiOut, "<frame "
              "src=/cgi-bin/mail?uid=%s&des=%s&mid=%s>\n",uid,des,mid);
      fprintf(cgiOut, "</frameset>\n");
      fprintf(cgiOut, "</html>\n");
    }
  else
    {
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


  strcpy(r_uid, uid);
  strcpy(r_des, des);
  strcpy(r_mid, mid);


  return 0;
}






