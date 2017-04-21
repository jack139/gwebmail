/* mailbar.c */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "cgic.h"
#include "db.h"
#include "mail_sub.h"
#include "lang.h"
#include "html.h"

int get_para(char *uid, char *des, char *mid);

int cgiMain() {
  char uid[MAXID+1],des[40],mid[40];
  char *html_file, *template;
  char t1[200],t2[200],t3[200],t4[200];


  cgiHeaderContentType("text/html");


  if (get_para(uid, des, mid)==0)
    {
      char temp[300];

      sprintf(temp, "select folder_n,sender, recv, title, tm from mail"
	      " where uid='%s' and mail_id='%s'", uid, mid);
      if (db_exec(temp, 1)==-1)
	{
	  warn(W_DB_ERR);
	  goto err;
	}


      html_file = html_Load(TEMPLATE_PATH, "/mailbar.html");

      html_File(html_file, "", "<%$bg0$%>");

      template = html_GetTemplate(html_file, "<%$bg0$%>", "<%$ed0$%>");
      sprintf(t1, "%s/outbox?uid=%s&des=%s&mid=%s&way=r", 
		CGI_BIN, uid, des, mid);
      sprintf(t2, "%s/outbox?uid=%s&des=%s&mid=%s&way=f", 
		CGI_BIN, uid, des, mid);
      if (strcmp(g_buf[0][0], TRASH_NAME)!=0)
	sprintf(t3, "%s/move?uid=%s&des=%s&all=1&src=%s"
		"&check1=ON&mid1=%s&dest="TRASH_NAME, CGI_BIN, uid,
		des, g_buf[0][0], mid);
      else t3[0]='\0';
      sprintf(t4, "%s/folder?uid=%s&box=%s&des=%s", 
		CGI_BIN, uid, g_buf[0][0], des);
      if (strlen(g_buf[0][3])==0)
	strcpy(g_buf[0][3], M_NOSUBJ);
      html_FillTemplate(template, 
			  "box",  g_buf[0][0],
			  "act1", t1,
			  "act2", t2,
			  "act3", t3,
			  "act4", t4,
			  "from", g_buf[0][1],
			  "rcpt", g_buf[0][2],
			  "title", g_buf[0][3],
			  "date", g_buf[0][4],
			  NULL);
      free(template);

      html_File(html_file, "<%$ed0$%>", "");      
      
      return 0;
    }	

 err:

  fprintf(cgiOut, "<html><head>\n");
  fprintf(cgiOut,"<meta http-equiv=Content-Type "
	  "content=\"text/html; charset=gb2312\">\n");
  fprintf(cgiOut, "<title>mail</title></head>\n");

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
 get_para(): 
 para:   
   r_uid   retrive uid
   r_des   retrive md5_id
 ret:                               
   0       success                     
   -1      fail
----------------------------------------*/
int get_para(char *uid, char *des, char *mid)
{
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


  return 0;
}



