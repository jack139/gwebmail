/* doreg.c */

#include <stdio.h>
#include <time.h>
#include "cgic.h"
#include "db.h"
#include "mail_sub.h"
#include "lang.h"

int doreg();

int cgiMain() {
  char uid[MAXID+1],des[40];


  cgiHeaderContentType("text/html");
  fprintf(cgiOut, "<html><head>\n");
  fprintf(cgiOut,"<meta http-equiv=Content-Type "
	  "content=\"text/html; charset=gb2312\">\n");
  fprintf(cgiOut, "<title>"M_REG"</title></head>\n");


  if (doreg()==-1)
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

      fprintf(cgiOut, "<p align=center><a href=/cgi-bin/register0>"
	      M_REGNEW"</a></p>\n");

      fprintf(cgiOut, "</body></html>\n");
    }

  return 0;
}


/*---------------------------------------
 doreg(): 
 ret:                               
   0       success                     
   -1      fail
----------------------------------------*/
int doreg()
{
  char uid[MAXID+1],des[40];
  int result;
  char temp[500];
  char name[30], pwd[30], cpwd[100], addr[250], ques[201], ans[201];
  int uuid;
  char *p;

  
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


  for(p=uid; *p; p++){*p=tolower(*p);}

  result=cgiFormStringNoNewlines("usr_n", name, 30);
  if (result!=cgiFormSuccess && result!=cgiFormEmpty)
    {
      warn(W_CGI_ERR);
      return -1;
    }

  result=cgiFormStringNoNewlines("pwd", pwd, 30);
  if (result!=cgiFormSuccess && result!=cgiFormEmpty)
    {
      warn(W_CGI_ERR);
      return -1;
    }

  if (result==cgiFormEmpty)
    {
      db_logout(uid, des);
      warn(W_NO_PASS);

      return -1;
    }



  result=cgiFormStringNoNewlines("confirm_pwd", cpwd, 30);
  if (result!=cgiFormSuccess && result!=cgiFormEmpty)
    {
      warn(W_CGI_ERR);
      return -1;
    }

  result=cgiFormStringNoNewlines("addr", addr, 210);
  if (result!=cgiFormSuccess && result!=cgiFormEmpty)
    {
      warn(W_CGI_ERR);
      return -1;
    }

  db_prepare(addr);

  if (strcmp(pwd, cpwd)!=0)
    {
      db_logout(uid, des);
      warn(W_PASS_ERR2);
      
      return -1;
    }

  ques[0]=ans[0]='\0';

  result=cgiFormStringNoNewlines("ques", ques, 200);
  if (result!=cgiFormSuccess && result!=cgiFormEmpty)
    {
      warn(W_CGI_ERR);
      return -1;
    }
  db_prepare(ques);

  if (result==cgiFormEmpty)
    {
      db_logout(uid, des);
      warn(W_NO_QUES);

      return -1;
    }


  result=cgiFormStringNoNewlines("ans", ans, 200);
  if (result!=cgiFormSuccess && result!=cgiFormEmpty)
    {
      warn(W_CGI_ERR);
      return -1;
    }
  db_prepare(ans);

  if (result==cgiFormEmpty)
    {
      db_logout(uid, des);
      warn(W_NO_ANS);

      return -1;
    }

  sprintf(temp, "insert into user (uid, name, pwd, addr, ques, ans) "
	  "values ('%s','%s','%s','%s','%s','%s')", 
	  uid, name, cpwd, addr, ques, ans);
  if (db_exec(temp, 0)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }

  sprintf(temp, "insert into domainalias (address, alias) "
	  "values ('%s@%s','%s')",
	  get_user_name(uid), get_domain_name(uid), uid);
  if (db_exec(temp, 0)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }

  sprintf(temp, "insert into folder (uid, folder_n)"
	  "values ('%s', '"INBOX_NAME"')", uid);
  if (db_exec(temp, 0)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }

  sprintf(temp, "insert into folder (uid, folder_n)"
	  "values ('%s', '"SENT_NAME"')", uid);
  if (db_exec(temp, 0)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }

  sprintf(temp, "insert into folder (uid, folder_n)"
	  "values ('%s', '"TRASH_NAME"')",uid);
  if (db_exec(temp, 0)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }

  if (db_logout(uid, des)==-1) return -1;

  fprintf(cgiOut, "<body bgcolor=#99c8ff text=#0033cc>\n");

  fprintf(cgiOut, "<p align=center>　</p>\n");
  fprintf(cgiOut, "<p align=center>　</p>\n");
  fprintf(cgiOut, "<p align=center>　</p>\n");

  fprintf(cgiOut,"<p align=center><b>"M_REGOK"</b></p>\n");

  fprintf(cgiOut, "<p align=center>　</p>\n"); 

  fprintf(cgiOut,"<p align=center><a href=/cgi-bin/login0 "
	  "target=_self>"M_LOGNOW"</a></p>\n");

  fprintf(cgiOut, "</form>\n");


  return 0;
}






