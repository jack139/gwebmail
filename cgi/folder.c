/* folder.c */

#include <stdio.h>
#include <time.h>
#include <mysql.h>
#include "cgic.h"
#include "db.h"
#include "mail_sub.h"
#include "lang.h"
#include "html.h"

int get_para(char *uid, char *des, char *box);
int get_mail_list(const char *uid, const char *des, const char *folder);

int cgiMain() {
  char uid[MAXID+1],des[40],box[25];


  cgiHeaderContentType("text/html");

  if (get_para(uid, des, box)==0)
    {
      if (get_mail_list(uid, des, box)==-1) goto fail;
      
      return 0;
    }

 fail:

  fprintf(cgiOut, "<html><head>\n");
  fprintf(cgiOut,"<meta http-equiv=Content-Type "
	  "content=\"text/html; charset=gb2312\">\n");
  fprintf(cgiOut, "<title>folder</title></head>\n");

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
 get_para(): get paras
 para:   
   r_uid   retrive uid
   r_des   retrive md5_id
 ret:                               
   0       success                     
   -1      fail
----------------------------------------*/
int get_para(char *uid, char *des, char *box)
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

  result=cgiFormStringNoNewlines("box", box, 21);
  if (result!=cgiFormSuccess)
    {
      warn(W_CGI_ERR);
      return -1;
    }

  if (db_flash_active(uid, des)==-1) return -1;


  return 0;
}

