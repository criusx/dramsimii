CC = g++

CCFLAGS = 
CFLAGS = -Wall -O3 -c
LDFLAGS = $(OPT)

dramsim2: dramsim2.o dram_system.o dram_channel.o globals.o input_stream.o main.o executeCommand.o transaction2commands.o getNextCommand.o
	$(CC) $(CCFLAGS) -o dramsim2 dramsim2.o dram_system.o dram_channel.o globals.o input_stream.o main.o

clean: 
	rm *.o dramsim2

DRAM = dram_src

tar:
	cd .. ; \
	tar -cvf $(DRAM).tar src ; \
	gzip $(DRAM).tar 

%.o:%.cpp
	$(CC) $(CFLAGS) $< -o $@ 
