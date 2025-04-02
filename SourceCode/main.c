#include "inputModule.h"
#include <stdio.h>
#include <string.h>

int main() {
    char input[256];
    bookingCount = 0; // Initialize booking count

    printf("~~ WELCOME TO PolyU ~~\n");
    printf("Please enter booking: ");
    while (fgets(input, sizeof(input), stdin)) {
        // Remove newline character
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            break;
        }

        if (strlen(input) > 0) {
            handle_command(input);
        }

        printf("Please enter booking: ");
    }
    return 0;
}