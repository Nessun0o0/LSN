CC = g++
CFLAGS = -O3 --std=c++11
AFLAGS = -larmadillo
all: simulator.exe
simulator.exe : system.o NSL_SIMULATOR.o particle.o random.o
	$(CC) system.o particle.o random.o NSL_SIMULATOR.o -o simulator.exe $(AFLAGS)
NSL_SIMULATOR.o : NSL_SIMULATOR.cpp system.h particle.h
	$(CC) -c NSL_SIMULATOR.cpp -o NSL_SIMULATOR.o $(CFLAGS)
system.o : system.cpp system.h random.h particle.h
	$(CC) -c system.cpp -o system.o $(CFLAGS)
particle.o : particle.cpp particle.h
	$(CC) -c particle.cpp -o particle.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
low_entropy : NSL_SIMULATOR.o particle.o random.o system_low_entropy.o
	$(CC) NSL_SIMULATOR.o particle.o random.o system_low_entropy.o -o simulator_low_entropy.exe $(AFLAGS)
system_low_entropy.o : system_low_entropy.cpp system.h
	$(CC) -c system_low_entropy.cpp -o system_low_entropy.o $(CFLAGS)
clean :
	rm -f *.o *.exe
remove:
	rm ../OUTPUT/*.*
	rm ../OUTPUT/CONFIG/*.*
