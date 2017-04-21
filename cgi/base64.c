/* base64.c */

#include <stdlib.h>
#include <stdio.h>


/*
  base64
*/
static void fromb64alp(int *ibuf)  
{
  if(*ibuf<26)
    {
      *ibuf=*ibuf+65;
    }
  else if(*ibuf<52)
    {
      *ibuf=*ibuf+71;
    }
  else if(*ibuf<62)
    {
      *ibuf=*ibuf+(-4);
    }
  else if(*ibuf==62)
    {
      *ibuf=*ibuf+(-19);
    }
  else if(*ibuf==63) 
    {
      *ibuf=*ibuf+(-16);
    }
}

static int tob64alp(int *ibuf)  
{
  if(*ibuf>=(0x61)&&*ibuf<=(0x7a))  /* downcase */
    {
      *ibuf=*ibuf-71;
    }
  else if(*ibuf>=(0x41)&&*ibuf<=(0x5a))  /* upcase */
    {
      *ibuf=*ibuf-65;
    }
  else if(*ibuf>=(0x30)&&*ibuf<=(0x39)) /* num */
    {
      *ibuf=*ibuf-(-4);
    }
  else if(*ibuf==(0x2b)) /* '+' */
    {
      *ibuf=*ibuf-(-19);
    }
  else if(*ibuf==(0x2f)) /* '/' */
    {
      *ibuf=*ibuf-(-16);
    }
  else
    {
      return -1;
    }
  return 0;
}

void b64_encode(unsigned char *instr,long len, char *outstr)
{
  long i,i2,j;
  int ibuffer,itemp,ibuf1,ibuf2,ibuf3,ibuf4;
  int eq=0;

  for(i=0,i2=0;i<len;i+=3,i2+=4)
    {
      ibuffer=0;
      for(j=0;j<3;j++)
	{
	  ibuffer=ibuffer<<8;
	  if (i+j>=len)
	    {
	      eq++;
	      itemp=0;
	    }
	  else
	    itemp=(int)(*(instr+i+j));
	  itemp=itemp&255;
	  ibuffer=ibuffer|itemp;
	}
      ibuffer=ibuffer&(16777215);
      ibuf1=(ibuffer>>18)&63;
      ibuf2=(ibuffer>>12)&63;
      ibuf3=(ibuffer>>6)&63;
      ibuf4=(ibuffer)&63;
      fromb64alp(&ibuf1);
      fromb64alp(&ibuf2);
      fromb64alp(&ibuf3);
      fromb64alp(&ibuf4);
      *(outstr+i2+0)=(char)(ibuf1);
      *(outstr+i2+1)=(char)(ibuf2);
      *(outstr+i2+2)=(char)(ibuf3);
      *(outstr+i2+3)=(char)(ibuf4);
    }
  
  for(i=0; i<eq; i++) *(outstr+i2-1-i)='=';

  *(outstr+i2)=0;
}

long b64_decode(char *instr, long len, unsigned char *outstr)
{
  int ibuf=0, ibuf1=0, ibuf2=0, ibuf3=0, eq=0;
  long i, i2, j;

  for(i=0, i2=0; i<len; i+=4, i2+=3)
    {
      ibuf=0;
      for(j=0; j<4; j++)
	{
	  int itemp;

	  if (*(instr+i+j)=='=') eq++;
	  ibuf=ibuf<<6;
	  itemp=(int)(*(instr+i+j));
	  if(tob64alp(&itemp)==-1)	continue;
	  itemp=itemp&255;
	  ibuf=ibuf|itemp;
	}
      ibuf1=(ibuf>>16)&255;
      ibuf2=(ibuf>>8)&255;
      ibuf3=(ibuf)&255;
      *(outstr+i2+0)=(unsigned char)(ibuf1);
      *(outstr+i2+1)=(unsigned char)(ibuf2);
      *(outstr+i2+2)=(unsigned char)(ibuf3);
    }
  *(outstr+i2)=0;

  if (eq>3) eq=3;

  return (i2-3)+(3-eq);
}

/*
  quoted-printable
*/
static int astoint(char *str)
{
  int ibuf1,ibuf2;
  *(str+2)=0;
  if((int)*str>=0x30 && (int)*str<=0x39)
    {
      ibuf1=(int)*str-0x30;
    }
  else if((int)*str>=0x41 && (int)*str<=0X46)
    {
      ibuf1=(int)*str-0x37;
    }
  if((int)*(str+1)>=0x30 && (int)*(str+1)<=0x39)
    {
      ibuf2=(int)*(str+1)-0x30;
    }
  else if((int)*(str+1)>=0x41 && (int)*(str+1)<=0X46)
    {
      ibuf2=(int)*(str+1)-0x37;
    }
  return (ibuf1*16+ibuf2);
}

void qp_decode(char *instr,long len,unsigned char *outstr)
{
  long	i,i1;
  int ibuf;
  char	stemp[5];
  for(i=0,i1=0;i<len;)
    {
      if(*(instr+i)=='=')
	{
	  if(*(instr+i+1)!='\r')
	    {
	      stemp[0]=0;
	      strncpy(stemp,(instr+i+1),2);
	      stemp[2]=0;
	      ibuf=astoint(stemp);
	      *(outstr+i1)=(unsigned char)ibuf;
	      i+=3;
	      i1++;
	    }
	  else
	    {
	      *(outstr+i1)=(unsigned char) *(instr+i+1);
	      i+=2;
	      i1++;
	    }
	  continue;
	}
      *(outstr+i1)=(unsigned char) *(instr+i);
      i++;
      i1++;
    }
  *(outstr+i1)=0;
}





