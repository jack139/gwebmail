/* db.c */

#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include "db.h"
#include "cgic.h"
#include "lang.h"

char g_info[200];
char g_buf[DB_MAXROW][DB_MAXFIELD][DB_MAXSIZE];


void warn(const char *str){ strcpy(g_info,str); }

/*----------------------------------+
  db_open(): open db
  para: none
  ret:
    handle   succ
    NULL     fail

+-----------------------------------*/
MYSQL *db_open()
{
  MYSQL *sql;

  sql=mysql_init(NULL);
  if (sql==NULL) return NULL;
  if (mysql_real_connect (sql, DEF_HOST_NAME, DEF_USER_NAME, DEF_PASSWORD, 
			  DEF_DB_NAME, 0, NULL, 0)==NULL)
    return NULL;
  else
    return sql;
}


/*-----------------------------+
 db_close(): close db
 para: none
 ret:
  0  succ
+-----------------------------*/
int db_close(MYSQL *sql)
{
  mysql_close(sql);
  return 0;
}


/*-----------------------------+
 db_login(): login a user
 para:
  uid  user name
  pwd  passwd
 ret:
  0    succ
  -1   fail

+------------------------------*/
int db_login(const char *uid, const char *pwd)
{
  char sql_cmd[300];
  long re=0L;

  sprintf(sql_cmd, "select pwd from user where uid='%s'", uid);
  if ((re=db_exec(sql_cmd, 1))==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }
  
  if (re)
    {
      sprintf(sql_cmd, "select count(*) from user "
	          "where uid='%s' and pwd='%s'", uid, pwd);
      if ((re=db_exec(sql_cmd, 1))==-1)
       {
          warn(W_DB_ERR);
          return -1;
       }
 
      if (atoi(g_buf[0][0])>0) return 0;
      else
	{
	  warn(W_PASS_ERR);
	  return -1;
	}
    }
  else
    {
      warn(W_NO_USER);
      return -1;
    }

}

/*---------------------------------------------------+
 db_exec() : exec a query
 para:
  sql_cmd     sql query
  have_res    if have result
 ret:
  -1           fail
  >0           num of rows returned

 caution:
   this func is NOT SUIT for query the table
   whose field is larger than DB_MAXSIZE bytes
   or which has more than DB_MAXFIELD fields
   or which has more than DB_MAXROW rows!!!

 +--------------------------------------------------*/
long db_exec(const char *sql_cmd, int have_res)
{
  MYSQL *sql;
  MYSQL_RES *res;
  MYSQL_ROW row;
  long re;
  unsigned int i, field_num;
  
  sql=db_open();
  if (sql==NULL) return -1;

  if (mysql_query(sql, sql_cmd)) return -1;

  if (!have_res)  /* may INSERT, DELETE ... */
    {
      db_close(sql);
      return 0;
    }

  if ((res = mysql_store_result(sql))==NULL)
    {
      db_close(sql);
      return -1;
    }

  if (re=(long)mysql_num_rows(res))
    {
      int j=0;

      field_num=mysql_num_fields(res);
      while(row=mysql_fetch_row(res))
	{
	  for (i=0;i<field_num;i++)
	    {
	      if (row[i]==NULL)
		g_buf[j][i][0]='\0';
	      else
		strcpy(g_buf[j][i], row[i]);
	    }
	  j++;
	  if (j==DB_MAXROW) break;
	}
    }
  
  mysql_free_result(res);
  db_close(sql);

  return re;
}
      

/*----------------------------------------------------+
  db_insert_active(): insert a uid into active list
  para:
    uid        user's login name
    tt         login time (unix time_t time)
    des        unique id (in MD5)
  ret:
    0          succ
    -1         fail

+-----------------------------------------------------*/
int db_insert_active(const char *uid, time_t tt, const char *des)
{
  char sql_cmd[300];
  
  if (db_check_active(uid, des)==1)
    {
      warn(W_USER_BUSY);
      return -1;
    }

  sprintf(sql_cmd, "insert into active (uid, login_t, des) "
	  "values ('%s', %ld, '%s')", uid, tt, des);
  if (db_exec(sql_cmd, 0)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }
    
  return 0;
}

/*-----------------------------------------------+
 db_check_active(): check uid if active
 para:
  uid
  des
 ret:
  1   active
  0   deactive
  -1  fail

+------------------------------------------------*/
db_check_active(const char *uid, const char *des)
{
  char sql_cmd[300];
  long re;

  sprintf(sql_cmd, "select uid from active where "
	  "uid='%s' and des='%s'", uid, des);
  re=db_exec(sql_cmd, 1);
  if (re>0L) return 1;
  if (re==0L) return 0;

  warn(W_DB_ERR);
  return -1;
}

/*-----------------------------------------+
 db_flash_active(): flash active user
 para:
   uid
   des
 ret:
   0     succ
   -1    fail
+------------------------------------------*/
int db_flash_active(const char *uid, const char *des)
{
  char sql_cmd[300];
  int re;

  re=db_check_active(uid,des);
  if (re==-1) return -1;
  if (re==0)
    {
      warn(W_USER_FOOL);
      return -1;
    }

  sprintf(sql_cmd, "update active set login_t=%ld "
	  "where uid='%s' and des='%s'", time(NULL), uid, des);
  if (db_exec(sql_cmd, 0)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }

  return 0;
}