/*---------------------------------------------------+
 get_mail_list() : 

 ret:
  -1           fail
	  1            succ
+--------------------------------------------------*/
int get_mail_list(const char *uid, const char *des, const char *folder)
{
  MYSQL *sql;
  MYSQL_RES *res;
  MYSQL_ROW row;
  long re;
  unsigned int i, field_num;
  char sql_cmd[500];
  int m_mails, m_unread;
  char *html_file, *template;
  char t1[200], t2[200], tmp[50], tmp2[50], tmp3[50];
  char *p;


  sprintf(sql_cmd, "select mails,unread from folder"
	 " where uid='%s' and folder_n='%s'", uid, folder);
  if (db_exec(sql_cmd, 1)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }

  m_mails=atoi(g_buf[0][0]);
  m_unread=atoi(g_buf[0][1]);

  html_file = html_Load(TEMPLATE_PATH, "/folder.html");  

  html_File(html_file, "", "<%$bg0$%>");

  template = html_GetTemplate(html_file, "<%$bg0$%>", "<%$ed0$%>");

  sprintf(t1, "%s@%s", get_user_name(uid), get_domain_name(uid));
  sprintf(t2, "%s/move", CGI_BIN);
  sprintf(tmp, "%d", m_mails);
  html_FillTemplate(template, 
		      "addr", t1,
		      "fcg_mov", t2,
		      "uid", uid,
		      "des", des,
		      "nn", tmp,
		      "box", folder,
		      NULL);
  free(template);

  html_File(html_file, "<%$ed0$%>", "<%$bg1$%>");

  template = html_GetTemplate(html_file, "<%$bg1$%>", "<%$ed1$%>");
  sprintf(tmp, "%d", m_mails);
  sprintf(tmp2, "%d", m_unread);
  html_FillTemplate(template, 
		      "box2", folder,
		      "tot", tmp,
		      "tot2", tmp2,
		      NULL);
  free(template);

  html_File(html_file, "<%$ed1$%>", "<%$bgg$%>");

  template = html_GetTemplate(html_file, "<%$bgg$%>", "<%$edd$%>");
  if (strcmp(folder, SENT_NAME)==0)
    html_FillTemplate(template, "name", M_RECV, NULL);
  else
    html_FillTemplate(template, "name", M_SEND, NULL);
  free(template);

  html_File(html_file, "<%$edd$%>", "<%$bg2$%>");


  template = html_GetTemplate(html_file, "<%$bg2$%>", "<%$ed2$%>");  

  sprintf(sql_cmd, "select unread,mail_id,sender,recv,tm,title,size from mail"
	  " where uid='%s' and folder_n='%s' order by tm desc", uid, folder);
  
  sql=db_open();
  if (sql==NULL)
    {
      warn(W_DB_ERR);
      free(template);
      free(html_file);
      return -1;
    }

  if (mysql_query(sql, sql_cmd)) 
    {
      warn(W_DB_ERR);
      db_close(sql);
      free(template);
      free(html_file);
      return -1;
    }

  if ((res = mysql_store_result(sql))==NULL)
    {
      warn(W_DB_ERR);
      db_close(sql);
      free(template);
      free(html_file);
      return -1;
    }

  if (re=(long)mysql_num_rows(res))
    {
      int j=0;

      field_num=mysql_num_fields(res);
      while(row=mysql_fetch_row(res))
	{
	  j++;
	  
	  for (i=0;i<field_num;i++)
	    {
	      strcpy(g_buf[0][i], row[i]);
	    }

          sprintf(tmp, "check%d", j);
	  sprintf(tmp2, "mid%d", j);
	  if (g_buf[0][0][0]=='1')
	    {
	      strcpy(t1, "<img src=/images/new.gif width=28 height=11>");
	    }
	  else t1[0]='\0';
	  if (strcmp(folder, SENT_NAME)==0) p=g_buf[0][3];
	  else p=g_buf[0][2];
	  sprintf(t2, "%s/showmail?uid=%s&mid=%s&des=%s", 
		  CGI_BIN, uid, g_buf[0][1], des);
	  if (strlen(g_buf[0][5])==0)
	    strcpy(g_buf[0][5], M_NOSUBJ);
	  sprintf(tmp3, "%.2f", (float)(atol(g_buf[0][6])/1024.0));
	  html_FillTemplate(template, 
			      "name", tmp, 
			      "name2", tmp2,
			      "mid", g_buf[0][1],
			      "img", t1,
			      "from", p,
			      "date", g_buf[0][4],
			      "act", t2,
			      "title", g_buf[0][5],
			      "sz", tmp3,
			      NULL);

	}
      html_File(html_file, "<%$ed2$%>", "<%$tag$%>");
    }
  else
    {
      html_File(html_file, "<%$other$%>", "<%$edother$%>");
    }

  free(template);

  html_File(html_file, "<%$tag$%>", "<%$bg3$%>");
  
  mysql_free_result(res);
  db_close(sql);

  sprintf(sql_cmd, "select folder_n from folder where uid='%s'"
	  "order by id", uid);
  if ((re=db_exec(sql_cmd, 1))==-1)
    {
      warn(W_DB_ERR);
      free(html_file);
      return -1;
    }

  template = html_GetTemplate(html_file, "<%$bg3$%>", "<%$ed3$%>");    

  for (i=0; i<re; i++)
    {
      if (strcmp(folder, g_buf[i][0])==0) continue;
      if (strcmp(INBOX_NAME, g_buf[i][0])==0) continue;
      if (strcmp(TRASH_NAME, g_buf[i][0])==0)
	html_FillTemplate(template, 
			    "val", g_buf[i][0],
			    "sel", "selected",
			    "name", g_buf[i][0],
			    NULL);
      else
	html_FillTemplate(template, 
			    "val", g_buf[i][0],
			    "sel", "",
			    "name", g_buf[i][0],
			    NULL);
    }
  free(template);

  html_File(html_file, "<%$ed3$%>", "<%$bg4$%>");


  if (strcmp(folder, TRASH_NAME)==0)
    {
      template = html_GetTemplate(html_file, "<%$bg4$%>", "<%$ed4$%>");    
      sprintf(t1, "%s/empty?uid=%s&des=%s", CGI_BIN, uid, des);
      html_FillTemplate(template, "act", t1, NULL);
      free(template);
    }

  html_File(html_file, "<%$ed4$%>", "<%$other$%>");

  free(html_file);

  return re;
}
      


