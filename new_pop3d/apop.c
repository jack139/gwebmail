
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <sys/types.h>

#include "pop3.h"


/* added by Glynn Clements <glynn@sensei.co.uk> 1997-06-02 */

/**************************************************************************/

static char timestamp[256];
static int stamplen;

/**************************************************************************/

char *
apop_timestamp()
{
	time_t t;
	struct tm *tm;

	time(&t);
	tm = localtime(&t);
	strftime(timestamp, sizeof(timestamp), "<%a %b %d %H:%M:%S %Y>", tm);

	return timestamp;
}

/**************************************************************************/

/* Verify a usercode/password-hash */
int
verify_user_apop(user, pass)
char *user;
char *pass;
{
	char buff[1024];
	int userlen, passlen;
	FILE *fp;
	char *p, *q;
	struct passwd *pwd;

	for (p = user; *p; p++)
		*p = tolower(*p);
	userlen = p - user;

	pwd = getpwnam(user);
	if (!pwd)
	{
		fprintf(stderr, "User not found in /etc/passwd: %s\n", user);
		return -1;
	}

	fp = fopen(APOP_PASSWORD_FILE, "r");
	if (!fp)
	{
		perror("verify_user_apop: fopen()");
		return -1;
	}

	while (1)
	{
		if (feof(fp))
		{
			fprintf(stderr, "User not found in /etc/apop: %s\n", user);
			return -1;
		}

		fgets(buff, sizeof(buff), fp);
		if (strncmp(buff, user, userlen) != 0)
			continue;
		if (buff[userlen] != ':')
			continue;

		q = timestamp + strlen(timestamp);
		for (p = buff + userlen + 1; *p && *p != '\n'; p++, q++)
			*q = *p;
		passlen = q - timestamp;
		break;
	}
	fclose(fp);

	do_md5_string(timestamp, passlen, buff);

	if (strcmp(pass, buff) != 0)
	{
		fprintf(stderr, "Invalid password for user %s\n", user);
		return -1;
	}

	if (setuid(pwd->pw_uid) < 0)
	{
		perror("verify_user_apop: setuid()");
		return -1;
	}

	return 0;
}

/**************************************************************************/

