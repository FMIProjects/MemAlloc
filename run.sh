#!/bin/bash

# Create directories
mkdir -p bin
mkdir -p FitAlgo/bin
mkdir -p BuddySystem/bin

# Compile FitAlgo
gcc -g -o FitAlgo/bin/fitalgo FitAlgo/main.c FitAlgo/src/*.c

# Compile BuddySystem
gcc -g -o BuddySystem/bin/buddysystem BuddySystem/main.c BuddySystem/src/*.c -lm

# Compile main
gcc -g -o bin/main main.c

# Change directory to bin
cd bin/

# Run the executable
./main
