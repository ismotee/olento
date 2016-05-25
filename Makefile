CC=g++
CFLAGS=-c -std=c++11 -Wall -w -Iolento/include
LDFLAGS=-lSDL2 -lSDL2_image -lSDL2_net -lGLEW -lGL
SOURCES=olento/dobject.cpp olento/material.cpp olento/objloader.cpp olento/oCamera.cpp olento/oLoader.cpp olento/oWindow.cpp olento/shader.cpp olento/random.cpp olento/server.cpp olento/olento.cpp olento/main.cpp olento/libnn.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=olentodemo

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@