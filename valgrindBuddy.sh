#!/bin/bash

# Compile BuddySystem
gcc -g -o BuddySystem/bin/buddysystem BuddySystem/main.c BuddySystem/src/*.c -lm

# Change directory to BuddySystem/bin
cd BuddySystem/bin


# Run the executable with valgrind
valgrind -s --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./buddysystem