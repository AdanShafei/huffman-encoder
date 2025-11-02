CC = clang
CFLAGS = -g3 -Werror -Wall -Wextra -Wconversion -Wdouble-promotion -Wstrict-prototypes -pedantic -g
LFLAGS = 
SOURCES1 = bitwriter.c bitreader.c huff.c node.c pq.c 
SOURCES2 = bitwriter.c bitreader.c dehuff.c node.c pq.c 
SOURCES_TESTS = brtest.c bwtest.c nodetest.c pqtest.c
OBJECTS1 = $(SOURCES1:.c=.o)
OBJECTS2 = $(SOURCES2:.c=.o)
O_TESTS = $(SOURCES_TESTS:.c=.o)

EXEC1 = huff
EXEC2 = dehuff
TESTS = brtest bwtest nodetest pqtest

all: $(EXEC1) $(EXEC2) $(TESTS)

$(EXEC1): $(OBJECTS1) 
	$(CC) $^ $(LFLAGS) -o $(EXEC1)

$(EXEC2): $(OBJECTS2) 
	$(CC) $^ $(LFLAGS) -o $(EXEC2)

brtest: brtest.o bitreader.o
	$(CC) $^ $(LFLAGS) -o $@

bwtest: bwtest.o bitwriter.o
	$(CC) $^ $(LFLAGS) -o $@

nodetest: nodetest.o node.o
	$(CC) $^ $(LFLAGS) -o $@

pqtest: pqtest.o pq.o node.o
	$(CC) $^ $(LFLAGS) -o $@

%.o: %.c bitwriter.h bitreader.h node.h pq.h 
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(EXEC1) $(EXEC2) $(TESTS) *.o

format:
	clang-format -i -style=file *.[ch]

.PHONY: all clean format
