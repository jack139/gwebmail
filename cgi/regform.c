/* regform.c */

#include <stdio.h>
#include <time.h>
#include "cgic.h"
#include "db.h"
#include "lang.h"
#include "html.h"

int regform();

int cgiMain() {
  char uid[MAXID+1],des[40];


  cgiHeaderContentType("text/html");

  if (regform()==-1)
    {
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

  html_file=html_Load(TEMPLATE_PATH, "/regform.html");

  html_File(html_file, "", "<%$bg$%>");

  template=html_GetTemplate(html_file, "<%$bg$%>", "<%$ed$%>");

  sprintf(temp, "%s@%s", get_user_name(uid), get_domain_name(uid));
  html_FillTemplate(template,
			"uid", uid,
			"des", des,
			"addr", temp,
			NULL);
  free(template);

  html_File(html_file, "<%$ed$%>", "");

  free(html_file);

  return 0;
}






