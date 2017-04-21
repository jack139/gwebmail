#ifndef _HTML_H_
#define _HTML_H_

#define TEMPLATE_PATH "/usr/local/apache/cgi-bin/template"

#define CGI_BIN "/cgi-bin"
#define HTML_MAX_SHIFT_SET		200

// exam: sPath='/usr/local/evs/htdocs', sFileName = /admin/login.htm
char *html_Load(char *sPath, char *sFileName);
int html_File(char *pBuffer, char *start_token, char *end_token);
void html_Free(char *pBuffer);

char *html_GetTemplate(char *sBuffer, char *sMarkBegin, char *sMarkEnd);
void html_FillTemplate(char *sTemplate, ...);

char *html_ConvertToHTML(char *s);

#endif
