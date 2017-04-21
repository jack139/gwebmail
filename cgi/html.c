
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#include "cgic.h"
#include "html.h"

#define HTML_PMARK_BEGIN				"$"
#define HTML_PMARK_END				"$"

#define HTML_PMARK_BEGIN_FILLER	"<%"
#define HTML_PMARK_END_FILLER		"%>"

char *html_Load(char *sPath, char *sFileName)
{
  FILE *fp;
  char *pBuffer;
  long size, output_size;
  char html_file[200];
	
  strcpy(html_file, sPath);
  strcat(html_file, sFileName);

  if (html_file[0] != '/') {
    // html_file must start with '/'
    return NULL;
  }

  if ((fp = fopen(html_file, "r")) == NULL) {
    // html_Load():fopen fail
    return NULL;
  }

  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  pBuffer = malloc(size+1);
  fread(pBuffer, size, 1, fp);
	
  pBuffer[size] = 0;
  
  fclose(fp);
  return pBuffer;
}

int html_File(char *pBuffer, char *start_token, char *end_token)
{
  long size, output_size;
  char *temp, *p1, *p2;

  if (strlen(start_token) == 0) 
    p1 = pBuffer;
  else {
    p1 = strstr(pBuffer, start_token);
    if (p1 == NULL) {
      // start_token not found in html file
      return -1;
    }
    p1 = p1 + strlen(start_token);
  }

  if (strlen(end_token) == 0) 
    p2 = pBuffer + strlen(pBuffer) - 1;
  else {
    p2 = strstr(pBuffer, end_token);
    if (p2 == NULL) {
      // end_token not found in html file
      return -1;
    }
  }
  
  if (p1 >= p2) {
    // start_token must before end_token
    return -1;
  }
  
  output_size = p2 - p1;
  temp = malloc(output_size+1);
  strncpy(temp, p1, output_size);
  temp[output_size] = '\0';
  fprintf(cgiOut, "%s", temp);
  free(temp);

  return 0;
}

void html_Free(char *pBuffer)
{
  free(pBuffer);
}

////////////////////////////////////////////////////////////////////////////////////

char *html_GetTemplate(char *sBuffer, char *sMarkBegin, char *sMarkEnd)
{
  long lSize;
  char *sTemplate, *pcBegin, *pcEnd;
  
  if (strlen(sMarkBegin) == 0)
    {
      pcBegin = sBuffer;
    }
  else
    {
      pcBegin = strstr(sBuffer, sMarkBegin);
      if (pcBegin == NULL)
	{
	  // start_token not found in html file
	  return NULL;
	}
      pcBegin = pcBegin + strlen(sMarkBegin);
    }

  if (strlen(sMarkEnd) == 0)
    {
      pcEnd = sBuffer + strlen(sBuffer);
    }
  else
    {
      pcEnd = strstr(sBuffer, sMarkEnd);
      if (pcEnd == NULL)
	{
	  // end_token not found in html file
	  return NULL;
	}
    }
  
  if (pcBegin >= pcEnd)
    {
      // start_token must before end_token
      return NULL;
    }
  
  lSize = pcEnd - pcBegin;
  sTemplate = malloc(lSize+1);
  strncpy(sTemplate, pcBegin, lSize);
  sTemplate[lSize] = '\0';
  return sTemplate;
}

void html_FillTemplate(char *sTemplate, ...)
{
  char *pcOutputBegin, *pcOutputEnd;
  char *pcMarkBegin, *pcParamBegin, *pcParamEnd;
  char cTemp;
  char *sParam, *sVal;
  va_list ap;
  
  pcOutputBegin = sTemplate;
  while(1)
    {
      if (*pcOutputBegin == 0)
	{
	  return;
	}
		
      pcMarkBegin = strstr(pcOutputBegin, HTML_PMARK_BEGIN);
      if (pcMarkBegin == NULL)
	{
	  pcOutputEnd = pcOutputBegin + strlen(pcOutputBegin);
	}
      else
	{
	  pcOutputEnd = pcMarkBegin;
	  if (pcMarkBegin-pcOutputBegin >= strlen(HTML_PMARK_BEGIN_FILLER))
	    {
	      if (strncmp(pcMarkBegin-strlen(HTML_PMARK_BEGIN_FILLER), 
			  HTML_PMARK_BEGIN_FILLER,
			  strlen(HTML_PMARK_BEGIN_FILLER)) == 0)
		{
		  pcOutputEnd = pcMarkBegin-strlen(HTML_PMARK_BEGIN_FILLER);
		}
	    }
	}
      
      cTemp = *pcOutputEnd;
      *pcOutputEnd = 0;
      fprintf(cgiOut, "%s", pcOutputBegin);
      *pcOutputEnd = cTemp;
      
      if (*pcOutputEnd == 0)
	{
	  return;
	}
		
      pcParamBegin = pcMarkBegin + strlen(HTML_PMARK_BEGIN);
      pcParamEnd = strstr(pcParamBegin, HTML_PMARK_END);
      if (pcParamEnd == NULL)
	{
	  fprintf(cgiOut,"html_FillTemplate():HTML_PMARK_END not found\n");
	  return;
	}
      
      va_start(ap, sTemplate);
      sParam = va_arg(ap, char *);
      while (sParam != NULL)
	{
	  sVal = va_arg(ap, char *);
	  if (strncmp(sParam, pcParamBegin, pcParamEnd-pcParamBegin) == 0)
	    {
	      fprintf(cgiOut, "%s", sVal);
	      //				vprintf(sVal, ap);
	      break;
	    }
	  sParam = va_arg(ap, char *);
	}
      va_end(ap);
      
      pcOutputBegin = pcParamEnd + strlen(HTML_PMARK_END);
      if (strncmp(pcOutputBegin, HTML_PMARK_END_FILLER, 
		  strlen(HTML_PMARK_END_FILLER))==0)
	{
	  pcOutputBegin = pcOutputBegin + strlen(HTML_PMARK_END_FILLER);
	}
    }
  return;
}

//----------------------------------------------------------------

char *html_ConvertToHTML(char *s)
{
  char *p, *p2, *p3;
  int i=0;
  
  p=(char *)malloc(strlen(s)+100);
  if (p==NULL)
    {
      return NULL;
    }

  strcpy(p, s);

  for(p2=p, p3=s; *p2; p2++,p3++)
    {
      switch(*p2)
	{
	case '<':
	  strcpy(p3, "&lt;"); p3+=3;  break;

	case '>':
	  strcpy(p3, "&gt;"); p3+=3; break;

	case '"':
	  strcpy(p3, "&quot;"); p3+=5; break;

	case '&':
	  strcpy(p3, "&amp;"); p3+=4; break;

	case ' ':
	  strcpy(p3, "&nbsp;"); p3+=5; break;

	case '\t':
	  strcpy(p3, "&nbsp;&nbsp;&nbsp;&nbsp;"); p3+=23; break;

	case '\n':
	  strcpy(p3, "<BR>"); p3+=3; break;

	case '\r':
	  strcpy(p3, "<BR>"); p3+=3; break;

	default:
	  *p3=*p2;
	}
    }
  *p3='\0';

  free(p);
  return s;
}



