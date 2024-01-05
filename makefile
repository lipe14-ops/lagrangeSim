CC = g++
FILE = ./src/main.cpp
EXE = ./dist/lagrange

all: $(EXE)
	$(EXE)

$(EXE): $(FILE)
	$(CC) $(FILE) -o $(EXE)
