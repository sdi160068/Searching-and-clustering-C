CC=gcc
CFLAGS=-Wall
OBJECTS = data.o vector_list.o vector.o random.o HT.o hash.o mod.o loading.o lsh.o cube.o stack.o math_custom.o curve.o timer.o grid.o
MAIN = main.o
MAIN2 = main2.o
MAIN3 = main3.o
LSH = lsh.o
CUBE = cube.o
CLUSTER = cluster.o
SEARCH = search.o
GRID = grid.o
PLUS= -g -Og

all: main main2 lsh cube cluster search

main			: $(MAIN) $(OBJECTS)
	$(CC) $(CFLAGS) $(MAIN) $(OBJECTS) $(PLUS) -o main -lm

main2			: $(MAIN2) $(OBJECTS)
	$(CC) $(CFLAGS) $(MAIN2) $(OBJECTS) $(PLUS) -o main2 -lm

main3			: $(MAIN3) $(OBJECTS)
	$(CC) $(CFLAGS) $(MAIN3) $(OBJECTS) $(PLUS) -o main3 -lm

lsh				: $(LSH) $(OBJECTS)
	$(CC) $(CFLAGS) $(LSH) $(OBJECTS) $(PLUS) -o lsh -lm

cube			: $(CUBE) $(OBJECTS)
	$(CC) $(CFLAGS) $(CUBE) $(OBJECTS) $(PLUS) -o cube -lm

cluster			: $(CLUSTER) $(OBJECTS)
	$(CC) $(CFLAGS) $(CLUSTER) $(OBJECTS) $(PLUS) -o cluster -lm

search			: $(SEARCH) $(OBJECTS)
	$(CC) $(CFLAGS) $(SEARCH) $(OBJECTS) $(PLUS) -o search -lm

data.o   		: data.c
	$(CC) $(CFLAGS) -c data.c -o data.o

vector_list.o	: vector_list.c
	$(CC) $(CFLAGS) -c vector_list.c -o vector_list.o

stack.o			: stack.c
	$(CC) $(CFLAGS) -c stack.c -o stack.o

vector.o		: vector.c
	$(CC) $(CFLAGS) -c vector.c -o vector.o -lm

random.o   		: random.c
	$(CC) $(CFLAGS) -c random.c -o random.o

HT.o   			: HT.c
	$(CC) $(CFLAGS) -c HT.c -o HT.o

hash.o   		: hash.c
	$(CC) $(CFLAGS) -c hash.c -o hash.o

mod.o   		: mod.c
	$(CC) $(CFLAGS) -c mod.c -o mod.o

loading.o 	: loading.c
	$(CC) $(CFLAGS) -c loading.c -o loading.o

math_custom.o	: math_custom.c
	$(CC) $(CFLAGS) -c math_custom.c -o math_custom.o

curve.o			: curve.c
	$(CC) $(CFLAGS) -c curve.c -o curve.o

grid.o			: grid.c
	$(CC) $(CFLAGS) -c grid.c -o grid.o

timer.o			: timer.c
	$(CC) $(CFLAGS) -c timer.c -o timer.o

main.o   		: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

main2.o   		: main2.c
	$(CC) $(CFLAGS) -c main2.c -o main2.o

main3.o   		: main3.c
	$(CC) $(CFLAGS) -c main3.c -o main3.o

lsh.o			: lsh.c
	$(CC) $(CFLAGS) -c lsh.c -o lsh.o

cube.o			: cube.c
	$(CC) $(CFLAGS) -c cube.c -o cube.o

cluster.o			: cluster.c
	$(CC) $(CFLAGS) -c cluster.c -o cluster.o

search.o   			: search.c
	$(CC) $(CFLAGS) -c search.c -o search.o

.PHONY: clean

clean:
	rm -f $(OBJECTS) main3 main2 main lsh cube cluster search \
	main3.o main2.o main.o lsh.o cube.o cluster.o cluster_main.o \
	search.o stack.o math_custom.o curve.o timer.o grid.o
