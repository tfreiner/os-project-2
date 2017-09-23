CC=gcc
CFLAGS=-Wall
TARGET=master palin

all: $(TARGET) 

$(TARGET): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.o $(TARGET)
