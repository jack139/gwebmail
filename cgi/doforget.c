/* doforget.c */

#include <stdio.h>
#include <time.h>
#include "cgic.h"
#include "db.h"
#include "lang.h"

int regform();
int cgi_encode(char *s);

int cgiMain() {
  char uid[MAXID+1],des[40];


  if (regform()==-1)
    {
      cgiHeaderContentType("text/html");
      fprintf(cgiOut, "<html><head>\n");
      fprintf(cgiOut,"<meta http-equiv=Content-Type "
	      "content=\"text/html; charset=gb2312\">\n");
      fprintf(cgiOut, "<title>"M_REGINFO"</title></head>\n");

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
 regform(): 
 ret:                               
   0       success                     
   -1      fail
----------------------------------------*/
int regform()
{
  char uid[MAXID+1], des[40], pwd[30], ans[201];
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

  ans[0]='\0';
  result=cgiFormStringNoNewlines("ans", ans, 200);
  if (result!=cgiFormSuccess && result!=cgiFormEmpty)
    {
      warn(W_CGI_ERR);
      return -1;
    }

  sprintf(temp, "select pwd,ans from user where uid='%s'", uid);
  if (db_exec(temp, 1)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }

  strcpy(pwd, g_buf[0][0]);
  cgi_encode(pwd);

  if (strcmp(ans, g_buf[0][1])!=0)
    {
      warn(W_ANS_ERR);
      return -1;
    }

  if (db_logout(uid, des)==-1) return -1;

  cgiHeaderContentType("text/html");
  fprintf(cgiOut, "<html><head>\n");
  fprintf(cgiOut, "<meta HTTP-EQUIV=refresh "
	  "CONTENT=\"0; URL=/cgi-bin/login?uid=%s&domain=%s"
	  "&pwd=%s\">\n", get_user_name(uid), get_domain_name(uid), pwd);
  fprintf(cgiOut, "<meta http-equiv=Content-Type "
	  "content=\"text/html; charset=gb2312\">\n");
  fprintf(cgiOut, "<title></title></head><body></body></html>\n");

  return 0;
}


int cgi_encode(char *s)
{
  char tmp[200];
  int i,j;

  strcpy(tmp, s);
  s[0]='\0';

  for(i=0,j=0;tmp[i];i++)
    {
      if (tmp[i]==' ') s[j++]='+';
      else if (isalnum(tmp[i])) s[j++]=tmp[i];
      else
	{
	  sprintf(s+j, "%%%02x", tmp[i]);
	  j=strlen(s);
	}
    }

  return 0;
}

