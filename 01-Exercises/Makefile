CC = g++
CFLAGS = -Wall -O3

all: exercise01.1.exe exercise01.2.exe exercise01.3.exe

exercise01.1.exe : exercise01.1.o random.o
	$(CC) -o $@ $^
exercise01.1.o : exercise01.1.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
exercise01.2.exe : exercise01.2.o random.o
	$(CC) -o $@ $^
exercise01.2.o : exercise01.2.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
exercise01.3.exe : exercise01.3.o random.o
	$(CC) -o $@ $^
exercise01.3.o : exercise01.3.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
clean :
	rm *.o exercise01.*.exe
remove :
	rm OUTPUT/*.out