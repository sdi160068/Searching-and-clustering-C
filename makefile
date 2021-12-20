CC=gcc
CFLAGS=-Wall
OBJECTS = data.o vector_list.o vector.o random.o HT.o hash.o mod.o loading.o lsh.o cube.o math_custom.o timer.o grid.o
CLUSTER = cluster.o cluster_main.o
SEARCH = search.o
GRID = grid.o
PLUS= -g -Og

all: cluster search

cluster			: $(CLUSTER) $(OBJECTS)
	$(CC) $(CFLAGS) $(CLUSTER) $(OBJECTS) $(PLUS) -o cluster -lm

search			: $(SEARCH) $(OBJECTS)
	$(CC) $(CFLAGS) $(SEARCH) $(OBJECTS) $(PLUS) -o search -lm

data.o   		: data.c
	$(CC) $(CFLAGS) -c data.c -o data.o

vector_list.o	: vector_list.c
	$(CC) $(CFLAGS) -c vector_list.c -o vector_list.o

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

grid.o			: grid.c
	$(CC) $(CFLAGS) -c grid.c -o grid.o

timer.o			: timer.c
	$(CC) $(CFLAGS) -c timer.c -o timer.o

lsh.o			: lsh.c
	$(CC) $(CFLAGS) -c lsh.c -o lsh.o

cube.o			: cube.c
	$(CC) $(CFLAGS) -c cube.c -o cube.o

cluster.o			: cluster.c
	$(CC) $(CFLAGS) -c cluster.c -o cluster.o

cluster_main.o			: cluster_main.c
	$(CC) $(CFLAGS) -c cluster_main.c -o cluster_main.o

search.o   			: search.c
	$(CC) $(CFLAGS) -c search.c -o search.o

.PHONY: clean

clean:
	rm -f $(OBJECTS) lsh cube cluster search lsh.o cube.o cluster.o cluster_main.o \
	search.o math_custom.o timer.o grid.o
