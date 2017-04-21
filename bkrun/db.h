/* db functions for cgi */

#ifndef _DB_H
#define _DB_H

#include <mysql.h>


#define DEF_HOST_NAME "localhost"
#define DEF_USER_NAME "root"
#define DEF_PASSWORD  ""
#define DEF_DB_NAME   "webmail"

#define MAXUID 15

#define DB_MAXROW 100
#define DB_MAXFIELD 20
#define DB_MAXSIZE 300

extern char g_info[200];
extern char g_buf[DB_MAXROW][DB_MAXFIELD][DB_MAXSIZE];

void warn(const char *str);

MYSQL *db_open();
int db_close(MYSQL *sql);

long db_exec(const char *sql_cmd, int have_res);

char *db_prepare(char *s);
int db_if_pure_string(char *s);

/*  in md5.c */
void my_md5(const char *uid, time_t tt, char *des);
void my_md5_str(const char *s, char *des);


#endif









