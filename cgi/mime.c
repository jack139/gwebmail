/* mime.c */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "mail_sub.h"
#include "mime.h"


mime_entry mime_e[20];


/*-------------------------------------------------
  set_entry()
--------------------------------------------------*/
int set_entry(char *body, long b_len, int now_e)
{
  long tt;
  char *p, *r, *ed, bk_char;

  p=body;

  ed=strstr(p, "\r\n\r\n");

  /*    cut mime head  */
  ed+=3;
  bk_char=*ed;
  *ed='\0';

  mime_e[now_e].content_x[0]='\0';

  mime_e[now_e].bg=ed+1;
  mime_e[now_e].len=b_len-(ed+1-p);

  r=get_a_part(p, "Content-Type:", "\r\n", &tt);
  if (r!=NULL) goto do_proc_type;

  // added that to process "Content-type" 
  // because FoxMail 3.1 beta 1
  r=get_a_part(p, "Content-type:", "\r\n", &tt);
  if (r==NULL) 
    {
      mime_e[now_e].content_type=TE_PLAIN; 
      goto skip_proc_type;
    }

 do_proc_type:
  //  analyse Content-Type
  r+=13;  //  length of "Content-Type"
      
  for(; *r==' '; r++);  // kill space
      
  if (strncasecmp(r, "multipart", 9)==0)
    {
      char *tp;
      long tt_len;

      if (strncasecmp(r+10, "related", 7)==0)
	mime_e[now_e].content_type=MU_RELAT;
      else if (strncasecmp(r+10, "alternative", 11)==0)
	mime_e[now_e].content_type=MU_ALTER;
      else 
	mime_e[now_e].content_type=MU_MIXED;
      
      tp=get_a_part(r, "boundary=\"", "\"", &tt_len);
      if (tp!=NULL) goto do_proc_bound;

      // added that to process "BOUNDARY" 
      // because FoxMail 3.1 beta 1
      tp=get_a_part(r, "BOUNDARY=\"", "\"", &tt_len);
      if (tp==NULL)
	{
	  mime_e[now_e].content_type=TE_PLAIN;
	  goto skip_proc_bound;
	}

    do_proc_bound:
      //  get boundary !!!

      tt_len-=10;
      if (tt_len>200) tt_len=200;
      strcpy(mime_e[now_e].content_x, "--");
      memcpy(mime_e[now_e].content_x+2, tp+10, tt_len);
      mime_e[now_e].content_x[tt_len+2]='\0';
	  
    skip_proc_bound:
      
    }
  else if (strncasecmp(r, "text", 4)==0)
    {
      if (strncasecmp(r+5, "html", 4)==0)
	mime_e[now_e].content_type=TE_HTML;
      else
	mime_e[now_e].content_type=TE_PLAIN;
    }
  else if (strncasecmp(r, "image", 5)==0)
    {
      if (strncasecmp(r+6, "jpeg", 4)==0)
	mime_e[now_e].content_type=IM_JPG;
      else if (strncasecmp(r+6, "gif", 3)==0)
	mime_e[now_e].content_type=IM_GIF;
      else
	mime_e[now_e].content_type=AP_APP;
    }
  else if (strncasecmp(r, "application", 11)==0)
    {
      char *tp;
      long tt_len;
      long tmp_len;

      tp=get_a_part(r, "name=\"", "\"", &tt_len);
      if (tp!=NULL){ tmp_len=6; goto do_process_name; }

      // added that to process name without " " 
      // because FoxMail 3.1 beta 1
      tp=get_a_part(r, "name=", "\r\n", &tt_len);
      if (tp==NULL)
	{
	  strcpy(mime_e[now_e].content_x, "unknown.dat");
	  goto skip_proc_name;
	}

      tmp_len=5;

    do_process_name:

      tt_len-=tmp_len;
      if (tt_len>200) tt_len=200;
      memcpy(mime_e[now_e].content_x, tp+tmp_len, tt_len);
      mime_e[now_e].content_x[tt_len]='\0';

    skip_proc_name:

      mime_e[now_e].content_type=AP_APP;
    }
  else mime_e[now_e].content_type=AP_APP;
 
 skip_proc_type:
  
  /*  not check content-id for multipart header */
  if ((mime_e[now_e].content_type & C_MULTI)==0 &&
      (mime_e[now_e].content_type & C_APP)==0 )
    {
      r=get_a_part(p, "Content-ID:", "\r\n", &tt);
      if (r==NULL) mime_e[now_e].content_x[0]='\0';
      else
	{
	  char *tr;
	  long tt_len;

	  tr=get_a_part(r, "<", ">", &tt_len);
	  if (tr==NULL) mime_e[now_e].content_x[0]='\0';
	  else  //  get Content-ID !!!
	    {
	      tt_len--;
	      if (tt_len>200) tt_len=200;
	      memcpy(mime_e[now_e].content_x, tr+1, tt_len);
	      mime_e[now_e].content_x[tt_len]='\0';
	    }
	}
    }

  
  /*  analyse Content-Transfer-Encoding  */
  r=get_a_part(p, "Content-Transfer-Encoding:", "\r\n", &tt);
  if (r!=NULL) goto do_proc_encode;

  // add this because FoxMail 3.1 beta 1
  r=get_a_part(p, "Content-transfer-encoding:", "\r\n", &tt);
  if (r==NULL)
    {
      mime_e[now_e].transfer_encoding=CODE_7BIT;
      goto skip_proc_encode;
    }

 do_proc_encode:

  r+=26;
  for(;*r==' ';r++);

  if (strncasecmp(r, "base64", 6)==0)
    mime_e[now_e].transfer_encoding=CODE_B64;
  else if (strncasecmp(r, "quoted-printable", 16)==0)
    mime_e[now_e].transfer_encoding=CODE_QP;
  else
    mime_e[now_e].transfer_encoding=CODE_7BIT;

 skip_proc_encode:

  *ed=bk_char;

  return 0;

}


