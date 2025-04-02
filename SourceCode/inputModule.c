#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "booking.h"
#include "output.h"
#include "inputModule.h"

Booking bookingQueue[MAX_BOOKINGS];
int bookingCount = 0;

// Adds a new booking to the global queue.
void add_queue(Booking newBooking) {
    if (bookingCount < MAX_BOOKINGS) {
        bookingQueue[bookingCount++] = newBooking;
        printf("Pending\n");
    } else {
        printf("Error: Booking queue is full\n");
    }
}

void parse_parking(char* main_cmd) {
    char* token = strtok(main_cmd, " ");
    // 在parse_parking函数中
    Booking newBooking = {{0}}; // 添加额外的大括号
    newBooking.status = STATUS_PENDING;
    newBooking.priority = PARKING; // set priority for parking
    strcpy(newBooking.type, "Parking");

    int param_index = 0;
    while ((token = strtok(NULL, " ")) != NULL) {
        switch(param_index) {
            case 0: // member ID (skip leading '-' character)
                strncpy(newBooking.member, token + 1, sizeof(newBooking.member)-1);
                break;
            case 1: // date
                strcpy(newBooking.date, token);
                break;
            case 2: // start time
                strcpy(newBooking.start_time, token);
                break;
            case 3: // duration
                newBooking.duration = atof(token);
                break;
            default: // additional essentials
                if (param_index - 4 < MAX_ESSENTIALS) {
                    strcpy(newBooking.essentials[param_index - 4], token);
                } else {
                    printf("Error: Too many facilities specified\n");
                    return;
                }
                break;
        }
        param_index++;
    }
    // Check for required essentials: battery and cable
    bool has_battery = false;
    bool has_cable = false;

    for (int i = 0; i < MAX_ESSENTIALS && newBooking.essentials[i][0] != '\0'; i++) {
        if (strcmp(newBooking.essentials[i], "battery") == 0) {
            has_battery = true;
        } else if (strcmp(newBooking.essentials[i], "cable") == 0) {
            has_cable = true;
        }
    }

    if (has_battery && has_cable) {
        add_queue(newBooking);
    } else {
        printf("Error: Parking requires both battery and cable facilities\n");
    }
}

void parse_reservation(char* main_cmd) {
    char* token = strtok(main_cmd, " ");
    // 在parse_reservation函数中
    Booking newBooking = {{0}}; // 添加额外的大括号
    newBooking.status = STATUS_PENDING;
    strcpy(newBooking.type, "Reservation");

    int param_index = 0;
    while ((token = strtok(NULL, " ")) != NULL) {
        switch(param_index) {
            case 0: // member ID
                strncpy(newBooking.member, token + 1, sizeof(newBooking.member)-1);
                break;
            case 1: // date
                strcpy(newBooking.date, token);
                break;
            case 2: // start time
                strcpy(newBooking.start_time, token);
                break;
            case 3: // duration
                newBooking.duration = atof(token);
                break;
            default: // facilities
                if (param_index - 4 < MAX_ESSENTIALS) {
                    strcpy(newBooking.essentials[param_index - 4], token);
                } else {
                    printf("Error: Too many facilities specified\n");
                    return;
                }
                break;
        }
        param_index++;
    }
    // Check for required essentials: battery and cable
    bool has_battery = false;
    bool has_cable = false;

    for (int i = 0; i < MAX_ESSENTIALS && newBooking.essentials[i][0] != '\0'; i++) {
        if (strcmp(newBooking.essentials[i], "battery") == 0) {
            has_battery = true;
        } else if (strcmp(newBooking.essentials[i], "cable") == 0) {
            has_cable = true;
        }
    }

    if (has_battery && has_cable) {
        add_queue(newBooking);
    } else {
        printf("Error: Reservation requires both battery and cable facilities\n");
    }
}

