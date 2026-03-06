CC = g++
CFLAGS = -Wall -O3

exercise01.1.exe : exercise01.1.o random.o
	$(CC) -o $@ $^
exercise01.1.o : exercise01.1.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
clean :
	rm *.o exercise01.1.exe *.out