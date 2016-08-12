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

all::                       # set default target

CC=g++                      # A "driver", which runs preprocessor (cpp), compiler (?), linker (ld), etc. as necessary.
                              # g++ is just another name for gcc with libstdc++; gcc is "gnu compiler collection"
CFLAGS=-c -g -Wall          # COMPILE into a .o object file, and emit all warnings
LDFLAGS=                    # LINK (default g++ action for .o files)
SRC=$(wildcard src/*.cpp)   # Source files.
OBJ=$(src:.cpp=.o)          # 
EXECUTABLE=cppnn            # The name of the executable to write to

all: $(EXECUTABLE)          # Make the executable, of course
  $(EXECUTABLE)             # and execute it for laziness points

clean:
  rm -r *.o $(EXECUTABLE).exe

$(EXECUTABLE): $(OBJECTS) 
  $(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJECTS): same.o same.h
  $(CC) $(CFLAGS) $(SOURCES) -o $
