noinst_LTLIBRARIES = libDRAMsim.la

libDRAMsim_la_SOURCES = Address.cpp Address.h Algorithm.cpp Algorithm.h Bank.cpp Bank.h busEvent.cpp busEvent.h Channel.cpp Channel.h \
command.cpp command.h enumTypes.h event.cpp event.h executeCommand.cpp fbdAMB.cpp fbdAMB.h fbdChannel.cpp fbdChannel.h fbdFrame.cpp fbdFrame.h \
fbdSystem.cpp fbdSystem.h getNextCommand.cpp globals.cpp globals.h InputStream.cpp InputStream.h  \
minProtocolGap.cpp  powerConfig.cpp powerConfig.h queue.h Rank.cpp Rank.h reporting runSimulations.cpp Settings.cpp SettingsXML.cpp Settings.h simulationParameters.cpp \
simulationParameters.h SSTdramSystem.cpp SSTdramSystem.h Statistics.cpp Statistics.h SystemConfiguration.cpp SystemConfiguration.h System.cpp System.h \
TimingSpecification.cpp TimingSpecification.h transaction2commands.cpp transaction.cpp transaction.h reporting/soapC.cpp reporting/soapClient.cpp \
reporting/stdsoap2.cpp

libDRAMsim_la_CPPFLAGS = -I/usr/include/libxml2

libDRAMsim_la_LIBADD = -lxml2 -lboost_iostreams -lboost_thread -lboost_filesystem -lboost_system

libDRAMsim_la_DEFS = -DSSTDEBUG