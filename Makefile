
# advanced commands / options:
# g++ -MMD 	extracts dependency information, that is, it lists all the encountered header files
# g++ -MF 	specifies the output .d file for -MMD
# $@		current make target (file that invokes the rule)
objs := main.o instMan.o instStackTriInst.o myGl.o explosible.o explTraj.o

OBJECTS := $(addprefix build/,$(objs))

CC := g++
CFLAGS := -Wall -Werror -fmax-errors=1 -pedantic -Wextra -Wno-unused -g -O3
# do NOT use -ffast-math, we have NAN 
# -g
#-DNDEBUG
LIBS_RASPI = -L/usr/lib/aarch64-linux-gnu -lGL -lglfw
all: main

DEPS := $(patsubst %.o,%.d,$(OBJECTS))
-include $(DEPS)
DEPFLAGS = -MMD -MF $(@:.o=.d)

main: 	$(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS_RASPI)

%.o: ../src/%.cpp
	@echo "building target '$@'"
	$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS)

clean:
	rm -f $(OBJECTS) $(DEPS) main 

.PHONY: clean 