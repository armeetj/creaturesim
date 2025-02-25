CC = g++
CFLAGS = -Wall -std=c++11
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

TARGET = game
SOURCE = main.cpp

$(TARGET): $(SOURCE)
	$(CC) $(SOURCE) $(CFLAGS) -o $(TARGET) $(LIBS)

.PHONY: run clean

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
