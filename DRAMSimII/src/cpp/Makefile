CC = g++

CCFLAGS = 
CFLAGS = -Wall -O3 -c
LDFLAGS = $(OPT)

dramsim2: dramsim2.o dramSystem.o dramChannel.o globals.o InputStream.o executeCommand.o transaction2commands.o getNextCommand.o main.o minProtocolGap.o rank_c.o runSimulations.o transaction2commands.o bank_c.o
	$(CC) $(CCFLAGS) -o dramsim2 dramsim2.o dramSystem.o dramChannel.o globals.o InputStream.o executeCommand.o transaction2commands.o getNextCommand.o main.o minProtocolGap.o rank_c.o runSimulations.o transaction2commands.o bank_c.o

clean: 
	rm *.o dramsim2

DRAM = dram_src

tar:
	cd .. ; \
	tar -cvf $(DRAM).tar src ; \
	gzip $(DRAM).tar 

%.o:%.cpp
	$(CC) $(CFLAGS) $< -o $@ 
