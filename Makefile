CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = uart_runner
SRC = uart.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)