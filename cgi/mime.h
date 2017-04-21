/* mime.h */

#define C_MULTI    0x80
#define C_TEXT     0x10
#define C_IMAGE    0x04
#define C_APP      0x01
#define C_OTHER    0x00

#define MU_ALTER   0xa0
#define MU_RELAT   0xc0
#define MU_MIXED   0xe0

#define TE_PLAIN   0x10
#define TE_HTML    0x18

#define IM_GIF     0x04
#define IM_JPG     0x06

#define AP_APP     0x01

#define CODE_B64   0x01
#define CODE_QP    0x02
#define CODE_7BIT  0x00


typedef struct
{
  unsigned int content_type;
  unsigned int transfer_encoding;

  /*
    when content_type=multipart,
    the content_x is BOUNDARY;

    when papa's content_type=related
    the content_x may be CONTENT_ID;

    when content_type=application,
    the content_x is NAME;
  */    
  char content_x[200];  

  int face;
  char *bg;
  long len;
  int papa;
  int next;
} mime_entry;


extern mime_entry mime_e[20];

int set_entry(char *body, long b_len, int now_e);
int prepare_entry(char *body, long  b_len);
int prepare_order(int nums, int *text, int *attach);







