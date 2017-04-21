#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "db.h"
#include "mail_sub.h"

#define MAXBOXSIZE (1024*1024*5)
#define SPOOL_PATH "/var/spool/mail/"

#define EATSPACE(s)  while(isspace(*s)&&(*s!='\0')) ++s

long cnt;

int check_mailbox_size();
int do_check(char *);

int main()
{
  char recv[100], send[100], subj[200], msg[200], sid[20];
  char sql_cmd[500];
  int sm , i, re, tot;
  long id;
  char temp[200];
  int sock;
  char *p, *q;
  
  puts("-- Start!");

  cnt=1;

  fcntl(0, F_SETFL, O_NONBLOCK);
  while(1)
    {
      if (getchar()=='q') break;

      sleep(2);
      
      // delete 
      sprintf(sql_cmd, "delete from active where"
	      " unix_timestamp()-login_t>3600");
      if (db_exec(sql_cmd, 0)==-1)
	{
	  puts("-- DB error to delete Active!");
	  continue;
	}
      
      cnt--;
      if (cnt<=0)
	{
	  check_mailbox_size();
	  cnt=1800;
	}
      puts("-- IDLE");

    }

  puts("-- Bye!");

  return 0;
}

/*****************************************************
 * check user's inbox if too big
 * if so, get it small
 */
int check_mailbox_size()
{
  MYSQL *sql;
  MYSQL_RES *res;
  MYSQL_ROW row;
  long re;
  unsigned int i, field_num;
  char uid[200];

  puts("-- Size Checking!");

  sql=db_open();
  if (sql==NULL)
    {
      puts("-- DB error to open!");
      return -1;
    }

  if (mysql_query(sql, "select uid from user order by uid"))
    {
      puts("-- DB error to query!");
      return -1;
    }

  if ((res = mysql_store_result(sql))==NULL)
    {
      db_close(sql);
      return 0;
    }

  if (re=(long)mysql_num_rows(res))
    {
      while(row=mysql_fetch_row(res))
	{
	  if (row[0]==NULL) continue;
	  else strcpy(uid, row[0]);

	  do_check(uid);
	}
    }
  
  mysql_free_result(res);
  db_close(sql);

  return re;
}
      

int do_check(char *uid)
{
  char fn[150], fn2[150];
  struct stat sta;
  size_t fl;
  FILE *fp, *fp2;
  int j;
  uid_t uID;
  gid_t gID;

  sprintf(fn, SPOOL_PATH"%s", uid);
  if (access(fn, F_OK)==-1) return 0;
  if (stat(fn, &sta)==-1) return -1;
  fl=sta.st_size;
  uID=sta.st_uid;
  gID=sta.st_gid;

  if (fl<MAXBOXSIZE) return 0;
  
  printf("-- Cut size of %s@mysite.com!\n", uid);

  sprintf(fn2, SPOOL_PATH"00%s", uid);

  fp=fopen(fn, "r");
  if (fp==NULL)
    {
      puts("-- File error to open!");
      return -1;
    }

  fp2=fopen(fn2, "w");
  if (fp2==NULL) 
    { 
      fclose(fp); 
      puts("-- File error to open!");
      return -1; 
    }

  for (j=0; j<2; j++)
    {
      while(1)
	{
	  if (fgets(sock_buf, SOCKSIZE, fp)==NULL) break;
	  if (strncasecmp(sock_buf, "From ", 5)==0) break;
	}
    }

  fputs(sock_buf, fp2);

  while(1)
    {
      if (fgets(sock_buf, SOCKSIZE, fp)==NULL) break;
      fputs(sock_buf, fp2);
    }

  fclose(fp);
  fclose(fp2);

  if (chown(fn2, uID, gID)==-1)
    {
      puts("-- File error to chown!");
      return -1;
    }

  if (unlink(fn)==-1)
    {
      puts("-- File error to delete!");
      return -1;
    }

  if (rename(fn2, fn)==-1)
    {
      puts("-- File error to rename!");
      return -1;
    }
 
  puts("-- Cut OK!");

  return 0;
}


