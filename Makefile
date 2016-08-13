# OK this is a cool learning experience but for the actual structure of the project
# We'll have:
#   /src contain the actual LayeredNN_Backprop_Floats code
#   /example contain several examples, including "Finding Zero" and 
#   /misc contain several historical files, including tanks.cpp, FullyConnectedNeural.cpp, and miscellaneous.
# make should therefore be able to build the following targets:
#   all: build the library - do one of the following (http://stackoverflow.com/q/2980102/1181387) (http://stackoverflow.com/a/6604697/1181387)
#      - statically linked a *.a/*.lib library, with libraries bundled in
#      - ld --relocatable object file, with no libraries bundled in (only linked)
#      - dynamically linked *.so/*.dll library, which will be linked at runtime (not compile-time)
#   ex*: build the example

all::                           # set default target to all

CC=g++                          # A "driver", which runs preprocessor (cpp), compiler (?), linker (ld), etc. as necessary.
                                  # g++ is just another name for gcc with libstdc++; gcc is "gnu compiler collection"
CFLAGS=-c -g -Wall -std=gnu++11 # COMPILE (-c) into an .o object file, make debugging easier (-g)
                                  # and emit all warnings (-Wall), and use C++11 stl (-std=gnu++11)
LDFLAGS=                        # LINK (default g++ action when invoked on .o files) - no options currently specified
SRC=$(wildcard src/*.cpp)       # Source files.
OBJ=$(addprefix build/,$(notdir $(SRC:.cpp=.o))) # Object files - replace all SRC's .cpp extensions with .o extensions.
                                                 # Then remove the source directory name (src) and add the build directory name (build).
EXECUTABLE=cppnn                # The name of the executable to write to

all: $(EXECUTABLE)          # Depend on the executable, of course
  $(EXECUTABLE)             # and execute it for laziness points

# Automatic collection of Include Dependencies
# http://stackoverflow.com/a/2394651/1181387
.PHONY: depend
depend: build/.depend                                 # Just an alias so we can call `make depend`.
build/.depend: $(SRC)                                 # the .depend file depends on all of the sources
        rm -f ./build/.depend                         # To create this file, we're going to clear away the old dependency list
        $(CC) $(CFLAGS) -MM $^ -MF ./build/.depend;   # And output the dependencies of each file in $^ = $(SRC) to the file ./.depend
                                                        # e.g.: `g++ -std=gnu++11 -MM LayeredNeural_BackProp_Floats.cpp` outputs two lines:
                                                          # LayeredNeural_BackProp_Floats.o: LayeredNeural_BackProp_Floats.cpp \
                                                          #  ../common.h Vectors.h NeuralNetwork.h AffineMatrix.h
                                                        # Usefully, this includes both the cpp file itself and the various .h files.
include build/.depend                                 # Now, next time you call `make` something, it'll include the correct dependencies list.

# Now that we've collected our .o file dependency graph, we need to make sure the executable depends on all of the .o files
$(EXECUTABLE): $(OBJ) 
  $(CC) $(LDFLAGS) $^ -o $@


.PHONY: clean
clean:
  rm -r *.o $(EXECUTABLE).exe

