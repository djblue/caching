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

.PHONY: clean zip test

test: $(BIN)
	@./$(BIN) 32768 ./traces/P1.lis
	@./$(BIN) 32768 ./traces/P2.lis
	@./$(BIN) 32768 ./traces/P3.lis
	@./$(BIN) 32768 ./traces/P4.lis
	@./$(BIN) 32768 ./traces/P5.lis
	@./$(BIN) 32768 ./traces/P6.lis
	@./$(BIN) 32768 ./traces/P7.lis
	@./$(BIN) 32768 ./traces/P12.lis

clean:; @rm $(OBJ) $(BIN)
