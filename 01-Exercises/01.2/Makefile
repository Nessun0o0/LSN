CC = g++
CFLAGS = -Wall -O3

exercise01.2.exe : exercise01.2.o random.o
	$(CC) -o $@ $^
exercise01.2.o : exercise01.2.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
clean :
	rm *.o exercise01.2.exe *.out