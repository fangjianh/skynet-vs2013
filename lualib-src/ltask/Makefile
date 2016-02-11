SRCS = ltask.c handlemap.c queue.c schedule.c serialize.c

linux : ltask.so
mingw : ltask.dll

ltask.so : $(SRCS)
	gcc -Wall -g --shared -fpic -o$@ $^ -lpthread

ltask.dll : $(SRCS)
	gcc -Wall -g --shared -o $@ $^ -I/usr/local/include -L/usr/local/bin -llua53

clean :
	rm -f ltask.so ltask.dll