/*-----------------------------------------------------
  prepare_entry()
------------------------------------------------------*/
int prepare_entry(char *body, long b_len)
{
  char *now_p, *tp, *bd;
  int now_e, papa_e;
  long tl;


  set_entry(body, b_len, 0);
  mime_e[0].face=0;
  mime_e[0].papa=-1;

  //  return 1;

  now_e=1;
  papa_e=0;

  while(papa_e<now_e)
    {
      now_p=mime_e[papa_e].bg;
 
      if ((mime_e[papa_e].content_type & C_MULTI)==0)
	{
	  papa_e++;
	  continue;
	}
    
      bd=mime_e[papa_e].content_x;
      
      tp=get_a_part(now_p, bd, bd, &tl);

      for(;;)
	{
	  tp=get_a_part(now_p, bd, bd, &tl);
	  if (tp==NULL) break;

	  now_p=tp+tl;

	  set_entry(tp, tl, now_e);
	  mime_e[now_e].face=mime_e[papa_e].face+1;
	  mime_e[now_e].papa=papa_e;

	  now_e++;

	}

      papa_e++;
    }

  return now_e;
}


/*-------------------------------------------
  prepare()
--------------------------------------------*/
int prepare_order(int nums, int *text, int *attach)
{
  int i, now_face;

  *text=*attach=-1;
  for (i=nums-1; i>=0; i--)
    {
      now_face=mime_e[i].face;

      if (mime_e[i].content_type & C_MULTI) continue;

      if ((mime_e[mime_e[i].papa].content_type == MU_RELAT)
	  && strlen(mime_e[i].content_x)) continue;

      if (mime_e[i].content_type & C_TEXT)
	{
	  mime_e[i].next=*text;
	  *text=i;
	}
      else
	{
	  mime_e[i].next=*attach;
	  *attach=i;
	}

      if (mime_e[mime_e[i].papa].content_type == MU_ALTER)
	for (; (mime_e[i-1].face)==now_face && i>=0; i--);

    }

  return 0;
}
  






