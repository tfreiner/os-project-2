CC=gcc
CFLAGS=-Wall
TARGET=project2

all: $(TARGET) 

$(TARGET): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.o $(TARGET)
