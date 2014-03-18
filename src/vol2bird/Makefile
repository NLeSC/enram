OBJS = vol2bird_routines.o 
CC = gcc 
HDFFLAGS=-lhdf5_hl -lhdf5 
LIBFLAGS=-lz -lm
 
vol2bird : $(OBJS)
	$(CC) -shared  -o ./vol2bird.so $(OBJS) $(LIBFLAGS)
	
vol2bird_routines.o : vol2bird_routines.c
	$(CC) -fPIC -c vol2bird_routines.c -o vol2bird_routines.o $(LIBFLAGS)

clean: 
	rm -f *.o *.so 
	
