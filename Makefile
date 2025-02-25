CC = g++
CFLAGS = -Wall -std=c++11 -I/opt/homebrew/include
LIBS = -L/opt/homebrew/lib -lraylib

TARGET = game
SOURCE = main.cpp

$(TARGET): $(SOURCE)
	$(CC) $(SOURCE) $(CFLAGS) -o $(TARGET) $(LIBS)

.PHONY: run clean

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
