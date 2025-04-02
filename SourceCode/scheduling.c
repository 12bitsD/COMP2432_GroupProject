#include "booking.h"
#include "scheduling.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

// Time slot management (7 days, 24 hours each)
static bool time_slots[7][24] = {{0}}; // 添加额外的大括号

// 修改类型以匹配booking.h中的声明
ParkingSlot parkingSlots[10] = {{0}};

// 修改其他设施的初始化方式
Facility lockers[3] = {{0}};
Facility umbrellas[3] = {{0}};
Facility batteries[3] = {{0}};
Facility cables[3] = {{0}};
Facility valetParking[3] = {{0}};
Facility inflationServices[3] = {{0}};

// Days in month for date validation
static int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// Date validation: only allow bookings for 10-16 May 2025.
static bool is_valid_date(int year, int month, int day) {
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > days_in_month[month]) return false;
    if (year != 2025 || month != 5 || day < 10 || day > 16) return false;
    return true;
}

// Parse date and compute the day index (0=Sunday, …, 6=Saturday) using Zeller's formula.
static int parse_date_to_day(const char* date) {
    int year, month, day;
    if (sscanf(date, "%d-%d-%d", &year, &month, &day) != 3) {
        return -1;
    }

    if (!is_valid_date(year, month, day)) {
        return -1;
    }

    if (month == 1 || month == 2) {
        month += 12;
        year--;
    }
    int h = (day + 13*(month+1)/5 + year + year/4 - year/100 + year/400) % 7;
    return (h + 5) % 7; // convert so that Sunday=0, Monday=1, ..., Saturday=6
}

// Check for time conflict for a booking.
static bool check_time_conflict(const char* date, int start_hour, float duration) {
    int day = parse_date_to_day(date);
    if (day == -1) return true; // invalid date

    // Allowed time range: 08:00-20:00
    if (start_hour < 8 || start_hour >= 20) return true;

    int end_hour = start_hour + (int)duration;
    if (end_hour > 20) return true;

    for (int h = start_hour; h < end_hour; h++) {
        if (h >= 0 && h < 24 && time_slots[day][h]) return true;
    }

    return false;
}

// Check if a facility is available (no time conflict) for the given booking.
static bool is_facility_available(Facility* facility, const char* date, float start_hour, float duration) {
    if (facility->booking_count == 0) {
        return true;
    }

    float end_hour_request = start_hour + duration;
    for (int i = 0; i < facility->booking_count; i++) {
        if (strcmp(facility->bookings[i].date, date) == 0) {
            float start_hour_existing = facility->bookings[i].start_hour;
            float end_hour_existing = start_hour_existing + facility->bookings[i].duration;
            if ((start_hour < end_hour_existing) && (end_hour_request > start_hour_existing)) {
                return false;
            }
        }
    }

    return true;
}

// Record a facility booking.
static void record_facility_booking(Facility* facility, const char* member_id,
                                    const char* date, float start_hour, float duration) {
    if (facility->booking_count >= 84) {
        printf("Warning: Facility booking capacity reached!\n");
        return;
    }

    int idx = facility->booking_count;
    strcpy(facility->bookings[idx].booking_id, member_id);
    strcpy(facility->bookings[idx].date, date);
    facility->bookings[idx].start_hour = start_hour;
    facility->bookings[idx].duration = duration;
    facility->booking_count++;
    

}



// Record parking slot booking
static bool allocate_essentials(char essentials[][20], BookingTypePriority priority,
                            const char* member_id, const char* date, const char* start_time, float duration) {
    int resource_index = -1;
    float start_hour = atof(start_time);
    
    // Find available resource pair
    for (int j = 0; j < 3; j++) {
        bool is_available = false;
        switch(priority) {
            case EVENT:
                is_available = is_facility_available(&lockers[j], date, start_hour, duration) &&
                               is_facility_available(&umbrellas[j], date, start_hour, duration);
                break;
            case RESERVATION:
            case PARKING:
                is_available = is_facility_available(&batteries[j], date, start_hour, duration) &&
                               is_facility_available(&cables[j], date, start_hour, duration);
                break;
            case ESSENTIALS:
                is_available = is_facility_available(&valetParking[j], date, start_hour, duration) &&
                               is_facility_available(&inflationServices[j], date, start_hour, duration);
                break;
        }

        if (is_available) {
            resource_index = j;
            break;
        }
    }
    
    if (resource_index == -1) return false; // No available facility pair found

    // Find an available parking slot
    int slot_index = -1;
    for (int i = 0; i < 10; i++) {
        if (parkingSlots[i].booking_count < 10) {
            slot_index = i;
            break;
        }
    }

    if (slot_index == -1) return false; // No available parking slot

    // Record parking slot booking
    record_facility_booking((Facility*)&parkingSlots[slot_index], member_id, date, start_hour, duration);

    switch(priority) {
        case EVENT:
            record_facility_booking(&lockers[resource_index], member_id, date, start_hour, duration);
            record_facility_booking(&umbrellas[resource_index], member_id, date, start_hour, duration);
            break;
        case RESERVATION:
        case PARKING:
            // 修改这里，确保记录电池和充电线的使用
            record_facility_booking(&batteries[resource_index], member_id, date, start_hour, duration);
            record_facility_booking(&cables[resource_index], member_id, date, start_hour, duration);
            break;
        case ESSENTIALS:
            record_facility_booking(&valetParking[resource_index], member_id, date, start_hour, duration);
            record_facility_booking(&inflationServices[resource_index], member_id, date, start_hour, duration);
            break;
    }

    return true;
}

    // Get the booking priority based on the type string.
