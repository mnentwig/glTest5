
# advanced commands / options:
# g++ -MMD 	extracts dependency information, that is, it lists all the encountered header files
# g++ -MF 	specifies the output .d file for -MMD
# $@		current make target (file that invokes the rule)

# note: Conventionally the engine would be .so/.a/.dll, this approach works with individual 
# object files, somewhat less complex in debugging
objs_user := main.o
objs_engine := engine.o instMan.o instStackTriInst.o explosible.o explTraj.o observer.o map/surface.o map/antCrawlerSurface.o map/myAntCrawlerSurface.o map/antCrawler.o
LIBS_WIN = -lglew32.dll -lglfw3.dll -lopengl32
LIBS_RASPI = -L/usr/lib/aarch64-linux-gnu -lGL -lglfw

CC := g++
# regarding "-I.": #includes should preferably be referenced to the top level directory, not the location of the including file (unless it's the same)
CFLAGS := -Wall -Werror -fmax-errors=1 -pedantic -Wextra -Wno-unused -g -O3 -I.

OBJECTS_USER := $(addprefix build/,$(objs_user))
OBJECTS_ENGINE := $(addprefix buildEngine/,$(objs_engine))
DEPS_USER := $(patsubst %.o,%.d,$(OBJECTS_USER))
DEPS_ENGINE := $(patsubst %.o,%.d,$(OBJECTS_ENGINE))

# do NOT use -ffast-math, we have NAN 
# -g
#-DNDEBUG
all: main

-include $(DEPS_USER)
-include $(DEPS_ENGINE)

DEPEXTR_FLAGS = -MMD -MF $(@:.o=.d)

main: 	$(OBJECTS_USER) $(OBJECTS_ENGINE)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS_RASPI)
mainWin.exe: 	$(OBJECTS_USER) $(OBJECTS_ENGINE)
	$(CC) -static $(CFLAGS) -o $@ $^ $(LIBS_WIN)
	
build/%.o: src/%.cpp
	@echo "building user target '$@'"
	@mkdir -p $(@D)        
	$(CC) $(CFLAGS) -c -o $@ $< $(DEPEXTR_FLAGS)

buildEngine/%.o: srcEngine/%.cpp
	@echo "building engine target '$@'"
	@mkdir -p $(@D)        
	$(CC) $(CFLAGS) -c -o $@ $< $(DEPEXTR_FLAGS)

clean:
	rm -f $(OBJECTS_USER) $(OBJECTS_ENGINE) $(DEPS_USER) $(DEPS_ENGINE) main mainWin.exe

.PHONY: clean 