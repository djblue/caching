CC=gcc
BIN=cacher
SRC=$(wildcard *.c)
CFLAGS=-std=c99 -O3
LFLAGS=

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(LFLAGS) -o $(BIN) $(SRC)

# zip project directory using the scheme
# yourFirstName-yourLastName.zip
zip:
	git archive --format zip master -o Abdullah-Badahdah.zip

clean:; @rm $(BIN)
