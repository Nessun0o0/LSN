CC = g++
CFLAGS = -Wall -O3

exercise02.1.exe : exercise02.1.o random.o
	$(CC) -o $@ $^
exercise02.1.o : exercise02.1.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
clean :
	rm *.o exercise02.1.exe *.out