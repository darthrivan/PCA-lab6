CC= gcc
CFLAGS= -O2 -march=native -fno-inline
all:
	$(CC) $(CFLAGS) vector_add.c -o vector_add.e8  -DCHAR
	$(CC) $(CFLAGS) vector_add.c -o vector_add.e16 -DSHORT
	$(CC) $(CFLAGS) vector_add.c -o vector_add.e32 -DINT
	$(CC) $(CFLAGS) vector_add.c -o vector_add.v8  -DVECTORIAL -DCHAR 
	$(CC) $(CFLAGS) vector_add.c -o vector_add.v16 -DVECTORIAL -DSHORT
	$(CC) $(CFLAGS) vector_add.c -o vector_add.v32 -DVECTORIAL -DINT

clean:
	rm -f vector_add.e* vector_add.v*
