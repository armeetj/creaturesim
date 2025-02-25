CC = g++
CFLAGS = -Wall -std=c++11 -I/opt/homebrew/include
LIBS = -L/opt/homebrew/lib -lraylib

TARGET = game
SOURCES = main.cpp Creature.cpp
HEADERS = Creature.h

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(SOURCES) $(CFLAGS) -o $(TARGET) $(LIBS)

.PHONY: run clean

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
