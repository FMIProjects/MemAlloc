#!/bin/bash

# Compile FitAlgo
gcc -g -o FitAlgo/bin/fitalgo FitAlgo/main.c FitAlgo/src/*.c

# Change directory to BuddySystem/bin
cd FitAlgo/bin


# Run the executable with valgrind
valgrind -s --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./fitalgo