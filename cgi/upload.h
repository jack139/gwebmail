
#define TMP_PATH "/var/tmp/webmail"
#define UPLOAD_MIME_TYPE "multipart/form-data"
#define MAX_UPLOAD 1024*1024*3
#define MAX_FORM 10


typedef struct 
{
  char name[101];
  char file_n[101];
  unsigned char *value;
  long length;
  int next;
} form_list;

extern form_list form_e[MAX_FORM];
extern unsigned char *upload_buf;


int upload();
int get_form_entry(int all_e, char *name);
int get_form_value(int all_e, char *name, char *value, size_t len);
  
  










