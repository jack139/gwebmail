EXENAME= bkrun

CC    = gcc
CFLAGS= -O3 -m486 -fomit-frame-pointer -DLINUX
OBJS  = main.o db.o mail_sub.o connectTCP.o connectsock.o
LIBS  = -L/usr/local/lib/mysql -lmysqlclient 
INC   = -I/usr/local/include/mysql

all: $(EXENAME)

install: all
	chmod +s bkrun

clean:
	rm -f *.o */*.o *~ core $(EXENAME)

.c.o: 
	$(CC) -c $(CFLAGS) $(INC) $<

$(EXENAME): $(OBJS)
	$(CC) -o $(EXENAME) $(OBJS) $(LIBS)





