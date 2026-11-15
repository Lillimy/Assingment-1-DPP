
CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -pthread

TARGET = assign1.3
OBJECTS = main.o sieve.o queue.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
