# $Id: Makefile,v davewang Exp $

CC = gcc

OPT = -O3  -g

INCLUDEPATH = -I/usr/local/include -I/usr/include -I/usr/openwin/include
LIBDIRS = -L/usr/local/lib -L/usr/lib
LIBS =  -lm 

CFLAGS = $(OPT) $(INCLUDEPATH)
LDFLAGS = $(OPT)

DRAMOBJ = address.o algorithm.o commandline.o dramsim.o execute.o files.o inits.o protocol.o queues.o random.o stats.o

dramsim: $(DRAMOBJ)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(DRAMOBJ) $(LIBDIRS) -o $@ $(LIBS)

clean: 
	rm *.o
	rm *~
	rm dramsim

DRAM = dram_src

tar:
	cd .. ; \
	tar -cvf $(DRAM).tar src ; \
	gzip $(DRAM).tar 

address.o: dramsim.h
algorithm.o: dramsim.h
commandline.o: dramsim.h
dramsim.o: dramsim.h
execute.o: dramsim.h
inits.o: dramsim.h
files.o: dramsim.h
protocol.o: dramsim.h
queues.o: dramsim.h
random.o: dramsim.h
stats.o: dramsim.h
