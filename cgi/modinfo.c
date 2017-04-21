/* modinfo.c */

#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include "cgic.h"
#include "db.h"
#include "lang.h"

#define FORWARD_PATH  "/var/forward/"

int info();
int change_forward(char *, int);

int cgiMain() {
  char uid[MAXID+1],des[40];


  cgiHeaderContentType("text/html");
  fprintf(cgiOut, "<html><head>\n");
  fprintf(cgiOut,"<meta http-equiv=Content-Type "
	  "content=\"text/html; charset=gb2312\">\n");
  fprintf(cgiOut, "<title>modify info</title></head>\n");


  if (info()==-1)
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
 info(): modify user's info                 
 ret:                               
   0       success                     
   -1      fail
----------------------------------------*/
int info()
{
  char uid[MAXID+1],des[40];
  int result;
  char temp[500];
  char name[30], pwd[30], cpwd[100], addr[250],
    ques[201], ans[201];
  
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
      warn(W_PASS_BL);
      return -1;
    }

  result=cgiFormStringNoNewlines("confirm_pwd", cpwd, 30);
  if (result!=cgiFormSuccess && result!=cgiFormEmpty)
    {
      warn(W_CGI_ERR);
      return -1;
    }

  if (result==cgiFormEmpty)
    {
      warn(W_PASS_BL);
      return -1;
    }

  result=cgiFormStringNoNewlines("addr", addr, 210);
  if (result!=cgiFormSuccess && result!=cgiFormEmpty)
    {
      warn(W_CGI_ERR);
      return -1;
    }

  db_prepare(addr);

  result=cgiFormStringNoNewlines("ques", ques, 200);
  if (result!=cgiFormSuccess && result!=cgiFormEmpty)
    {
      warn(W_CGI_ERR);
      return -1;
    }
  db_prepare(ques);
  if (result==cgiFormEmpty)
   {
      warn(W_NO_QUES2);
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
	warn(W_NO_ANS2);
	return -1;
    }

  if (strcmp(pwd, cpwd)!=0)
    {
      warn(W_PASS_ERR3);
      return -1;
    }

  /*
  if (change_forward(uid, atoi(sm))==-1)
    {
      warn("Set Forwarding Fail!");
      return -1;
    }
  */

  sprintf(temp, "update user set "
	  "name='%s',"
	  "pwd='%s',"
	  "addr='%s',"
	  "ques='%s',"
	  "ans='%s'"
	  " where uid='%s'", 
	  name, cpwd, addr, ques, ans, uid);
  if (db_exec(temp, 0)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }

  fprintf(cgiOut, "<body bgcolor=#99c8ff text=#0033cc>\n");

  fprintf(cgiOut, "<p align=center>　</p>\n");

  fprintf(cgiOut, "<p align=center>　</p>\n");
  fprintf(cgiOut, "<p align=center>　</p>\n");

  fprintf(cgiOut,"<p align=center>"M_INFO_OK"</p>\n");

  fprintf(cgiOut, "</body></html>\n");


  return 0;
}



/*--------------------------------------------------*/

int change_forward(char *uid, int sm)
{
  FILE *fp;
  char fn[100];

  sprintf(fn, FORWARD_PATH"%s", uid);

  if (sm==0)
    {
      if (access(fn, F_OK)!=0) return 0; // not exist
      unlink(fn);  // del the forward file
      return 0;
    }

  fp=fopen(fn, "w");
  if (fp==NULL) return -1;
  fprintf(fp, "%s@sm-gateway\n%s\n", uid, uid);
  fclose(fp);
  
  return 0;
}



