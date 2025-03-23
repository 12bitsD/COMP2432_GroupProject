#include <stdio.h>
#include <string.h>
#include "booking.h"

void handle_command(const char* command);

int main() {
    char input[256];
    bookingCount = 0; // 初始化全局计数器
    
    printf("~~ WELCOME TO PolyU ~~\n");
    printf("Please enter booking: ");
    while (fgets(input, sizeof(input), stdin)) {
        // 去除换行符
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