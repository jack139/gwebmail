/* info.c */

#include <stdio.h>
#include <time.h>
#include "cgic.h"
#include "db.h"
#include "lang.h"
#include "html.h"

int info();

int cgiMain() {

  cgiHeaderContentType("text/html");

  if (info()==-1)
    {
      fprintf(cgiOut, "<html><head>\n");
      fprintf(cgiOut,"<meta http-equiv=Content-Type "
        	  "content=\"text/html; charset=gb2312\">\n");
      fprintf(cgiOut, "<title>outbox</title></head>\n");

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
  char temp[300];
  char *html_file, *template;

  
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

  sprintf(temp, "select name,pwd,addr,ques,ans from user"
	  " where uid='%s'", uid);
  if (db_exec(temp, 1)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }

  html_file=html_Load(TEMPLATE_PATH, "/info.html");

  html_File(html_file, "", "<%$bg$%>");

  template=html_GetTemplate(html_file, "<%$bg$%>", "<%$ed$%>");

  sprintf(temp, "%s@%s", get_user_name(uid), get_domain_name(uid));
  html_FillTemplate(template,
			"uid", uid,
			"des", des,
			"addr", temp,
			"name", g_buf[0][0],
			"pwd", g_buf[0][1],
			"pwd2", g_buf[0][1],
			"addr2", g_buf[0][2],
			"ques", g_buf[0][3],
			"ans", g_buf[0][4],
			NULL);
  free(template);
   
  html_File(html_file, "<%$ed$%>", "");

  free(html_file);

  return 0;
}







