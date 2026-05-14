CC = mpicxx
CFLAGS = -Wall -O3

exercise10.1.exe : exercise10.1.o random.o individual.o population.o
	$(CC) -o $@ $^
exercise10.1.o : exercise10.1.cpp random.h individual.h population.h
	$(CC) -c $< -o $@ $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)
individual.o : individual.cpp individual.h
	$(CC) -c $< -o $@ $(CFLAGS)
population.o : population.cpp population.h
	$(CC) -c $< -o $@ $(CFLAGS)

clean :
	rm *.o *.exe *.out