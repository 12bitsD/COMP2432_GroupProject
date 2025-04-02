#ifndef BOOKING_H
#define BOOKING_H

#define MAX_ESSENTIALS 6

// Facility structure for managing paired resources
typedef struct {
    int booking_count;  // current booking count
    struct {
        char booking_id[20];
        char date[11];
        float start_hour;
        float duration;
    } bookings[84];
} Facility;

// Parking slot management structure
typedef struct {
    int booking_count;
    struct {
        char booking_id[20];
        char date[11];
        float start_hour;
        float duration;
    } bookings[84];
} ParkingSlot;

// Booking type priority enumeration
typedef enum {
    EVENT,       // highest priority
    RESERVATION, // second highest priority
    PARKING,     // third priority
    ESSENTIALS   // lowest priority
} BookingTypePriority;

// Booking structure
typedef struct {
    char member[20];
    char type[15];      // e.g., "Parking", "Reservation", "Event", "Essentials"
    char date[11];      // format: YYYY-MM-DD
    char start_time[6]; // format: HH:MM
    float duration;
    int slot;
    char essentials[MAX_ESSENTIALS][20];
    int status;         // 0: pending, 1: accepted, -1: rejected
    BookingTypePriority priority; // booking priority
    int algorithm;      // scheduling algorithm used: 0: FCFS, 1: PRIO, 2: OPTI
} Booking;

// Status codes
#define STATUS_PENDING 0
#define STATUS_ACCEPTED 1
#define STATUS_REJECTED -1

// Global booking queue
#define MAX_BOOKINGS 100
extern Booking bookingQueue[MAX_BOOKINGS];
extern int bookingCount;

// Global resource arrays
extern ParkingSlot parkingSlots[10];
extern Facility lockers[3];
extern Facility umbrellas[3];
extern Facility batteries[3];
extern Facility cables[3];
extern Facility valetParking[3];
extern Facility inflationServices[3];

// Function declarations for scheduling and resource management
void process_requests(int algo);
void process_requests_fcfs();
void process_requests_prio();
void process_requests_opti();
void reset_scheduling_results();
void release_essentials(Booking booking); // Stub (if needed)

#endif // BOOKING_H