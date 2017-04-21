/* forget.c */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "cgic.h"
#include "db.h"
#include "lang.h"
#include "html.h"

char g_debug[100];

int check_regs();

int cgiMain() {
  char uid[MAXID+1],des[40];
  int ret;


  cgiHeaderContentType("text/html");

  if ((ret=check_regs())==-1)
    {

      fprintf(cgiOut, "<html><head>\n");
      fprintf(cgiOut,"<meta http-equiv=Content-Type "
	      "content=\"text/html; charset=gb2312\">\n");
      fprintf(cgiOut, "<title>move</title></head>\n");
      fprintf(cgiOut, "<body bgcolor=#FFFFFF text=#000000>\n");

      fprintf(cgiOut, "<title>Forget Password</title></head>\n");

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
 check_regs(): check user's login                    
 para:   none
 ret:                               
   0       success                     
   -1      fail

----------------------------------------*/
int check_regs()
{
  char uid[MAXID+1], *cp, pass[30], ques[201];
  int result, re;
  time_t tt;
  char des[40], temp[300];
  char *p, *html_file, *template;

  uid[0]=ques[0]='\0';

  result=cgiFormStringNoNewlines("uid", uid, MAXUID+1);
  if (result!=cgiFormSuccess && result!=cgiFormEmpty)
    {
      strcpy(g_info, W_CGI_ERR);
      return -1;
    }
  if (result==cgiFormEmpty)
    {
      warn(W_NO_NAME);
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
 

  for(cp=uid; *cp; cp++) { *cp=tolower(*cp); }

  /* generate id and insert into active list*/
  time(&tt);
  my_md5(uid, tt, des); 
  if (db_insert_active(uid, tt, des)!=0) return -1; 

  sprintf(temp, "select ques from user where uid='%s'", uid);
  if (db_exec(temp, 1)==-1)
    {
      warn(W_SYS_ERR);
      return -1;
    }
  strcpy(ques, g_buf[0][0]);

  html_file=html_Load(TEMPLATE_PATH, "/forget.html");

  html_File(html_file, "", "<%$bg$%>");

  template=html_GetTemplate(html_file, "<%$bg$%>", "<%$ed$%>");

  html_FillTemplate(template,
			"uid", uid,
			"des", des,
			"ques", ques,
			NULL);
  free(template);

  html_File(html_file, "<%$ed$%>", "");  

  free(html_file);

  return 0;
}


