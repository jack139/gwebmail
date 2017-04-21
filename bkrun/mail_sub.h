/* mail_sub.h */

#ifndef _MAIL_SUB_H
#define _MAIL_SUB_H


#define POP3_HOST "localhost"
#define SMTP_HOST POP3_HOST

/* block size of every reading and writing */
#define BLOCKSIZE 1024

/* reading timeout */
#define TIMEOUT 10

#define SOCKSIZE BLOCKSIZE*4

extern char sock_buf[SOCKSIZE+1];

#define close_sock close
#define close_SMTP close
#define close_POP3 close

#define init_POP3(h) init_sock(h,"110")
#define init_SMTP(h) init_sock(h,"25")


int connectTCP(const char *host, const char *service);

/* defined in mail.c */
int init_sock(const char *host, const char *port);

long read_respose(int s, char *buf, long sz, const char *end_tag);
int write_command(int s, const char *buf, long sz);


char *get_a_part(const char *src, 
		const char *bg_tag, const char *end_tag, long *length);
char *get_a_bit_part(const unsigned char *src,     size_t src_l,
		     const unsigned char *bg_tag,  size_t bg_l,
		     const unsigned char *end_tag, size_t end_l,
		     long *length);
char *filt_str(char *str);

#define D_SPC_AND_CRLF 1
#define D_SPC          2
#define D_CRLF         3

long giveup_spc(char *s, int flag);


#endif











