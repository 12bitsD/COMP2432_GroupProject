#ifndef BOOKING_H
#define BOOKING_H

#define MAX_ESSENTIALS 6

// 设施管理结构
typedef struct {
    int is_occupied;
    char booking_id[20];
    char date[11];
    float start_hour;
    float duration;
} Facility;

// 停车位管理
typedef struct {
    int booking_count;
    struct {
        char booking_id[20];
        char date[11];
        float start_hour;
        float duration;
    } bookings[5];
} ParkingSlot;

// 预约类型优先级枚举
typedef enum {
    EVENT,       // 最高优先级
    RESERVATION, // 次高优先级
    PARKING,     // 第三优先级
    ESSENTIALS   // 最低优先级
} BookingTypePriority;

// 预约结构体
typedef struct {
    char member[20];
    char type[15];
    char date[11];
    char start_time[6];
    float duration;
    int slot;
    char essentials[MAX_ESSENTIALS][20];
    int status; // 0: 待处理, 1: 接受, -1: 拒绝
    BookingTypePriority priority; // 预约优先级
} Booking;

// 全局请求队列
#define MAX_BOOKINGS 100
extern Booking bookingQueue[MAX_BOOKINGS];
extern int bookingCount;

// 全局资源数组
extern ParkingSlot parkingSlots[10];
extern Facility lockers[3];
extern Facility umbrellas[3];
extern Facility batteries[3];
extern Facility cables[3];
extern Facility valetParking[3];
extern Facility inflationServices[3];

// 函数声明
void process_requests();
void release_resources();
// 算法设置与处理函数
void set_scheduling_algorithm(SchedulingAlgorithm algorithm);
void process_requests_fcfs();
void process_requests_prio();
void handle_preemption(int current_idx, int day, int start_hour, float duration);
void release_booking_resources(int booking_idx);
void reset_scheduling_results();

// 资源管理函数
void release_essentials(Booking booking);

#endif // BOOKING_H