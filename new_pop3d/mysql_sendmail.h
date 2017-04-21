#include <pwd.h>
#include <sys/types.h>

struct mysql_sendmail_struct
{
        char            *mysql_host;
        char            *mysql_user;
        char            *mysql_passwd;
        char            *mysql_database;
        char            *mysql_user_table;
        char            *mysql_alias_table;
        char            *mysql_map_table;
        char            *mysql_lhs_col;
        char            *mysql_rhs_col;
};

struct passwd *get_mysql_pwd(char *user_name);
struct passwd *get_mysql_uid(int user_id);
char *get_mysql_alias(char *mysql_name, int *status);
struct mysql_sendmail_struct *get_mysql_conf();
char *mysql_map_dequote(char *);
static struct passwd pw;
typedef struct mysql_sendmail_struct  MYSQL_MAP_STRUCT;
MYSQL_MAP_STRUCT *mysql_map_scanconf();

#define EX_NOTFOUND    EX_NOHOST
#define ALIAS_LHS "address"
#define ALIAS_RHS "alias"
