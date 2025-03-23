#ifndef BOOKING_H
#define BOOKING_H

#define MAX_ESSENTIALS 3

typedef struct {
    char member[20];
    char type[15];
    char date[11];
    char start_time[6];
    float duration;
    int slot;
    char essentials[MAX_ESSENTIALS][20];
    int status;
} Booking;

// 全局请求队列
#define MAX_BOOKINGS 100
extern Booking bookingQueue[MAX_BOOKINGS];
extern int bookingCount;

#endif // BOOKING_H