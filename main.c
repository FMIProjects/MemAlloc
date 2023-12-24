#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// with ANSI escape sequence
// Colors
#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"

// Reset Current Color
#define CRESET "\e[0m"

int main()
{
    int input;

    // Display menu
    system("clear");
    printf(RED " __  __                         _ _            \n");
    printf("|  \\/  |                  /\\   | | |           \n");
    printf("| \\  / | ___ _ __ ___    /  \\  | | | ___   ___ \n");
    printf("| |\\/| |/ _ \\ '_ ` _ \\  / /\\ \\ | | |/ _ \\ / __|\n");
    printf("| |  | |  __/ | | | | |/ ____ \\| | | (_) | (__ \n");
    printf("|_|  |_|\\___|_| |_| |_/_/    \\_\\_|_|\\___/ \\___|\n" CRESET);
    printf("================================================\n");
    printf("Select a program to run:\n");
    printf("================================================\n");
    printf(BLU "1. FitAlgo\n");
    printf("2. BuddySystem\n");
    printf("3. Exit\n" CRESET);
    printf("================================================\n");
    printf("Enter your choice: ");
    scanf("%d", &input);

    pid_t pid;

    switch (pid = fork())
    {
    case -1:
        perror("fork");
        exit(1);

    case 0:
        switch (input)
        {
        case 1:
            // Run the FitAlgo program
            execl("../FitAlgo/bin/fitalgo", "fitalgo", (char *)NULL);
            perror("execl");
            exit(1);
            break;

        case 2:
            // Run the BuddySystem program
            execl("../BuddySystem/bin/buddysystem", "buddysystem", (char *)NULL);
            perror("execl");
            exit(1);
            break;

        case 3:
            exit(0);
            break;

        default:
            exit(1);
            break;
        }

    default:
        // Parent process
        wait(NULL); // Wait for the child to finish
        break;
    }
    exit(0);
}