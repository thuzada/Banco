CC = gcc
CFLAGS = -I./include -I./lib/sqlite
SRC = $(wildcard src/*.c) lib/sqlite/sqlite3.c
OBJ = $(SRC:.c=.o)
TARGET = Banco

all: $(TARGET)

$(TARGET): $(OBJ)
    $(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
    $(CC) -c -o $@ $< $(CFLAGS)

clean:
    rm -f $(OBJ) $(TARGET)