CC = g++
CFLAGS = -Wall -O3

exercise03.1.exe : exercise03.1.o random.o
	$(CC) -o $@ $^
exercise03.1.o : exercise03.1.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
clean :
	rm *.o exercise03.1.exe *.out