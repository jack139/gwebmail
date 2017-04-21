/* upload.c */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "upload.h"
#include "mail_sub.h"
#include "cgic.h"

//#define DEBUG_IT

form_list form_e[MAX_FORM];
unsigned char *upload_buf;


/*---------------------------------------
  upload():  
 ret:                               
   0       num of forms
   -1      fail
----------------------------------------*/
int upload()
{
  char bound[300];
  long content_length;
  unsigned char *p, *p2, *tp;
  char *ptr;
  long t_len, tt_len, lll;
  int now_e, all_e, i, bl;


  if (strncasecmp(cgiContentType, 
		  UPLOAD_MIME_TYPE, strlen(UPLOAD_MIME_TYPE))!=0)
    {
      warn("not a upload!");
      return -1;
    }

  cgiGetenv(&ptr, "CONTENT_LENGTH");
  content_length = atol(ptr);

  if (!content_length)
    {
      warn("no content!");
      return -1;
    }

  if (content_length>MAX_UPLOAD)
    {
      warn("上载的文件太大! 上载 的文件不能超过4M.");
      return -1;
    }


  upload_buf=(unsigned char *)malloc(cgiContentLength+1);
  if (upload_buf==NULL) 
    {
      warn("no mem!");
      return -1;
    }

  if (fread(upload_buf, 1, cgiContentLength, cgiIn)!=cgiContentLength)
    {
      warn("read error");
      free(upload_buf);
      return -1;
    }

  ptr=strstr(cgiContentType, "boundary=");
  if (ptr==NULL) 
    {
      warn("cannot found boundary!");
      free(upload_buf);
      return -1;
    }

  strcpy(bound, "--");
  strcat(bound, ptr+9);

  p=upload_buf;
  lll=content_length;
  bl=strlen(bound);
  now_e=0;

  while(p2=get_a_bit_part(p, lll, bound, bl, bound, bl, &tt_len))
    {
      p=p2+tt_len;

      lll=content_length-(p-upload_buf);

      tt_len+=bl;

      tp=get_a_bit_part(p2, tt_len, "name=\"", 6, "\"", 1, &t_len);
      if (tp==NULL) continue;
      t_len-=6;
      if (t_len>100) t_len=100;
      memcpy(form_e[now_e].name, tp+6, t_len);
      form_e[now_e].name[t_len]='\0';

      tp=get_a_bit_part(p2, tt_len, "filename=\"", 10, "\"", 1, &t_len);
      if (tp==NULL) form_e[now_e].file_n[0]='\0';
      else
	{
	  char *pi;

	  t_len-=10;
	  if (t_len>100) t_len=100;
	  memcpy(form_e[now_e].file_n, tp+10, t_len);
	  form_e[now_e].file_n[t_len]='\0';
	  for (pi=form_e[now_e].file_n+t_len;
	       pi>=form_e[now_e].file_n && *pi!='\\' && *pi!='/';
	       pi--);
	  if (pi!=form_e[now_e].file_n)
	    strcpy(form_e[now_e].file_n, pi+1);
	  
	}

      tp=get_a_bit_part(p2, tt_len, "\r\n\r\n", 4, bound, bl, &t_len);
      if (tp==NULL) form_e[now_e].value=NULL;
      else
	{
	  t_len-=(4+2);  //  add CRLF at part-end
	  form_e[now_e].value=tp+4;
	  form_e[now_e].length=t_len;
	}

      now_e++;
    }
	      
  all_e=now_e;

#ifdef DEBUG_IT

  fprintf(cgiOut, "%s<br>%d<br>", cgiContentType, cgiContentLength);
  fprintf(cgiOut, "%s<br>", upload_buf);

  for(i=0; i<all_e; i++)
    {
      fprintf(cgiOut, "<br>name=%s<br>", form_e[i].name);
      fprintf(cgiOut, "filename=%s<br>", form_e[i].file_n);
      fprintf(cgiOut, "value_addr=%x length=%ld<br>", 
	      form_e[i].value, form_e[i].length);

      if (form_e[i].file_n[0]=='\0')
	{
	  *(form_e[i].value+form_e[i].length)='\0';
	  fprintf(cgiOut, "value=%s<br>", form_e[i].value);
	}
    }
	

  fprintf(cgiOut, "<p>done.</p></body></html>\n");

  free(upload_buf);

#endif



  return all_e;
}


/*---------------------------------------------------------------
  get_form_entry()
----------------------------------------------------------------*/
int get_form_entry(int all_e, char *name)
{
  int i;

  for (i=0; i<all_e; i++)
    {
      if (strcasecmp(form_e[i].name, name)==0) return i;
    }

  return -1;
}

/*---------------------------------------------------------------
  get_form_value()
---------------------------------------------------------------*/
int get_form_value(int all_e, char *name, char *value, size_t len)
{
  int i;
  size_t tt_len;

  i=get_form_entry(all_e, name);
  if (i==-1) return -1;

  if (form_e[i].length>=len) tt_len=len-1;
  else tt_len=form_e[i].length;

  memcpy(value, form_e[i].value, tt_len);
  value[tt_len]='\0';
  
  return i;

}






