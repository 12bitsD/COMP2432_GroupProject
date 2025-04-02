#include <stdio.h>
#include <string.h>
#include "booking.h"
#include "output.h"

// Helper function to compute the end time given a start time and duration.
void compute_end_time(const char *start_time, float duration, char *end_time, int size) {
    int start_hour, start_minute;
    sscanf(start_time, "%d:%d", &start_hour, &start_minute);
    int total_minutes = start_hour * 60 + start_minute + (int)(duration * 60);
    int end_hour = (total_minutes / 60) % 24;
    int end_minute = total_minutes % 60;
    snprintf(end_time, size, "%02d:%02d", end_hour, end_minute);
}

// Helper function: Print bookings for a given algorithm and status, grouped by member.
void print_bookings_by_algo(int algo, int status_filter, const char *title) {
    printf("\n*** %s ***\n", title);
    const char *members[] = {"member_A", "member_B", "member_C", "member_D", "member_E"};
    int num_members = 5;

    for (int m = 0; m < num_members; m++) {
        int printed_any = 0;
        // Check if this member has any bookings with the specified algo and status.
        for (int i = 0; i < bookingCount; i++) {
            if (strcmp(bookingQueue[i].member, members[m]) == 0 &&
                bookingQueue[i].algorithm == algo &&
                bookingQueue[i].status == status_filter) {
                printed_any = 1;
                break;
            }
        }
        if (printed_any) {
            printf("\n%s has the following bookings:\n", members[m]);
            printf("Date         Start   End     Type         Essentials\n");
            printf("============================================================\n");
            for (int i = 0; i < bookingCount; i++) {
                if (strcmp(bookingQueue[i].member, members[m]) == 0 &&
                    bookingQueue[i].algorithm == algo &&
                    bookingQueue[i].status == status_filter) {
                    char end_time[6];
                    compute_end_time(bookingQueue[i].start_time, bookingQueue[i].duration, end_time, 6);
                    printf("%-12s %-7s %-7s %-12s ", bookingQueue[i].date, bookingQueue[i].start_time, end_time, bookingQueue[i].type);
                    if (strlen(bookingQueue[i].essentials[0]) == 0) {
                        printf("*\n");
                    } else {
                        for (int j = 0; j < MAX_ESSENTIALS; j++) {
                            if (strlen(bookingQueue[i].essentials[j]) > 0) {
                                printf("%s ", bookingQueue[i].essentials[j]);
                            }
                        }
                        printf("\n");
                    }
                }
            }
        }
    }
}

void print_booking_schedule_fcfs() {
    // Process using FCFS algorithm (0)
    process_requests(0);
    print_bookings_by_algo(0, STATUS_ACCEPTED, "Parking Booking – ACCEPTED / FCFS");
    print_bookings_by_algo(0, STATUS_REJECTED, "Parking Booking – REJECTED / FCFS");
}

void print_booking_schedule_prio() {
    // Process using Priority algorithm (1)
    process_requests(1);
    print_bookings_by_algo(1, STATUS_ACCEPTED, "Parking Booking – ACCEPTED / PRIO");
    print_bookings_by_algo(1, STATUS_REJECTED, "Parking Booking – REJECTED / PRIO");
}

void print_booking_schedule_opti() {
    // Process using Optimized algorithm (2)
    process_requests(2);
    print_bookings_by_algo(2, STATUS_ACCEPTED, "Parking Booking – ACCEPTED / OPTI");
    print_bookings_by_algo(2, STATUS_REJECTED, "Parking Booking – REJECTED / OPTI");
}

void print_all() {
    print_booking_schedule_fcfs();
    print_booking_schedule_prio();
    print_booking_schedule_opti();
    print_summary_report();
}

float calculate_facility_utilization(Facility *facilities, int num_facilities) {
    float total_hours = 0;
    for (int i = 0; i < num_facilities; i++) {
        for (int j = 0; j < facilities[i].booking_count; j++) {
            total_hours += facilities[i].bookings[j].duration;
        }
    }
    float total_available = num_facilities * 12 * 7; // total available hours per facility (84 hours)
    return (total_hours / total_available) * 100;
}