static BookingTypePriority get_booking_priority(const char* type) {
    if (strcmp(type, "Event") == 0) return EVENT;
    if (strcmp(type, "Reservation") == 0) return RESERVATION;
    if (strcmp(type, "Parking") == 0) return PARKING;
    return ESSENTIALS; // default lowest priority
}
// Comparator for sorting by priority (higher priority first).
static int priority_comparator(const void* a, const void* b) {
    const Booking* bookingA = (const Booking*)a;
    const Booking* bookingB = (const Booking*)b;
    if (bookingA->priority != bookingB->priority) {
        return bookingA->priority - bookingB->priority;
    }
    return (bookingA - bookingQueue) - (bookingB - bookingQueue);
}

    // FCFS scheduling: process bookings in the order they were added.
void process_requests_fcfs() {
    for (int i = 0; i < bookingCount; i++) {
        if (bookingQueue[i].status == STATUS_PENDING) {
            int day = parse_date_to_day(bookingQueue[i].date);
            // 修改这里：使用 atof 替代 atoi 来正确处理时间
            float start_hour = atof(bookingQueue[i].start_time);
            float duration = bookingQueue[i].duration;
            
            bookingQueue[i].priority = get_booking_priority(bookingQueue[i].type);
            
            if (!check_time_conflict(bookingQueue[i].date, start_hour, duration) &&
                allocate_essentials(bookingQueue[i].essentials, bookingQueue[i].priority,
                                    bookingQueue[i].member, bookingQueue[i].date,
                                    bookingQueue[i].start_time, duration)) {
                float end_hour = start_hour + duration;
                for (int h = start_hour; h < end_hour; h++) {
                    time_slots[day][h] = true;
                }
                bookingQueue[i].status = STATUS_ACCEPTED;
            } else {
                bookingQueue[i].status = STATUS_REJECTED;
            }
        }
    }
}

    // Priority scheduling: sort by priority (and preserve arrival order for same priority), then use FCFS logic.
void process_requests_prio() {
    for (int i = 0; i < bookingCount; i++) {
        bookingQueue[i].priority = get_booking_priority(bookingQueue[i].type);
    }
    // Add sorting by priority
    qsort(bookingQueue, bookingCount, sizeof(Booking), priority_comparator);
    process_requests_fcfs();
}
// Comparator for sorting by duration (shortest first), then by priority.
static int duration_comparator(const void* a, const void* b) {
    const Booking* bookingA = (const Booking*)a;
    const Booking* bookingB = (const Booking*)b;

    if (bookingA->duration != bookingB->duration) {
        return bookingA->duration > bookingB->duration ? 1 : -1;
    }

    if (bookingA->priority != bookingB->priority) {
        return bookingB->priority - bookingA->priority;
    }

    return (bookingA - bookingQueue) - (bookingB - bookingQueue);
}
    // Optimized scheduling: sort by shortest duration (to fit more bookings), then by priority.
void process_requests_opti() {
    for (int i = 0; i < bookingCount; i++) {
        if (bookingQueue[i].status == STATUS_PENDING) {
            bookingQueue[i].priority = get_booking_priority(bookingQueue[i].type);
        }
    }
    qsort(bookingQueue, bookingCount, sizeof(Booking), duration_comparator);
    process_requests_fcfs();
}
// Reset scheduling results and resource usage.
void reset_scheduling_results() {
    memset(time_slots, 0, sizeof(time_slots));

    for (int i = 0; i < 10; i++) {
        parkingSlots[i].booking_count = 0;
    }

    for (int i = 0; i < 3; i++) {
        lockers[i].booking_count = 0;
        umbrellas[i].booking_count = 0;
        batteries[i].booking_count = 0;
        cables[i].booking_count = 0;
        valetParking[i].booking_count = 0;
        inflationServices[i].booking_count = 0;
    }

    for (int i = 0; i < bookingCount; i++) {
        bookingQueue[i].status = STATUS_PENDING;
    }
}

// Main process_requests function: resets, applies the selected algorithm, and tags bookings with the algorithm used.
void process_requests(int algo) {
    reset_scheduling_results();
    switch (algo) {
        case ALGO_FCFS:
            process_requests_fcfs();
            break;
        case ALGO_PRIO:
            process_requests_prio();
            break;
        case ALGO_OPTI:
            process_requests_opti();
            break;
        default:
            printf("Unknown algorithm selected\n");
            break;
    }
    // Tag each processed booking with the algorithm used.
    for (int i = 0; i < bookingCount; i++) {
        if (bookingQueue[i].status != STATUS_PENDING) {
            bookingQueue[i].algorithm = algo;
        }
    }
}

// Stub for release_essentials (if required by further extensions)
void release_essentials(Booking booking) {
    // Placeholder: implement resource release logic if needed.
    printf("Releasing resources for booking by %s\n", booking.member);
}
