# OK this is a cool learning experience but for the actual structure of the project
# We'll have:
#	 /src contain the actual LayeredNN_Backprop_Floats code
#	 /example contain several examples, including "Finding Zero" and 
#	 /misc contain several historical files, including tanks.cpp, FullyConnectedNeural.cpp, and miscellaneous.
# make should therefore be able to build the following goals:
#	 all: build the library - do one of the following (http://stackoverflow.com/q/2980102/1181387) (http://stackoverflow.com/a/6604697/1181387)
#			- statically linked a *.a/*.lib library, with libraries bundled in
#			- ld --relocatable object file, with no libraries bundled in (only linked)
#			- dynamically linked *.so/*.dll library, which will be linked at runtime (not compile-time)
#	 ex*: build the example

# Our C++ Compiler.
# G++ is a "driver", which runs preprocessor (cpp), compiler (?), linker (ld), etc. as necessary.
# g++ is just another name for gcc with libstdc++; gcc is "gnu compiler collection"
CC=g++

# COMPILE (-c) into an .o object file, make debugging easier (-g)
# and emit all warnings (-Wall), and use C++11 stl (-std=gnu++11)
CFLAGS=-c -g -Wall -std=gnu++11

# LINK (default g++ action when invoked on .o files) - no options currently specified
LDFLAGS=

# Specify the directories
SRC_DIR=src
BUILD_DIR=build

# Source files.
SRC=$(wildcard $(SRC_DIR)/*.cpp)

# Object files - replace all SRC's .cpp extensions with .o extensions.
# Then remove the source directory name ($(SRC_DIR)) and add the build directory name ($(BUILD_DIR)).
OBJ=$(addprefix $(BUILD_DIR)/,$(notdir $(SRC:.cpp=.o)))

# The name of the executable to write to
EXECUTABLE=$(BUILD_DIR)/cppnn

# The default goal is the first one in the Makefile. It depends on the main executable, of course
.PHONY: all
all: dirs depend $(EXECUTABLE)

dirs: $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)

# Automatic collection of Include Dependencies
# http://stackoverflow.com/a/2394651/1181387
# Just an alias so we can call `make depend`.
.PHONY: depend
depend: dirs $(BUILD_DIR)/.depend
# the .depend dependency file depends on all of the sources
$(BUILD_DIR)/.depend: $(SRC)
	# To create this file, we're going to clear away the old dependency list
	rm -f $(BUILD_DIR)/.depend
	# And output the dependencies of each file in $^ = $(SRC) to the file ./.depend
	$(CC) $(CFLAGS) -MM $^ -MF $(BUILD_DIR)/.depend;	 
		# e.g.: `g++ -std=gnu++11 -MM LayeredNeural_BackProp_Floats.cpp` outputs two lines:
			# LayeredNeural_BackProp_Floats.o: LayeredNeural_BackProp_Floats.cpp \
			#	../common.h Vectors.h NeuralNetwork.h AffineMatrix.h
		# Usefully, this is in Make format and includes both the cpp file itself and the various .h files.
# Now, next time you call `make` something, this line will include the correct dependencies list.
include $(BUILD_DIR)/.depend

# Now that we've collected our .o file dependency graph, we need to make sure the executable depends on all of the .o files
$(EXECUTABLE): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

# Clean up stuff.
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

