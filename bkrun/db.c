/* db.c */

#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include "db.h"


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
      warn("ÄÚ´æ²»×ã£¡(prepare)");
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
int db_if_pure_string(char *s)
{
  char *p;

  if (!isalpha(s[0])) return 0;

  for(p=s; *p; p++)
    {
      if (isalnum(*p)) continue;
      else return 0;
    }

  return 1;
}

