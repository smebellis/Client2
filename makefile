# Place this makefile in the same directory where all your *.cpp and *h files are in
# Then, at the command prompt type: make

#for use with c++ files
.SUFFIXES : .cpp

#Compile with g++
CC = g++

#use -g for gnu debugger and -std- for c++11 compiling
CXXFLAGS = -g -std=c++11

#####Change to match all .cpp files.  Do not include .h files ####
OBJS = main.o

####Change the name of the executable####
TARGET = client.exe

#####Do NOT change this...Tabs Matter!#####
$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

.cpp.o:
	$(CC) -c $(CXXFLAGS) $(INCDIR) $<

#In order to get rid of all .o files created, at the command prompt
#make clean

clean:
	rm -f $(OBJS) $(TARGET) core
 
