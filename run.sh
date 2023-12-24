#!/bin/bash

# Create directories
mkdir -p bin
mkdir -p FitAlgo/bin
mkdir -p BuddySystem/bin

# Compile FitAlgo
gcc -o FitAlgo/bin/fitalgo FitAlgo/main.c FitAlgo/src/*.c

# Compile BuddySystem
# TBA

# Compile main
gcc -o bin/main main.c

# Change directory to bin
cd bin/

# Run the executable
./main