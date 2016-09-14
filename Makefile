CC=g++
CFLAGS=-c -std=c++11 -Wall -w -Iolento/include
LDFLAGS=-lSDL2 -lSDL2_image -lSDL2_net -lGLEW -lGL -lpthread
SOURCES=olento/dobject.cpp \
olento/archiver.cpp \
olento/oDirectory.cpp \
olento/oMesh.cpp \
olento/oMeshBundle.cpp \
olento/oModificators.cpp \
olento/material.cpp \
olento/oCamera.cpp \
olento/oLoader.cpp \
olento/olentogl.cpp \
olento/shader.cpp \
olento/random.cpp \
olento/server.cpp \
olento/main.cpp \
olento/libnn.cpp \
olento/libnnInterface.cpp \
olento/tilanne.cpp \
olento/tilanteet.cpp \
olento/genetic.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=olentodemo

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
