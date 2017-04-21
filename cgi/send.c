/* send.c */

#include <stdio.h>
#include <time.h>
#include "cgic.h"
#include "db.h"
#include "mail_sub.h"
#include "upload.h"
#include "lang.h"

char sign[1024*2];

int load_sign();
int send();

int cgiMain() {
  char uid[MAXID+1],des[40];


  cgiHeaderContentType("text/html");
  fprintf(cgiOut, "<html><head>\n");
  fprintf(cgiOut,"<meta http-equiv=Content-Type "
	  "content=\"text/html; charset=gb2312\">\n");
  fprintf(cgiOut, "<title>send</title></head>\n");

  load_sign();

  if (send()==0)
    {
      fprintf(cgiOut, "<body bgcolor=#99c8ff text=#0033cc>\n");

      fprintf(cgiOut, "<p align=center>　</p>\n");
      fprintf(cgiOut, "<p align=center>　</p>\n");
      fprintf(cgiOut, "<p align=center>　</p>\n");

      fprintf(cgiOut, "<p align=center><b>"M_SENDGO"</b><p>\n");

      fprintf(cgiOut, "<p align=center>　</p>\n"); 

      fprintf(cgiOut, "</body></html>\n");
    }
  else
    {
    eee:
      fprintf(cgiOut, "<body bgcolor=#99c8ff text=#0033cc>\n");

      fprintf(cgiOut, "<p align=center>　</p>\n");
      fprintf(cgiOut, "<p align=center><font size=6><b>"
	      M_SYSINFO"</b></font></p>\n");

      fprintf(cgiOut, "<p align=center>　</p>\n");
      fprintf(cgiOut, "<p align=center>　</p>\n");

      fprintf(cgiOut, "<p align=center><font color=#FF0000>"
	      "%s</font></p>\n", g_info);

      fprintf(cgiOut, "<p align=center>　</p>\n"); 

      fprintf(cgiOut, "</body></html>\n");
    }

  return 0;
}


/*---------------------------------------
 send(): send
 ret:                               
   0       success                     
   -1      fail
----------------------------------------*/
#define F_VALUE(x) form_e[x].value
#define F_LEN(x) form_e[x].length

