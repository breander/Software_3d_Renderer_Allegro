CC = g++

CFLAGS = -std=c++14

LIBS = -lallegro -lallegro_main -lallegro_font -lallegro_ttf -lallegro_primitives -lallegro_color

SRCS = main.cpp

TARGET = main

all: $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

clean:
	$(RM) $(TARGET)