void parse_event(char* main_cmd) {
    char* token = strtok(main_cmd, " ");
    // 在parse_event函数中
    Booking newBooking = {{0}}; // 添加额外的大括号
    newBooking.priority = EVENT;  // high priority event
    newBooking.status = STATUS_PENDING;
    strcpy(newBooking.type, "Event");

    int param_index = 0;
    while ((token = strtok(NULL, " ")) != NULL) {
        switch(param_index) {
            case 0: // member ID
                strncpy(newBooking.member, token + 1, sizeof(newBooking.member)-1);
                break;
            case 1: // date
                strcpy(newBooking.date, token);
                break;
            case 2: // start time
                strcpy(newBooking.start_time, token);
                break;
            case 3: // duration
                newBooking.duration = atof(token);
                break;
            default: // facilities
                if (param_index - 4 < MAX_ESSENTIALS) {
                    strcpy(newBooking.essentials[param_index - 4], token);
                } else {
                    printf("Error: Too many facilities specified\n");
                    return;
                }
                break;
        }
        param_index++;
    }
    // Check for required essentials: umbrella and valetpark
    bool has_umbrella = false;
    bool has_valetpark = false;

    for (int i = 0; i < MAX_ESSENTIALS && newBooking.essentials[i][0] != '\0'; i++) {
        if (strcmp(newBooking.essentials[i], "umbrella") == 0) has_umbrella = true;
        if (strcmp(newBooking.essentials[i], "valetpark") == 0) has_valetpark = true;
    }

    if (has_umbrella && has_valetpark) {
        add_queue(newBooking);
    } else {
        printf("Error: Event requires both umbrella and valetpark facilities\n");
    }
}

void parse_essentials(char* main_cmd) {
    char* token = strtok(main_cmd, " ");
    // 在parse_essentials函数中
    Booking newBooking = {{0}}; // 添加额外的大括号
    newBooking.priority = ESSENTIALS;  // lowest priority
    newBooking.status = STATUS_PENDING;
    strcpy(newBooking.type, "Essentials");

    int param_index = 0;
    while ((token = strtok(NULL, " ")) != NULL) {
        switch(param_index) {
            case 0: // member ID
                strncpy(newBooking.member, token + 1, sizeof(newBooking.member)-1);
                break;
            case 1: // date
                strcpy(newBooking.date, token);
                break;
            case 2: // start time
                strcpy(newBooking.start_time, token);
                break;
            case 3: // duration
                newBooking.duration = atof(token);
                break;
            default: // facilities
                if (param_index - 4 < MAX_ESSENTIALS) {
                    strcpy(newBooking.essentials[param_index - 4], token);
                } else {
                    printf("Error: Too many facilities specified\n");
                    return;
                }
                break;
        }
        param_index++;
    }

    add_queue(newBooking);
}

void handle_command(const char* command) {
    char cmd_copy[256];
    strcpy(cmd_copy, command);

    // Main command (commands are terminated by ';')
    char* main_cmd = strtok(cmd_copy, ";");

    if (strncmp(main_cmd, "addParking", 10) == 0) {
        parse_parking(main_cmd);
    }
    else if (strncmp(main_cmd, "addReservation", 14) == 0) {
        parse_reservation(main_cmd);
    }
    else if (strncmp(main_cmd, "addEvent", 8) == 0) {
        parse_event(main_cmd);
    }
    else if (strncmp(main_cmd, "bookEssentials", 14) == 0) {
        parse_essentials(main_cmd);
    }
    else if (strncmp(main_cmd, "addBatch", 8) == 0) {
        import_batch(main_cmd);
    }
    else if (strncmp(main_cmd, "printBookings -fcfs", 19) == 0) {
        print_booking_schedule_fcfs();
    }
    else if (strncmp(main_cmd, "printBookings -prio", 19) == 0) {
        print_booking_schedule_prio();
    }
    else if (strncmp(main_cmd, "printBookings -opti", 19) == 0) {
        print_booking_schedule_opti();
    }
    else if (strncmp(main_cmd, "printBookings -ALL", 18) == 0) {
        print_all();
    }
    else if(strcmp(main_cmd, "endProgram") == 0){
        exit(0);
    }
    else {
        printf("Unknown command: %s\n", main_cmd);
    }
}

void import_batch(char* filename) {
    // Extract filename from parameters
    char* batch_file = strtok(filename, " ");
    batch_file = strtok(NULL, " ");

    // Remove the leading '-' if present
    if (batch_file != NULL && batch_file[0] == '-') {
        batch_file++;
    }

    FILE* file = fopen(batch_file, "r");
    if (file == NULL) {
        printf("Error: Cannot open batch file %s\n", batch_file);
        return;
    }

    char line[256];
    int count = 0;

    while (fgets(line, sizeof(line), file)) {
        // Remove newline character
        line[strcspn(line, "\n")] = '\0';

        if (strlen(line) > 0) {
            handle_command(line);
            count++;
        }
    }

    fclose(file);
    printf("Imported %d commands from %s\n", count, batch_file);
}


