/* login0.c*/

#include <stdio.h>
#include <time.h>
#include "cgic.h"
#include "db.h"
#include "lang.h"
#include "html.h"

int login0();

int cgiMain() {

  cgiHeaderContentType("text/html");

  if (login0()==-1)
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
 ret:                               
   0       success                     
   -1      fail
----------------------------------------*/
int login0()
{
  int re, i;
  char temp[300];
  char *html_file, *template;

  
  sprintf(temp, "select domain from domains");
  if ((re=db_exec(temp, 1))==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }

  if (re==0) { warn("No data in table domains"); return -1; }

  html_file=html_Load(TEMPLATE_PATH, "/login.html");

  html_File(html_file, "", "<%$bg$%>");

  template=html_GetTemplate(html_file, "<%$bg$%>", "<%$ed$%>");

  for(i=0; i<re; i++)
  {
    if (i==0)
      html_FillTemplate(template,
			"domain1", g_buf[i][0],
			"sel", "selected",
			"domain2", g_buf[i][0],
			NULL);
    else
      html_FillTemplate(template,
			"domain1", g_buf[i][0],
			"sel", "",
			"domain2", g_buf[i][0],
			NULL);
  }

  free(template);
   
  html_File(html_file, "<%$ed$%>", "");

  free(html_file);

  return 0;
}







