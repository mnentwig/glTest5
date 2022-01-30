
# advanced commands / options:
# g++ -MMD 	extracts dependency information, that is, it lists all the encountered header files
# g++ -MF 	specifies the output .d file for -MMD
# $@		current make target (file that invokes the rule)

# note: Conventionally the engine would be .so/.a/.dll, this approach works with individual 
# object files and is intended to be somewhat faster
objs_user := main.o
objs_engine := instMan.o instStackTriInst.o myGl.o explosible.o explTraj.o
LIBS_RASPI = -L/usr/lib/aarch64-linux-gnu -lGL -lglfw
CC := g++
CFLAGS := -Wall -Werror -fmax-errors=1 -pedantic -Wextra -Wno-unused -g -O3

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

build/%.o: src/%.cpp
	@echo "building user target '$@'"
	$(CC) $(CFLAGS) -c -o $@ $< $(DEPEXTR_FLAGS)

buildEngine/%.o: srcEngine/%.cpp
	@echo "building engine target '$@'"
	$(CC) $(CFLAGS) -c -o $@ $< $(DEPEXTR_FLAGS)

clean:
	rm -f $(OBJECTS_USER) $(OBJECTS_ENGINE) $(DEPS_USER) $(DEPS_ENGINE) main 

.PHONY: clean 