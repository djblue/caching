CC=gcc
BIN=cacher
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
CFLAGS=-std=c99 -O3
LFLAGS=

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

test: P1 P2 P3 P4 P5 P6 P7 P12

P%: $(BIN)
	@./$(BIN) 1024 		./traces/P$*.lis
	@./$(BIN) 2048 		./traces/P$*.lis
	@./$(BIN) 4096 		./traces/P$*.lis
	@./$(BIN) 8192 		./traces/P$*.lis
	@./$(BIN) 16384 	./traces/P$*.lis
	@./$(BIN) 32768 	./traces/P$*.lis
	@./$(BIN) 65536 	./traces/P$*.lis
	@./$(BIN) 131072 	./traces/P$*.lis
	@./$(BIN) 262144 	./traces/P$*.lis
	@./$(BIN) 524288 	./traces/P$*.lis

clean:; @rm $(OBJ) $(BIN)
