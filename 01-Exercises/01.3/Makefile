CC = g++
CFLAGS = -Wall -O3

exercise01.3.exe : exercise01.3.o random.o
	$(CC) -o $@ $^
exercise01.3.o : exercise01.3.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
clean :
	rm *.o exercise01.3.exe *.out