int send()
{
  char uid[MAXID+1],des[40];
  char send[101],recv[101],subj[201];
  char copyto[201];
  char *p, temp[500];
  int result, sock, save;
  int ret=-1;
  int length;
  int all_e, this_e, attach;
  time_t tt;
  char my_mid[60];

  time(&tt);
  my_md5(uid, tt, my_mid);

  all_e=upload();
  if (all_e==-1)  return -1;

  result=get_form_value(all_e, "uid", uid, MAXID+1);
  if (result==-1)
    {
      warn(W_CGI_ERR);
      free(upload_buf);
      return -1;
    }

  result=get_form_value(all_e, "des", des, 35);
  if (result==-1)
    {
      warn(W_CGI_ERR);
      free(upload_buf);
      return -1;
    }

  if (db_flash_active(uid,des)==-1) return -1;

  result=get_form_value(all_e, "send", send, 101);
  if (result==-1)
    {
      warn(W_CGI_ERR);
      free(upload_buf);
      return -1;
    }

  result=get_form_value(all_e, "recv", recv, 101);
  if (result==-1)
    {
      warn(W_CGI_ERR);
      free(upload_buf);
      return -1;
    }
  if (strlen(recv)==0)
    {
      warn(W_NO_RECV);
      return -1;
    }


  result=get_form_entry(all_e, "save");
  if (result==-1) save=0;
  else save=1;

  result=get_form_value(all_e, "copyto", copyto, 201);
  if (result==-1)
    {
      warn(W_CGI_ERR);
      free(upload_buf);
      return -1;
    }

  result=get_form_value(all_e, "subject", subj, 201);
  if (result==-1)
    {
      warn(W_CGI_ERR);
      free(upload_buf);
      return -1;
    }

  result=get_form_entry(all_e, "body");
  if (result==-1)
    {
      warn(W_CGI_ERR);
      free(upload_buf);
      return -1;
    }

  length=F_LEN(result);
  p=(char *)malloc(length+1024);
  if (p==NULL)
    {
      warn(W_NO_MEM);
      free(upload_buf);
      return -1;
    }

  result=get_form_value(all_e, "body", p, length+1024);
  if (result==-1)
    {
      warn(W_CGI_ERR);
      free(p);
      free(upload_buf);
      return -1;
    }

  this_e=get_form_entry(all_e, "upfile");
  if (this_e==-1)
    {
      warn(W_CGI_ERR);
      free(upload_buf);
      free(p);
      return -1;
    }

  if (F_LEN(this_e)==0) attach=0;
  else attach=1;
  

  /* ready to send mail by SMTP */

  sock=init_SMTP(SMTP_HOST);
  if (sock==-1)
    {
      warn(W_MSR_ERR);
      free(upload_buf);
      free(p);
      return -1;
    }
  
  read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
  if (strncmp(sock_buf, "220", 3)!=0)
    {
      warn (W_MSR_ERR);
      close_SMTP(sock);
      free(upload_buf);
      free(p);
      return -1;
    }
  
  sprintf(temp, "HELO "MAIL_SVR"\r\n");
  write_command(sock, temp, strlen(temp));

  read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
  if (strncmp(sock_buf, "250", 3)!=0)
    {
      warn(W_MSR_ERR);
      goto quit_smtp;
    }

  sprintf(temp, "MAIL FROM: <%s>\r\n", send);
  write_command(sock, temp, strlen(temp));

  read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
  if (strncmp(sock_buf, "250", 3)!=0)
    {
      warn(W_MSR_ERR);
      goto quit_smtp;
    }

  sprintf(temp, "RCPT TO: <%s>\r\n", recv);
  write_command(sock, temp, strlen(temp));

  read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
  if (strncmp(sock_buf, "250", 3)!=0)
    {
      warn(W_MSR_ERR);
      goto quit_smtp;

    }
  
  sprintf(temp, "DATA\r\n");
  write_command(sock, temp, strlen(temp));
  
  read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
  if (strncmp(sock_buf, "354", 3)!=0)
    {
      warn(W_MSR_ERR);
      goto quit_smtp;
    }

  sprintf(temp, "Message-ID: <%s@"MAIL_SVR">\r\n", my_mid);
  write_command(sock, temp, strlen(temp));

  sprintf(temp, "From: <%s>\r\n", send);
  write_command(sock, temp, strlen(temp));

  sprintf(temp, "To: <%s>\r\n", recv);
  write_command(sock, temp, strlen(temp));

  if (copyto[0])
    {
      sprintf(temp, "Cc: <%s>\r\n", copyto);
      write_command(sock, temp, strlen(temp));
    }

  sprintf(temp, "Subject: %s\r\n", subj);
  write_command(sock, temp, strlen(temp));

  sprintf(temp, "MIME-Version: 1.0\r\n");
  write_command(sock, temp, strlen(temp));

  if (attach==0)
    {
      // send a mail in plain text

      sprintf(temp, "Content-Type: text/plain;\r\n\tcharset=\"gb2312\"\r\n"
	      "Content-Transfer-Encoding: 8bit\r\n\r\n");
      write_command(sock, temp, strlen(temp));

      write_command(sock, p, length);

      write_command(sock, "\r\n", 2);
      write_command(sock, sign, strlen(sign));

      sprintf(temp, "\r\n.\r\n");
      write_command(sock, temp, strlen(temp));

      read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
      if (strncmp(sock_buf, "250", 3)!=0)
	{
	  warn(W_MSR_ERR);
	  goto quit_smtp;
	}
    }
  else
    {
      //  sent a mail with attachment
      
      char bound[100];
      long ii, poff;
      int tlen;
      char *pbuf;
 
      sprintf(bound,"----=_NextPart_%s", my_mid);

      // main header      
      sprintf(temp, "Content-Type: multipart/mixed;\r\n\t"
	      "boundary=\"%s\"\r\n", bound);
      write_command(sock, temp, strlen(temp));

      sprintf(temp, "\r\nThis is a multi-part message"
	      " in MIME format.\r\n\r\n");
      write_command(sock, temp, strlen(temp));

      // first part: plain text
      sprintf(temp, "--%s\r\n", bound);
      write_command(sock, temp, strlen(temp));

      sprintf(temp, "Content-Type: text/plain;\r\n\tcharset=\"gb2312\"\r\n");
      write_command(sock, temp, strlen(temp));

      sprintf(temp, "Content-Transfer-Encoding: 8bit\r\n\r\n");
      write_command(sock, temp, strlen(temp));

      write_command(sock, p, length);

      write_command(sock, "\r\n", 2);
      write_command(sock, sign, strlen(sign));

      sprintf(temp, "\r\n\r\n");
      write_command(sock, temp, strlen(temp));

      // second part: file
      sprintf(temp, "--%s\r\n", bound);
      write_command(sock, temp, strlen(temp));

      sprintf(temp,"Content-Type: application/x-file;\r\n\t"
	      "name=\"%s\"\r\n", form_e[this_e].file_n);
      write_command(sock, temp, strlen(temp));

      sprintf(temp, "Content-Transfer-Encoding: base64\r\n"
	      "Content-Disposition: attachment;\r\n\t"
	      "filename=\"%s\"\r\n\r\n", form_e[this_e].file_n);
      write_command(sock, temp, strlen(temp));

      // file content
      
      pbuf=(char *)malloc(F_LEN(this_e)/57*78+1024);
      if (pbuf==NULL)
	{
	  warn("no enough mem!");
	  goto quit_smtp;
	}

 
      poff=0L;

      for(ii=0L; ii<F_LEN(this_e); ii+=57)
	{
	  int blen;

	  if (F_LEN(this_e)-ii>57) tlen=57;
	  else tlen=F_LEN(this_e)-ii;

	  memcpy(temp, F_VALUE(this_e)+ii, tlen);
	  b64_encode(temp, tlen, pbuf+poff);

	  blen=tlen/3*4+((tlen%3==0)?0:4);
	  poff+=blen;

	  memcpy(pbuf+poff, "\r\n", 2);
	  poff+=2;

	}

	  
      // file end

      write_command(sock, pbuf, poff);      

      free(pbuf);

      sprintf(temp, "\r\n--%s--\r\n\r\n", bound);
      write_command(sock, temp, strlen(temp));

      write_command(sock, "\r\n.\r\n", 5);

      read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
      if (strncmp(sock_buf, "250", 3)!=0)
        {
	  printf("%s", sock_buf);
          warn(W_MSR_ERR);
          goto quit_smtp;
        }

    }


  if (save==1)
    {
      sprintf(temp,
	      "insert into mail"
	      " (uid,folder_n,mail_id,sender,recv,tm,title,size,unread,"
	      " content) values ('%s','"SENT_NAME"','%s','%s','%s',"
	      "now(),'%s',%ld,'%1d', '%s')",
	      uid, my_mid,
	      db_prepare(send),
	      db_prepare(recv),
	      db_prepare(subj),
	      length+1024, 0, p);
      if (db_exec(temp, 0)==-1)
	{
	  warn("dataserver fail!(insert mail");
	  goto quit_smtp;
	}

      sprintf(temp,
	      "update mail set content=concat('"
	      "Date: %s"
	      "From: <%s>\r\n"
	      "To: <%s>\r\n"
	      "Subject: %s\r\n"
	      "MIME-Version: 1.0\r\n"
	      "Content-Type: text/plain;\r\n\t\"charset=gb2312\"\r\n"
	      "Content-Transfer-Encoding: 8bit\r\n\r\n"
	      "', content, '\r\n.\r\n') where mail_id='%s' and uid='%s'",
	      ctime(&tt),
              db_prepare(send),
              db_prepare(recv),
              db_prepare(subj),
	      my_mid,
	      uid
	      );
      if (db_exec(temp, 0)==-1)
        {
          warn("dataserver fail!(insert mail");
          goto quit_smtp;
        }

      sprintf(temp, "update folder set mails=mails+1, "
	      "size=size+%ld where uid='%s' and folder_n='%s'",
	      length+1024, uid, SENT_NAME);
      if (db_exec(temp, 0)==-1)
	{
	  warn("dataserver fail!(move 8)");
	  goto quit_smtp;
	}


    }


  ret=0;

 quit_smtp:

  free(p);

  free(upload_buf);
  
  sprintf(temp, "QUIT\r\n");
  write_command(sock, temp, strlen(temp));

  read_respose(sock, sock_buf, SOCKSIZE, "\r\n");
  if (strncmp(sock_buf, "221", 3)!=0) return -1;

  return ret;

}


/*
  load signature into memory
 */
int load_sign()
{
  FILE *fp;
  int len=0;
  char *p=sign;
  
  sign[0]='\0';

  fp=fopen(CONFIG_PATH"/signature", "r");
  if (fp==NULL) return -1;

  while(fgets(p, 2048-len, fp))
    {
      len=strlen(sign);
      p=sign+len;
    }

  fclose(fp);
      
  return 0;
}



