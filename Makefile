# advanced commands / options:
# g++ -MMD 	extracts dependency information, that is, it lists all the encountered header files
# g++ -MF 	specifies the output .d file for -MMD
# $@		current make target (file that invokes the rule)
objs := myGl.o \
	instStackTri.o \
	instStackLine.o \
	instStackTriInst.o main.o instMan.o \
	t1.o \
	explosible.o \
	explTraj.o utils.o \
	fpvInputProcessor.o \
	freeRoamCamera.o \
	crosshairs.o \
	terrain.o \
	terrTri.o \
	terrTriDomain.o

OBJECTS := $(addprefix build/,$(objs))

CC := g++
CFLAGS := -Wall -Werror -fmax-errors=1 -pedantic -Wextra -Wno-unused -O2
# -ffast-math 
#-DNDEBUG
LIBS_WIN64 = /mingw64/lib/libglew32.dll.a /mingw64/lib/libglfw3.dll.a -lopengl32
LIBS_WIN32 = /mingw32/lib/libglew32.dll.a /mingw32/lib/libglfw3.dll.a -lopengl32
LIBS_RASPI = -lGL -lglfw
# -lGLEW is not needed anymore
all: main

DEPS := $(patsubst %.o,%.d,$(OBJECTS))
#CLEANDEPS := $(addprefix build/,$(deps))
-include $(DEPS)
DEPFLAGS = -MMD -MF $(@:.o=.d)

main: 	$(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS_RASPI)

mainWin32.exe: 	$(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS_WIN32) 

mainWin64.exe: 	$(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS_WIN64) 

%.o: ../src/%.cpp
	@echo "building target '$@'"
	$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS)

clean:
	rm -f $(OBJECTS) $(DEPS) main mainWin32.exe mainWin64.exe *~ shaders/*~
	rm -Rf doxydoc

doxydoc:
	rm -Rf doxydoc
	doxygen

.PHONY: clean doxygen
