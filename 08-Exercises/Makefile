CC = g++
CFLAGS = -Wall -O3

exercise08.1.exe : exercise08.1.o random.o variationalMC.o
	$(CC) -o $@ $^
exercise08.1.o : exercise08.1.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
variationalMC.o : variationalMC.cpp variationalMC.h
	$(CC) -c $< -o $@ $(CFLAGS)
clean :
	rm *.o exercise08.1.exe *.out