/*-----------------------------------------+
 db_logout(): user logout
 para:
   uid
   des
 ret:
   0   succ
   -1  fail

+------------------------------------------*/
int db_logout(const char *uid, const char *des)
{
  char sql_cmd[300];
  int re;

  re=db_check_active(uid, des);
  if (re==-1) return -1;
  if (re==0)
    {
      warn(W_USER_FOOL);
      return -1;
    }

  sprintf(sql_cmd, "delete from active "
	  "where uid='%s' and des='%s'", uid, des);
  if (db_exec(sql_cmd, 0)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }
  
  return 0;
}

/*--------------------------------------+
  db_get_pwd(): get user's passwd
  para:
    uid
  ret:
    0     succ
    -1    fail
+--------------------------------------*/
int db_get_pwd(const char *uid, char *pwd)
{
  char sql_cmd[300];
  int re;

  sprintf(sql_cmd, "select pwd from user where uid='%s'", uid);
  if ((re=db_exec(sql_cmd, 1))==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }

  if (re==0)
    {
      warn(W_NO_USER);
      return -1;
    }

  strcpy(pwd, g_buf[0][0]);

  return 0;
}


/*-------------------------------------------
  db_update_unread()
  ret:
    0   succ
    -1  fail
+-------------------------------------------*/
int db_update_unread(const char *uid, const char *folder, const char *mid)
{
  char sql_cmd[300];
  
  sprintf(sql_cmd, "select unread from mail where"
	  " uid='%s' and mail_id='%s'", uid, mid);
  if (db_exec(sql_cmd, 1)==-1)
    {
      warn(W_DB_ERR);
      return -1;
    }

  if (atoi(g_buf[0][0])==1)
    {
      sprintf(sql_cmd, "update mail set unread='0' where"
	      " uid='%s' and mail_id='%s'", uid, mid);
      if (db_exec(sql_cmd, 0)==-1)
	{
	  warn(W_DB_ERR);
	  return -1;
	}

      sprintf(sql_cmd, "update folder set unread=unread-1 where"
	      " uid='%s' and folder_n='%s'", uid, folder);
      if (db_exec(sql_cmd, 0)==-1)
	{
	  warn(W_DB_ERR);
	  return -1;
	}
    }

  return 0;
}

  
/*----------------------------------------------------
 db_get_content()
 ret:
  -1           fail
  0            succ
+--------------------------------------------------*/
int db_get_content(const char *uid, const char *mid, char *buf)
{
  MYSQL *sql;
  MYSQL_RES *res;
  MYSQL_ROW row;
  long re;
  unsigned int i;
  char sql_cmd[300];

  
  sprintf(sql_cmd, "select content from mail"
	  " where uid='%s' and mail_id='%s'", uid, mid);

  sql=db_open();
  if (sql==NULL) return -1;

  if (mysql_query(sql, sql_cmd)) return -1;

  if ((res = mysql_store_result(sql))==NULL)
    {
      db_close(sql);
      return -1;
    }

  if (mysql_num_rows(res)>0)
    {
      if(row=mysql_fetch_row(res))
	{
	  strcpy(buf, row[0]);
	}
    }
  else
    {
      db_close(sql);
      return -1;
    }
  
  mysql_free_result(res);
  db_close(sql);

  return 0;
}
      
/*--------------------------------------------+
  db_prepare()
  ret:
    str   succ 
    NULL  fail
+---------------------------------------------*/
char *db_prepare(char *s)
{
  char *p, *p2, *p3;
  
  p=(char *)malloc(strlen(s)+1);
  if (p==NULL)
    {
      warn(W_NO_MEM);
      return NULL;
    }

  strcpy(p, s);

  for(p2=p, p3=s; *p2; p2++,p3++)
    {
      switch(*p2)
	{
	case '\n':
	  *p3='\\'; p3++; *p3='n'; break;

	case '\t':
	  *p3='\\'; p3++; *p3='t'; break;

	case '\r':
	  *p3='\\'; p3++; *p3='r'; break;

	case '\b':
	  *p3='\\'; p3++; *p3='b'; break;

	case '\'':
	  *p3='\\'; p3++; *p3='\''; break;

	case '\"':
	  *p3='\\'; p3++; *p3='\"'; break;
	  
	case '\\':
	  *p3='\\'; p3++; *p3='\\'; break;

	default:
	  *p3=*p2;
	}
    }
  *p3='\0';

  free(p);
  return s;
}


/*--------------------------------------
  db_if_pure_string()
--------------------------------------*/
int db_if_pure_string(unsigned char *s)
{
  unsigned char *p;

  if (!isalpha(s[0])) return 0;

  for(p=s; *p; p++)
    {
      if (isalnum(*p)) continue;
      else return 0;
    }

  return 1;
}

/*----------------------------------------
  db_if_hz_string()
----------------------------------------*/
int db_if_hz_string(unsigned char *s)
{
  unsigned char *p;
  
  if (s[0]<128 && !isalpha(s[0])) return 0;

  for (p=s; *p; p++)
    {
      if (isalnum(*p)) continue;
      else if (*p>127) continue;
      else return 0;
    }

  return 1;
}

/*---------------------------------------------------------
    function to get username
---------------------------------------------------------*/
char *get_user_name(const char *addr)
{
  char *p;
  static char t[200];

  strcpy(t, addr);
  for(p=t; *p && *p!='.'; p++);
  if (*p!='.') return NULL;
  *p='\0';
  
  return t;
}

/*---------------------------------------------------------
    function to get domainname
---------------------------------------------------------*/
char *get_domain_name(const char *addr)
{
  char *p;
  static char t[200];

  for(p=addr; *p && *p!='.'; p++);
  if (*p!='.') return NULL;
  strcpy(t, p+1); 
 
  return t;
}
