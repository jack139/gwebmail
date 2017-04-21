#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <mysql.h>
#include <errno.h>
#include "mysql_sendmail.h"


#define CONF_FILE	"/etc/mail/mail.conf"
#define BUF_SIZE	128

MYSQL mysql;
MYSQL_RES *result;
my_ulonglong num_rows;
my_ulonglong num_fields;
char *mysql_alias;
MYSQL_ROW field;
char *name;
struct mysql_sendmail_struct *mysqlconf;


struct mysql_sendmail_struct *get_mysql_conf() 
{
  static struct mysql_sendmail_struct mss;
  static char host[BUF_SIZE], user[BUF_SIZE], password[BUF_SIZE],
    database[BUF_SIZE], user_table[BUF_SIZE],
    alias_table[BUF_SIZE], map_table[BUF_SIZE],
    lhs_col[BUF_SIZE],rhs_col[BUF_SIZE];

  char buf[256];
  char *name, *value;
  int  pos;
  char *delim = "\n \t";
  FILE *conf;
  
  mss.mysql_host = NULL;
  mss.mysql_user = NULL;
  mss.mysql_passwd = NULL;
  mss.mysql_database = NULL;
  mss.mysql_user_table = NULL;
  mss.mysql_map_table = NULL;
  mss.mysql_rhs_col = NULL;
  mss.mysql_lhs_col = NULL;
  
  if(!(conf = fopen(CONF_FILE, "r")))
    /* Could not open config file */
    return NULL;
  while(fgets(buf, 256, conf)) {
    for(pos=0; strchr(delim+1, buf[pos]) && buf[pos]; pos++)
      ;
    name = buf+pos;
    if(*name=='#' || *name=='\n' || *name==0)
      continue;
    for(; !strchr(delim, buf[pos]) && buf[pos]; pos++)
      ;
    buf[pos] = 0;
    if(!*name) continue;
    for(pos++; strchr(delim+1, buf[pos]) && buf[pos]; pos++)
			;
    value = buf+pos;
    for(; !strchr(delim, buf[pos]) && buf[pos]; pos++)
      ;
    buf[pos] = 0;
    if(!*value) continue;
    switch(name[5]) {
    case 'H':
      if(!strcmp(name, "MysqlHost")) {
	strncpy(host, value, BUF_SIZE);
	mss.mysql_host = host;
      }
      break;
    case 'U':
      if(!strcmp(name, "MysqlUsername")) {
	strncpy(user, value, BUF_SIZE);
	mss.mysql_user = user;
      } else if(!strcmp(name, "MysqlUserTable")) {
	strncpy(user_table, value, BUF_SIZE);
	mss.mysql_user_table = user_table;
      }
      break;
    case 'D':
      if(!strcmp(name, "MysqlDatabase")) {
	strncpy(database, value, BUF_SIZE);
	mss.mysql_database = database;
      } 
      break;
    case 'M':
      if(!strcmp(name, "MysqlMapTable")){
	strncpy(map_table, value, BUF_SIZE);
	mss.mysql_map_table = map_table;
      }
      break;
    case 'A':
      if(!strcmp(name, "MysqlAliasTable")) {
	strncpy(alias_table, value, BUF_SIZE);
	mss.mysql_alias_table = alias_table;
      }
      break;
    case 'P':
      if(!strcmp(name, "MysqlPassword")) {
	strncpy(password, value, BUF_SIZE);
	mss.mysql_passwd = password;
      }
      break;
    }
  }
  fclose(conf);
  
  if(mss.mysql_host && mss.mysql_user && /* mss.mysql_passwd && */
     mss.mysql_database && mss.mysql_user_table &&
     mss.mysql_alias_table && mss.mysql_map_table)
    {
      return &mss;
    }
  return NULL;
}


struct passwd * get_mysql_pwd(char * user_name)
{
  char queryBuf[256];
  static struct passwd pw;

  mysql_init(&mysql);
  mysqlconf = get_mysql_conf();
  if(mysqlconf == NULL)
    {
      puts("Error reading config file");
      return NULL;
    }
  if(!mysql_real_connect(&mysql,
			 mysqlconf->mysql_host,
			 mysqlconf->mysql_user,
			 mysqlconf->mysql_passwd,
			 mysqlconf->mysql_database,0,NULL,0))
    {
      printf("Connection to MySQL server(%s) failed\n",mysql_error(&mysql));
      mysql_close(&mysql);
      return NULL;
    }
  snprintf(queryBuf, sizeof(queryBuf), 
	   "select * from %s where uid = \'%s\'",
	   mysqlconf->mysql_user_table,user_name);

  mysql_query(&mysql, queryBuf);
  if((result=mysql_store_result(&mysql)) == NULL)
    {
      puts("Error sending query to server");
      mysql_close(&mysql);
      return NULL;
    }
  num_rows=mysql_num_rows(result);
  num_fields=mysql_num_fields(result);
  if(num_rows == 0)
    {
      mysql_free_result(result);
      mysql_close(&mysql);
      return NULL;
    }
  
  field=mysql_fetch_row(result);

  pw.pw_name = field[0];
  pw.pw_passwd = field[1];
  pw.pw_uid = atoi(field[2]);
  pw.pw_gid = atoi(field[3]);
  if((pw.pw_gecos = field[4]) == NULL)
    pw.pw_gecos = ",,,";
  pw.pw_dir = field[5];
  if((pw.pw_shell = field[6]) == NULL)
    pw.pw_shell = "/bin/noshell";
  mysql_free_result(result);
  mysql_close(&mysql);

  return &pw;
} 


