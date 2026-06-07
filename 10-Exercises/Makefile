CC = mpicxx
CFLAGS = -Wall -O3

all: exercise10.1.exe single_core.exe

exercise10.1.exe : exercise10.1.o random.o individual.o population.o
	$(CC) -o $@ $^
exercise10.1.o : exercise10.1.cpp random.h individual.h population.h
	$(CC) -c $< -o $@ $(CFLAGS)
single_core.exe : single_core.o random.o individual.o population.o
	g++ -o $@ $^
single_core.o : single_core.cpp random.h individual.h population.h
	g++ -c $< -o $@ $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
individual.o : individual.cpp individual.h
	$(CC) -c $< -o $@ $(CFLAGS)
population.o : population.cpp population.h
	$(CC) -c $< -o $@ $(CFLAGS)

clean :
	rm *.exe *.o
remove :
	rm OUTPUT/*.out