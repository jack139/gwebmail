#
# Change these variables for your environment
#

CGI-BIN_PATH = /usr/local/apache/cgi-bin/
HTDOCS_PATH  = /usr/local/apache/htdocs/

# CONFIG_PATH must EQUAL to CONFIG_PATH in db.h
CONFIG_PATH  = /usr/local/conf  

INCLUDE = -I/usr/local/include/mysql
LIBS    = -L./ -L/usr/local/lib/mysql -lcgic -lmysqlclient -lm


#
# DON'T MODIFY DEBLOW IF YOU UNSURE !!!!!!!!!!!!!!!!
#

HEADER	= db.h md5.h upload.h mail_sub.h lang.h
DB_OBJ  = db.o 
MD5_OBJ = md5.o md5c.o 
UPLOAD_OBJ = upload.o 
MAIL_OBJ= mail_sub.o connectsock.o connectTCP.o base64.o mime.o
HTML_OBJ= html.o 

CFLAGS  = -Wall -O3 -pedantic -ansi # -g
CC      = gcc
AR      = ar


all:  	libcgic.a \
  	login logout listmenu folderlist folder mail move \
	outbox send empty part showmail mailbar info modinfo \
	register regform doreg forget doforget \
	addfolder delfolder \
	login0 forget0 register0

.c.o:
	$(CC) -c $(INCLUDE) $<

libcgic.a: cgic.o cgic.h
	rm -f libcgic.a
	$(AR) rc  libcgic.a cgic.o

login: login.o $(HEADER) $(DB_OBJ) $(MD5_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(MD5_OBJ) $(LIBS)

logout: logout.o $(HEADER) $(DB_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(LIBS)

listmenu: listmenu.o $(HEADER) $(DB_OBJ) $(HTML_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(HTML_OBJ) $(LIBS)

folderlist: folderlist.o $(HEADER) $(DB_OBJ) $(MAIL_OBJ) $(HTML_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(MAIL_OBJ) $(HTML_OBJ) $(LIBS)

folder: folder.o $(HEADER) $(DB_OBJ) $(MAIL_OBJ) $(HTML_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(MAIL_OBJ) $(HTML_OBJ) $(LIBS)

mail:	mail.o $(HEADER) $(DB_OBJ) $(MAIL_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(MAIL_OBJ) $(LIBS)

part: 	part.o $(HEADER) $(DB_OBJ) $(MAIL_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(MAIL_OBJ) $(LIBS)

move:	move.o $(HEADER) $(DB_OBJ) $(MAIL_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(MAIL_OBJ) $(LIBS)

outbox: outbox.o $(HEADER) $(DB_OBJ) $(HTML_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(HTML_OBJ) $(LIBS)

send:	send.o $(HEADER) $(DB_OBJ) $(MAIL_OBJ) $(MD5_OBJ) $(UPLOAD_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(MAIL_OBJ) $(MD5_OBJ) $(UPLOAD_OBJ) $(LIBS)

empty: 	empty.o $(HEADER) $(DB_OBJ) 
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(LIBS)

showmail: showmail.o $(HEADER) $(DB_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(LIBS)

mailbar: mailbar.o $(HEADER) $(DB_OBJ) $(MD5_OBJ) $(HTML_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(HTML_OBJ) $(LIBS)

info:	info.o $(HEADER) $(DB_OBJ) $(HTML_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(HTML_OBJ) $(LIBS)

modinfo: modinfo.o $(HEADER) $(DB_OBJ) $(MD5_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(MD5_OBJ) $(LIBS)

register: register.o $(HEADER) $(DB_OBJ) $(MD5_OBJ) $(ADUSER_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(MD5_OBJ) $(ADUSER_OBJ) $(LIBS)

regform: regform.o $(HEADER) $(DB_OBJ) $(HTML_OBJ) 
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(HTML_OBJ) $(LIBS)

doreg:	doreg.o $(HEADER) $(DB_OBJ) $(ADUSER_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(ADUSER_OBJ) $(LIBS)

forget: forget.o $(HEADER) $(DB_OBJ) $(MD5_OBJ) $(HTML_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(MD5_OBJ) $(HTML_OBJ) $(LIBS)

doforget: doforget.o $(HEADER) $(DB_OBJ) 
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(LIBS)

addfolder: addfolder.o $(HEADER) $(DB_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(LIBS)

delfolder: delfolder.o $(HEADER) $(DB_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(LIBS)

login0:	login0.o $(HEADER) $(DB_OBJ) $(HTML_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(HTML_OBJ) $(LIBS)

forget0: forget0.o $(HEADER) $(DB_OBJ) $(HTML_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(HTML_OBJ) $(LIBS)

register0: register0.o $(HEADER) $(DB_OBJ) $(HTML_OBJ)
	$(CC) -o $@ $(CFLAGS) $@.o $(DB_OBJ) $(HTML_OBJ) $(LIBS)

install:
	install -c -m 755 login		$(CGI-BIN_PATH)
	install -c -m 755 logout	$(CGI-BIN_PATH)
	install -c -m 755 listmenu	$(CGI-BIN_PATH)
	install -c -m 755 folderlist	$(CGI-BIN_PATH)
	install -c -m 755 folder	$(CGI-BIN_PATH)
	install -c -m 6755 mail		$(CGI-BIN_PATH)
	install -c -m 6755 move		$(CGI-BIN_PATH)
	install -c -m 755 outbox	$(CGI-BIN_PATH)
	install -c -m 755 send		$(CGI-BIN_PATH)
	install -c -m 755 empty		$(CGI-BIN_PATH)
	install -c -m 6755 part		$(CGI-BIN_PATH)nph-part
	install -c -m 755 showmail	$(CGI-BIN_PATH)
	install -c -m 755 mailbar	$(CGI-BIN_PATH)
	install -c -m 755 info		$(CGI-BIN_PATH)
	install -c -m 6755 modinfo	$(CGI-BIN_PATH)
	install -c -m 755 register	$(CGI-BIN_PATH)
	install -c -m 755 regform	$(CGI-BIN_PATH)
	install -c -m 6755 doreg	$(CGI-BIN_PATH)
	install -c -m 755 forget	$(CGI-BIN_PATH)
	install -c -m 755 doforget	$(CGI-BIN_PATH)
	install -c -m 755 addfolder	$(CGI-BIN_PATH)
	install -c -m 755 delfolder 	$(CGI-BIN_PATH)
	install -c -m 755 login0	$(CGI-BIN_PATH)
	install -c -m 755 forget0	$(CGI-BIN_PATH)
	install -c -m 755 register0	$(CGI-BIN_PATH)

	install -d -m 755 $(CGI-BIN_PATH)template
	install -c -m 644 ../template/*	$(CGI-BIN_PATH)template
	install -c -m 644 ../*.html	$(HTDOCS_PATH)
	install -d -m 755 $(HTDOCS_PATH)images
	install -c -m 644 ../images/* 	$(HTDOCS_PATH)images
	install -d -m 755 $(CONFIG_PATH)
	install -c -m 644 ../signature 	$(CONFIG_PATH)

clean:
	rm -f *.o core a.out *~ *.a \
		login logout listmenu folderlist folder mail move \
		outbox send empty part showmail mailbar info modinfo \
		register regform doreg forget doforget \
		addfolder delfolder \
		login0 forget0 register0






