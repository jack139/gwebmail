/* db functions for cgi */

#ifndef _DB_H
#define _DB_H

#include <mysql.h>

#define MAIL_SVR "localhost"

#define DEF_HOST_NAME "localhost"
#define DEF_USER_NAME "root"
#define DEF_PASSWORD  ""
#define DEF_DB_NAME   "webmail"

#define MAXUID 20
#define MAXCID 70
#define MAXID  (MAXUID+MAXCID)
#define MAXFOLDER 50
#define CONFIG_PATH "/usr/local/conf"

#define DB_MAXROW 100
#define DB_MAXFIELD 20
#define DB_MAXSIZE 300

extern char g_info[200];
extern char g_buf[DB_MAXROW][DB_MAXFIELD][DB_MAXSIZE];

void warn(const char *str);

MYSQL *db_open();
int db_close(MYSQL *sql);

long db_exec(const char *sql_cmd, int have_res);

int db_login(const char *uid, const char *pwd);
int db_logout(const char *uid, const char *des);
int db_insert_actice(const char *uid, const time_t tt, const char *des);
int db_check_active(const char *uid, const char *des);
int db_flash_active(const char *uid, const char *des);
int db_get_pwd(const char *uid, char *pwd);
int db_update_unread(const char *uid, const char *folder, const char *mid);
int db_get_content(const char *uid, const char *mid, char *buf);
char *db_prepare(char *s);
int db_if_pure_string(unsigned char *s);
int db_if_hz_string(unsigned char *s);

/*  in md5.c */
void my_md5(const char *uid, time_t tt, char *des);
void my_md5_str(const char *s, char *des);

char *get_user_name(const char *addr);
char *get_domain_name(const char *addr);


#endif









