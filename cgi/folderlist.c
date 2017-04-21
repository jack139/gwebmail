/* folderlist.c */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "cgic.h"
#include "db.h"
#include "mail_sub.h"
#include "lang.h"
#include "html.h"

char folder_n[MAXFOLDER+1][25];
int  mails[MAXFOLDER+1], unread[15];
long sz[MAXFOLDER+1];
char mons[][10]={ "Jan", "Feb", "Mar", "Apr",
		  "May", "Jun", "Jul", "Aug",
		  "Sep", "Oct", "Nov", "Dec"  };

int get_folder_info(char *uid, char *des);
int get_inbox_info(const char *uid, int nums);

int cgiMain() {
  char uid[MAXID+1],des[40];
  int i, re;
  char *html_file, *template;
  char t1[200], t2[100], tmp[50], tmp2[50], tmp3[50];


  cgiHeaderContentType("text/html");

  if ((re=get_folder_info(uid, des))>0)
    {
      int t_mails=0, t_unread=0;
      long t_sz=0L;

      html_file = html_Load(TEMPLATE_PATH, "/folderlist.html");

      html_File(html_file, "", "<%$bg0$%>");

      template = html_GetTemplate(html_file, "<%$bg0$%>", "<%$ed0$%>");
      sprintf(t1, "%s@%s", get_user_name(uid), get_domain_name(uid));
      sprintf(tmp, "%d", re);
      html_FillTemplate(template, "addr", t1, "nn", tmp, NULL);
      free(template);

      html_File(html_file, "<%$ed0$%>", "<%$bg1$%>");

      template = html_GetTemplate(html_file, "<%$bg1$%>", "<%$ed1$%>");

      for(i=0; i<re; i++)
	{
	  sprintf(t1, "%s/folder?uid=%s&box=%s&des=%s", 
                CGI_BIN, uid, folder_n[i], des);
	  sprintf(tmp, "%d", unread[i]);
	  sprintf(tmp2, "%d", mails[i]);
	  sprintf(tmp3, "%.2f", (float)(sz[i]/1024.0));
	  html_FillTemplate(template, 
			      "act", t1, 
			      "name", folder_n[i],
			      "n1", tmp,
			      "n2", tmp2,
			      "n3", tmp3,
			      NULL);

	  t_mails+=mails[i];
	  t_unread+=unread[i];
	  t_sz+=sz[i];
	}

      free(template);

      template = html_GetTemplate(html_file, "<%$ed1$%>", "<%$bg2$%>");

      sprintf(tmp, "%d", t_unread);
      sprintf(tmp2, "%d", t_mails);
      sprintf(tmp3, "%.2f", (float)(t_sz/1024.0));      
      html_FillTemplate(template, 
			  "n1", tmp,
			  "n2", tmp2,
			  "n3", tmp3,
			  NULL);
      free(template);

      template = html_GetTemplate(html_file, "<%$bg2$%>", "<%$ed2$%>");
      sprintf(t1, "%s/addfolder", CGI_BIN);
      sprintf(t2, "%s/delfolder", CGI_BIN);
      html_FillTemplate(template, 
			  "fcg_add", t1,
			  "uid1", uid,
			  "des1", des,
			  "fcg_del", t2,
			  "uid2", uid,
			  "des2", des,
			  NULL);
      free(template);

      template = html_GetTemplate(html_file, "<%$bg3$%>", "<%$ed3$%>");

      for(i=0; i<re; i++)
	{
	  if (strcmp(folder_n[i], INBOX_NAME)==0) continue;
	  if (strcmp(folder_n[i], SENT_NAME)==0) continue;
	  if (strcmp(folder_n[i], TRASH_NAME)==0) continue;

	  html_FillTemplate(template, 
			      "val", folder_n[i],
			      "name", folder_n[i],
			      NULL);
	}
      if (re==3) 
	html_FillTemplate(template, 
			    "val", "__NONE__",
			    "name", M_NULL,
			    NULL);
      free(template);
      
      html_File(html_file, "<%$ed3$%>", "");

      free(html_file);
    }
  else
    {
      fprintf(cgiOut, "<html><head>\n");
      fprintf(cgiOut,"<meta http-equiv=Content-Type "
	      "content=\"text/html; charset=gb2312\">\n");
      fprintf(cgiOut, "<title>folderlist</title></head>\n");
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
 get_folder_info(): check folder info                    
 para:   
   r_uid   retrive uid
   r_des   retrive md5_id
 ret:                               
   >0      success (folders num)
   -1      fail
----------------------------------------*/
int get_folder_info(char *uid, char *des)
{
  char sql_cmd[300];
  int result, re, i, t_re;
  int t_mail, t_unread;
  long t_size;

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

  /*  
  strcpy(uid, "gt");
  strcpy(des, "1234");
  */

 again:
  sprintf(sql_cmd, "select folder_n, mails, unread, size"
	  " from folder where uid='%s' order by id", uid);
  re=db_exec(sql_cmd, 1);
  if (re==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }

  if (re==0)
    {
      sprintf(sql_cmd, "insert into folder (uid, folder_n)"
	      "values ('%s', '"INBOX_NAME"')", uid);
      if (db_exec(sql_cmd, 0)==-1)
	{
	  warn(W_DB_ERR);
	  return -1;
	}

      sprintf(sql_cmd, "insert into folder (uid, folder_n)"
	      "values ('%s', '"SENT_NAME"')", uid);
      if (db_exec(sql_cmd, 0)==-1)
	{
	  warn(W_DB_ERR);
	  return -1;
	}

      sprintf(sql_cmd, "insert into folder (uid, folder_n)"
	      "values ('%s', '"TRASH_NAME"')", uid);
      if (db_exec(sql_cmd, 0)==-1)
	{
	  warn(W_DB_ERR);
	  return -1;
	}
      
      goto again;
    }
  
  for(i=0; i<re; i++)
    {
      strcpy(folder_n[i], g_buf[i][0]);
      mails[i]=atoi(g_buf[i][1]);
      unread[i]=atoi(g_buf[i][2]);
      sz[i]=atol(g_buf[i][3]);
    }

  if (get_inbox_info(uid, re)==-1) return -1;

  t_re=re;
  for(i=0; i<t_re; i++)
    {
      sprintf(sql_cmd, "select count(*),sum(size) from mail"
	      " where uid='%s' and folder_n='%s'", uid, folder_n[i]);
      re=db_exec(sql_cmd, 1);
      if (re==-1) return -1;

      t_mail=atoi(g_buf[0][0]);
      t_size=atol(g_buf[0][1]);
      
      sprintf(sql_cmd, "select count(*) from mail"
	      " where uid='%s' and folder_n='%s' and unread=1",
	      uid, folder_n[i]);
      re=db_exec(sql_cmd, 1);
      if (re==-1) return -1;

      t_unread=atoi(g_buf[0][0]);

      if (t_mail!=mails[i] || t_size!=sz[i] || t_unread!=unread[i])
	{
	  sprintf(sql_cmd, "update folder set mails=%d, unread=%d,"
		  "size=%ld where uid='%s' and folder_n='%s'",
		  t_mail, t_unread, t_size, uid, folder_n[i]);
	  if (db_exec(sql_cmd, 0)==-1) continue;

	  mails[i]=t_mail;
	  sz[i]=t_size;
	  unread[i]=t_unread;
	}
    }

  return t_re;
}


/*--------------------------------------------+
 get_inbox_info()

 ret
   0  succ
   -1 fail
+--------------------------------------------*/

#define MID_LIST(x) (unread_list+(60*x)+2)
#define UNREAD_LIST(x) (unread_list+(60*x))

int get_inbox_info(const char *uid, int nums)
{
  int sock;
  int re, idx, i, ret=-1;
  char pwd[50], temp[500];
  int t_mails, t_unread;
  long t_sz;
  char *unread_list;


  if (db_get_pwd(uid, pwd)==-1) return -1;

  for(i=0; i<nums; i++)
    {
      if (strcmp(folder_n[i], INBOX_NAME)==0)
	{
	  idx=i;
	  break;
	}
    }

  sock=init_POP3(POP3_HOST);
  if (sock==-1)
    {
      warn(W_MSR_ERR);
      return -1;
    }
  
  /* get POP3d respose */
  read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
  if (sock_buf[0]!='+')
    {
      warn(W_MSR_ERR);
      close_POP3(sock);
      return -1;
    }

  /* login POP3 */
  sprintf(temp, "USER %s\r\n", uid);
  write_command(sock, temp, strlen(temp));

  read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
  if (sock_buf[0]!='+')
    {
      warn(W_MSR_ERR);
      goto pop_quit;
    }

  sprintf(temp, "PASS %s\r\n", pwd);
  write_command(sock, temp, strlen(temp));
  
  read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
  if (sock_buf[0]!='+') 
    {
      warn(W_MSR_ERR);
      goto pop_quit;
    }

  /* login POP3 passed */
  
  sprintf(temp, "STAT\r\n");
  write_command(sock, temp, strlen(temp));

  read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
  if (sock_buf[0]!='+')
    {
      warn(W_MSR_ERR);
      goto pop_quit;
    }

  sscanf(sock_buf, "%s %d %ld\n", temp, &t_mails, &t_sz);


  /* mail have retrieved by POP3 client */
  if (t_mails==0)
    {
      sprintf(temp, "delete from mail where"
	      " uid='%s' and folder_n='"INBOX_NAME"'", uid);
      if (db_exec(temp, 0)==-1)
	{
	  warn(W_DB_ERR);
	  goto pop_quit;
	}

      ret=0;

      goto pop_quit;
    }

  t_unread=0;

  /* have new mails */

  unread_list=(char *)malloc(t_mails*60+60);
  if (unread_list==NULL)
    {
      warn(W_NO_MEM);
      goto pop_quit;
    }

  /* process mail info by POP3 */

  for (i=1; i<=t_mails; i++)
    {
      long ttt;

      sprintf(temp, "UIDL %d\r\n", i);
      write_command(sock, temp, strlen(temp));

      read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
      if (sock_buf[0]!='+')
	{
	  warn(W_MSR_ERR);
	  goto pop_quit;
	}

      sscanf(sock_buf, "%s %ld %s", temp, &ttt, MID_LIST(i));

      sprintf(temp, "select unread from mail"
	      " where mail_id='%s' and uid='%s'"
	      " and folder_n='"INBOX_NAME"'", MID_LIST(i), uid);
      if ((re=db_exec(temp, 1))==-1)
	{
	  warn(W_DB_ERR);
	  goto pop_quit;
	}

      if (re==0) *UNREAD_LIST(i)=1;
      else *UNREAD_LIST(i)=atoi(g_buf[0][0]);

    }

  sprintf(temp, "delete from mail where uid='%s' and"
	  " folder_n='"INBOX_NAME"'", uid);
  if (db_exec(temp, 0)==-1)
    {
      warn(W_DB_ERR);
      goto pop_quit;
    }

  for (i=1; i<=t_mails; i++)
    {
      long m_sz;
      char m_from[101],
	m_to[101],
	m_subject[201],	
	m_date[101];
      char *ptr, tmp[6][20];
      long ttt;
      int mon;
      
      /* get the size of single mail */
      sprintf(temp, "LIST %d\r\n", i);
      write_command(sock, temp, strlen(temp));

      read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
      if (sock_buf[0]!='+')
	{
	  warn(W_MSR_ERR);
	  goto pop_quit;
	}

      sscanf(sock_buf, "%s %ld %ld\n", temp, &ttt, &m_sz);

      /* get detail info of a single mail */
      sprintf(temp, "TOP %d 0\r\n", i);
      write_command(sock, temp, strlen(temp));
      
      re=read_respose(sock, sock_buf, SOCKSIZE, "\r\n.\r\n");
      /*      printf("%s <BR>\n", sock_buf);   */
      if (re==0)
	{
	  warn(W_MSR_ERR);
	  return -1;
	}
      
      ptr=get_a_part(sock_buf, "From: ", "\r\n", &ttt);
      if (ptr)
	{
	  ttt-=6;
	  if (ttt>100) ttt=100;
	  memcpy(temp, ptr+6, ttt); 
	  temp[ttt]='\0';
	  	
	  ptr=get_a_part(temp, "<", ">", &ttt);
	  if (ptr==NULL) strcpy(m_from, temp);
	  else
	    {
	      ttt--;
	      memcpy(m_from, ptr+1, ttt);
	      m_from[ttt]='\0';
	    }
	}
      else m_from[0]='\0';

      ptr=get_a_part(sock_buf, "To: ", "\r\n", &ttt);
      if (ptr)
	{
	  ttt-=4;
	  if (ttt>100) ttt=100;
	  memcpy(temp, ptr+4, ttt);
	  temp[ttt]='\0';

	  ptr=get_a_part(temp, "<", ">", &ttt);
	  if (ptr==NULL) strcpy(m_to, temp);
	  else
	    {
	      ttt--;
	      memcpy(m_to, ptr+1, ttt);
	      m_to[ttt]='\0';
	    }
	}
      else m_to[0]='\0';	

      ptr=get_a_part(sock_buf, "Subject: ", "\r\n", &ttt);
      if (ptr)
	{
	  ttt-=9;
	  if (ttt>200) ttt=200;
	  memcpy(m_subject, ptr+9, ttt);
	  m_subject[ttt]='\0';
	}
      else m_subject[0]='\0';
      filt_subject(m_subject);
      
      ptr=get_a_part(sock_buf, "Date: ", "\r\n", &ttt);
      if (ptr)
	{
	  ttt-=6;
	  if (ttt>100) ttt=100;
	  memcpy(m_date, ptr+6, ttt);
	  m_date[ttt]='\0';
	}
      else m_date[0]='\0';

      sscanf(m_date,"%s %s %s %s %s", 
	     tmp[0], tmp[1], tmp[2], tmp[3], tmp[4]);
      
      for (mon=0; mon<12; mon++)
	{
	  if (strncasecmp(mons[mon], tmp[2], 3)==0) break;
	}
      
      if (mon<12) sprintf(tmp[2], "%02d", mon+1);
      
      sprintf(m_date, "%s-%s-%s %s", tmp[3], tmp[2], tmp[1], tmp[4]);

      sprintf(temp, 
	      "insert into mail"
	      " (uid,folder_n,mail_id,sender,recv,tm,title,size,unread,"
	      " content) values ('%s','"INBOX_NAME"','%s','%s','%s',"
	      "'%s','%s',%ld,'%1d', '')",
	      uid, MID_LIST(i), 
	      db_prepare(m_from),
	      db_prepare(m_to),
	      m_date,
	      db_prepare(m_subject)
	      , m_sz, (int)(*UNREAD_LIST(i)));
      if (db_exec(temp, 0)==-1)
	{
	  warn(W_DB_ERR);
	  goto pop_quit;
	}

      t_unread+=(int)(*UNREAD_LIST(i));

    }

  free(unread_list);
  
  ret=0;
  
  /* quit POP3 */

 pop_quit:
  
  sprintf(temp, "quit\r\n");
  write_command(sock, temp, strlen(temp));
  
  read_respose(sock, sock_buf, SOCKSIZE, "\r\n");

  close_POP3(sock);

  return ret;
}





