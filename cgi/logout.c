/* logout.c */

#include <stdio.h>
#include <time.h>
#include "cgic.h"
#include "db.h"
#include "lang.h"

int logout(char *uid, char *des);

int cgiMain() {
  char uid[MAXID+1],des[40];


  cgiHeaderContentType("text/html");

  if (logout(uid, des)==0)
    {

      fprintf(cgiOut, "<html>\n");
      fprintf(cgiOut, "<head>");
      fprintf(cgiOut, "<meta HTTP-EQUIV=refresh "
	      "CONTENT=\"0; URL=/cgi-bin/login0\">\n");
      fprintf(cgiOut, "<meta http-equiv=Content-Type "
	      "content=\"text/html; charset=gb2312\">\n");
      fprintf(cgiOut, "<title></title></head>\n");
      fprintf(cgiOut, "<body></body>");
      fprintf(cgiOut, "</html>\n");
    }
  else
    {
      fprintf(cgiOut, "<html><head>\n");
      fprintf(cgiOut,"<meta http-equiv=Content-Type "
	      "content=\"text/html; charset=gb2312\">\n");
      fprintf(cgiOut, "<title>"M_LOGOUT"</title></head>\n");

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
 logout(): logout user                    
 para:   
   r_uid   retrive uid
   r_des   retrive md5_id
 ret:                               
   0       success                     
   -1      fail
----------------------------------------*/
int logout(char *r_uid, char *r_des)
{
  char uid[MAXID+1],des[40];
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

  if (db_logout(uid,des)==-1) return -1;

  return 0;
}