// Calculate parking slot utilization.
float calculate_utilization(Facility* facilities, int facility_count) {
    int total_slots = facility_count * 12 * 7; // 3个设施 * 12小时/天 * 7天
    int used_slots = 0;
    
    for (int i = 0; i < facility_count; i++) {
        for (int j = 0; j < facilities[i].booking_count; j++) {
            used_slots += (int)facilities[i].bookings[j].duration;
        }
    }
    
    return (used_slots * 100.0f) / total_slots;
}

float calculate_parking_utilization() {
    int total_slots = 10 * 12 * 7; // 10个停车位 * 12小时/天 * 7天
    int used_slots = 0;
    
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < parkingSlots[i].booking_count; j++) {
            used_slots += (int)parkingSlots[i].bookings[j].duration;
        }
    }
    
    return (used_slots * 100.0f) / total_slots;
}

void print_summary_report() {
    // Save original booking queue state
    int original_booking_count = bookingCount;
    Booking original_bookings[MAX_BOOKINGS];
    memcpy(original_bookings, bookingQueue, sizeof(Booking) * original_booking_count);

    // Process FCFS
    process_requests(0);
    int accepted_fcfs = 0, rejected_fcfs = 0;
    for (int i = 0; i < bookingCount; i++) {
        if (bookingQueue[i].status == STATUS_ACCEPTED) accepted_fcfs++;
        else if (bookingQueue[i].status == STATUS_REJECTED) rejected_fcfs++;
    }
    float locker_util_fcfs = calculate_facility_utilization(lockers, 3);
    float battery_util_fcfs = calculate_facility_utilization(batteries, 3);
    float parking_util_fcfs = calculate_parking_utilization();

    // Process Priority
    process_requests(1);
    int accepted_prio = 0, rejected_prio = 0;
    for (int i = 0; i < bookingCount; i++) {
        if (bookingQueue[i].status == STATUS_ACCEPTED) accepted_prio++;
        else if (bookingQueue[i].status == STATUS_REJECTED) rejected_prio++;
    }
    float locker_util_prio = calculate_facility_utilization(lockers, 3);
    float battery_util_prio = calculate_facility_utilization(batteries, 3);
    float parking_util_prio = calculate_parking_utilization();

    // Process Optimized
    process_requests(2);
    int accepted_opti = 0, rejected_opti = 0;
    for (int i = 0; i < bookingCount; i++) {
        if (bookingQueue[i].status == STATUS_ACCEPTED) accepted_opti++;
        else if (bookingQueue[i].status == STATUS_REJECTED) rejected_opti++;
    }
    float locker_util_opti = calculate_facility_utilization(lockers, 3);
    float battery_util_opti = calculate_facility_utilization(batteries, 3);
    float parking_util_opti = calculate_parking_utilization();

    // Restore original booking queue state
    memcpy(bookingQueue, original_bookings, sizeof(Booking) * original_booking_count);
    bookingCount = original_booking_count;

    // Output summary report
    printf("\n*** Parking Booking Manager – Summary Report ***\n");
    printf("For FCFS:\n");
    printf("    Total Bookings: %d\n", bookingCount);
    printf("    Accepted: %d\n", accepted_fcfs);
    printf("    Rejected: %d\n", rejected_fcfs);
    printf("    Utilization:\n");
    printf("        Lockers: %.1f%%\n", locker_util_fcfs);
    printf("        Batteries: %.1f%%\n", battery_util_fcfs);
    printf("        Parking Slots: %.1f%%\n", parking_util_fcfs);

    printf("For PRIO:\n");
    printf("    Total Bookings: %d\n", bookingCount);
    printf("    Accepted: %d\n", accepted_prio);
    printf("    Rejected: %d\n", rejected_prio);
    printf("    Utilization:\n");
    printf("        Lockers: %.1f%%\n", locker_util_prio);
    printf("        Batteries: %.1f%%\n", battery_util_prio);
    printf("        Parking Slots: %.1f%%\n", parking_util_prio);

    printf("For OPTI:\n");
    printf("    Total Bookings: %d\n", bookingCount);
    printf("    Accepted: %d\n", accepted_opti);
    printf("    Rejected: %d\n", rejected_opti);
    printf("    Utilization:\n");
    printf("        Lockers: %.1f%%\n", locker_util_opti);
    printf("        Batteries: %.1f%%\n", battery_util_opti);
    printf("        Parking Slots: %.1f%%\n", parking_util_opti);
}