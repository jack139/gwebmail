/* MD5.C  */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "global.h"
#include "md5.h"

static void MDString(char *, char *);

#define MD_CTX MD5_CTX
#define MDInit MD5Init
#define MDUpdate MD5Update
#define MDFinal MD5Final

void my_md5(const char *uid, time_t tt, char *des)
{
  char tmp[100];

  sprintf(tmp, "%s%08X", uid, tt);
  MDString(tmp, des);
}

void my_md5_str(const char *s, char *des)
{
  char tmp[101];
  int ll;

  ll=strlen(s);
  if (ll>100) ll=100;
  memcpy(tmp, s, ll);
  tmp[ll]='\0';  
  MDString(tmp, des);
}

static void MDString (char *string, char *des)
{
  MD_CTX context;
  unsigned char digest[16];
  unsigned int len = strlen (string);
  int i;
  unsigned char tmp[10];

  MDInit (&context);
  MDUpdate (&context, string, len);
  MDFinal (digest, &context);

  for(i=0, des[0]='\0'; i<16; i++)
      sprintf(des+i+i, "%02x", digest[i]);
}









