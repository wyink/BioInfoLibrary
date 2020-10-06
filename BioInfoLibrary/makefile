#makefile
CC = g++
CFLAGS = -c -std=c++17

all: BioinfoLibrary Analysis BlastParser Formatter PajekHelper Utils Link
BioinfoLibrary: BioinfoLibrary.cpp
	$(CC) $(CFLAGS) BioinfoLibrary.cpp

Analysis: Analysis.cpp
	$(CC) $(CFLAGS) Analysis.cpp

BlastParser: BlastParser.cpp
	$(CC) $(CFLAGS) BlastParser.cpp

Formatter:Formatter.cpp
	$(CC) $(CFLAGS) Formatter.cpp

PajekHelper:PajekHelper.cpp
	$(CC) $(CFLAGS) PajekHelper.cpp

Utils:Utils.cpp
	$(CC) $(CFLAGS) Utils.cpp

Link: BioinfoLibrary.o Analysis.o BlastParser.o Formatter.o PajekHelper.o Utils.o
	$(CC) -o  BioinfoLib BioinfoLibrary.o Analysis.o BlastParser.o Formatter.o PajekHelper.o Utils.o

