CC = g++
CFLAGS = -Wall -O3

exercise02.2.exe : exercise02.2.o random.o random_walk.o
	$(CC) -o $@ $^
exercise02.2.o : exercise02.2.cpp random.h random_walk.h
	$(CC) -c $< -o $@ $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
random_walk.o : random_walk.cpp random_walk.h
	$(CC) -c $< -o $@ $(CFLAGS)

clean :
	rm *.o exercise02.2.exe *.out