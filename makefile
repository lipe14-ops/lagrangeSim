CC = g++
FILE = ./src/main.cpp
EXE = ./dist/lagrange
CFLAGS = -lraylib

all: $(EXE)
	$(EXE)

$(EXE): $(FILE)
	$(CC) $(CFLAGS) $(FILE) -o $(EXE) 
