all:
	cd cgi;pwd;make	
	cd new_pop3d;pwd;make
	cd bkrun;pwd;make
	@echo
	@echo "-------<done>---------"

install:
	cd cgi;pwd;make	install
	cd new_pop3d;pwd;make install
	cd bkrun;pwd;make install
	@echo
	@echo "-------<done>---------"

clean: 
	cd cgi;pwd;make clean
	cd new_pop3d;pwd;make clean
	cd bkrun;pwd;make clean

	rm -f *.o *.a *~ core
	@echo
	@echo "-------<cleaned>-------"






