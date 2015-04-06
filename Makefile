CC=gcc
BIN=cacher
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
CFLAGS=-std=c99 -O3
LFLAGS=
TIME=/usr/bin/time

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

# zip project directory using the scheme
# yourFirstName-yourLastName.zip
zip:
	git archive --format zip master -o Abdullah-Badahdah.zip

test: $(BIN)
	./$(BIN) 32768 ./traces/P1.lis

clean:; @rm $(OBJ) $(BIN)
