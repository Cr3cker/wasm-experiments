CC = gcc
INCLUDE_DIR = ./raylib/src
LIB_DIR = ./raylib/src
TARGET = dino
SRC = dino.c
CFLAGS = -I$(INCLUDE_DIR)
LDFLAGS = -L$(LIB_DIR) -lraylib -lm -lpthread -ldl -lrt -lX11

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)