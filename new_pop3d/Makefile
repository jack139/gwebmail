# make file for the IP/TCP/POP3 server for VAX/UNIX 4.3BSD (now Linux)

# Set the installation directory for the binary
BINDIR = /usr/sbin

# Where should the man page live?
MANDIR = /usr/man/man1

# Set the executable name.  This must matche whatever is in /etc/inetd.conf
EXENAME= in.pop3d

# -DTACACS_AUTH for that form of authentication.
# -DSHADOWPWD   for shadow authentication
# -DLINUX       for Linux (not tested anywhere else lately?)
# -DVIRTUAL	for VIRTUAL hosting (new, not functional yet).
# -DQMAIL	for ~/Mailbox QMAIL machines
CFLAGS= -O3 -m486 -fomit-frame-pointer -DSHADOWPWD -DLINUX

# Set the POP temporary work directory.  This must match the value
# in pop3d.h  Comment out if using -DQMAIL
POPWORK= /usr/local/temp/pop

OBJS  = main.o folder.o util.o md5.o apop.o tacacs_auth.o \
	virtual.o md5/md5.o mysql_sendmail.o

# LIBS = -lshadow for shadow authentication
LIBS  = -L/usr/local/lib/mysql -lcrypt -lmysqlclient 

all: $(EXENAME)

install: all
	install -c -m 700 $(EXENAME) $(BINDIR)/$(EXENAME)
#	install -c -m 644 pop3d.1 $(MANDIR)
ifdef POPWORK
	install -d -m 1777 $(POPWORK)
endif

clean:
	rm -f *.o */*.o *~ core $(EXENAME)


$(EXENAME): $(OBJS)
	cc -o $(EXENAME) $(OBJS) $(LIBS)

main.o:	main.c pop3.h
folder.o: folder.c pop3.h
util.o: util.c pop3.h
	cc -c $(CFLAGS) -I/usr/local/include/mysql util.c
md5/md5.o: md5/md5.c md5/md5.h
md5.o: md5.c md5/md5.h pop3.h
apop.o: apop.c pop3.h
tacacs_auth.o: tacacs_auth.c tacacs.h
virtual.o: virtual.c pop3.h
mysql_sendmail.o: mysql_sendmail.c mysql_sendmail.h
	cc -c $(CFLAGS) -I/usr/local/include/mysql mysql_sendmail.c



