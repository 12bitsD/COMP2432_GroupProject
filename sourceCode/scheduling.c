#include "booking.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// 时间槽管理（7天，每天24小时）
static bool time_slots[7][24] = {0};

// 全局资源数组定义
ParkingSlot parkingSlots[10] = {0};
Facility lockers[3] = {0};
Facility umbrellas[3] = {0};
Facility batteries[3] = {0};
Facility cables[3] = {0};
Facility valetParking[3] = {0};
Facility inflationServices[3] = {0};
static int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
// 全局变量跟踪当前使用的算法
typedef enum {
    ALG_FCFS,
    ALG_PRIO,
    ALG_OPTI
} SchedulingAlgorithm;



// 全局变量跟踪当前使用的算法
static SchedulingAlgorithm current_algorithm = ALG_FCFS;


static bool is_valid_date(int year, int month, int day) {
    // 检查月份范围
    if (month < 1 || month > 12) return false;

    // 检查日期范围
    if (day < 1 || day > days_in_month[month]) return false;
    
    // 项目特定要求：只接受2025年5月10日至16日的预约
    if (year != 2025 || month != 5 || day < 10 || day > 16) return false;
    
    return true;
}

// 日期解析函数
static int parse_date_to_day(const char* date) {
    int year, month, day;
    if (sscanf(date, "%d-%d-%d", &year, &month, &day) != 3) {
        return -1; // 格式错误
    }
    
    if (!is_valid_date(year, month, day)) {
        return -1; // 无效日期
    }
    
    // 计算星期几 - Zeller公式
    if (month == 1 || month == 2) {
        month += 12;
        year--;
    }
    int h = (day + 13*(month+1)/5 + year + year/4 - year/100 + year/400) % 7;
    return (h + 5) % 7; // 转换为周日=0, 周一=1,...周六=6
}

// 检查时间冲突
static bool check_time_conflict(const char* date, int start_hour, float duration) {
    int day = parse_date_to_day(date);
    if (day == -1) return true; // 无效日期
    
    // 检查时间是否在允许范围内（8:00-20:00）
    if (start_hour < 8 || start_hour >= 20) return true;
    
    int end_hour = start_hour + (int)duration;
    if (end_hour > 20) return true; // 结束时间超过20:00
    
    // 检查时间槽是否已被占用
    for (int h = start_hour; h < end_hour; h++) {
        if (h >= 0 && h < 24 && time_slots[day][h]) return true;
    }
    
    return false;
}

// 分配设备资源
static bool allocate_essentials(const char essentials[][20], BookingTypePriority priority, const char* member_id, const char* date, const char* start_time, float duration) {
    int resource_index = -1;

    // 根据预约类型选择设施配对
    switch(priority) {
        case EVENT:
            // 检查locker和umbrella配对
            for (int j = 0; j < 3; j++) {
                if (!lockers[j].is_occupied && !umbrellas[j].is_occupied) {
                    resource_index = j;
                    break;
                }
            }
            break;
        
        case RESERVATION:
        case PARKING:
            // 检查battery和cable配对
            for (int j = 0; j < 3; j++) {
                if (!batteries[j].is_occupied && !cables[j].is_occupied) {
                    resource_index = j;
                    break;
                }
            }
            break;
        
        case ESSENTIALS:
            // 检查valetParking和inflationServices配对
            for (int j = 0; j < 3; j++) {
                if (!valetParking[j].is_occupied && !inflationServices[j].is_occupied) {
                    resource_index = j;
                    break;
                }
            }
            break;
    }

    if (resource_index != -1) {
        // 分配停车位
        int slot_index = -1;
        for (int i = 0; i < 10; i++) {
            if (parkingSlots[i].booking_count < 5) { // 每个停车位最多5个预约
                slot_index = i;
                break;
            }
        }
        
        if (slot_index == -1) return false; // 没有可用停车位
        
        // 记录停车位预约信息
        int booking_idx = parkingSlots[slot_index].booking_count;
        strcpy(parkingSlots[slot_index].bookings[booking_idx].booking_id, member_id);
        strcpy(parkingSlots[slot_index].bookings[booking_idx].date, date);
        parkingSlots[slot_index].bookings[booking_idx].start_hour = atof(start_time);
        parkingSlots[slot_index].bookings[booking_idx].duration = duration;
        parkingSlots[slot_index].booking_count++;
        
        // 根据预约类型分配设施
        switch(priority) {
            case EVENT:
                lockers[resource_index].is_occupied = 1;
                umbrellas[resource_index].is_occupied = 1;
                strcpy(lockers[resource_index].booking_id, member_id);
                strcpy(umbrellas[resource_index].booking_id, member_id);
                strcpy(lockers[resource_index].date, date);
                strcpy(umbrellas[resource_index].date, date);
                lockers[resource_index].start_hour = atof(start_time);
                umbrellas[resource_index].start_hour = atof(start_time);
                lockers[resource_index].duration = duration;
                umbrellas[resource_index].duration = duration;
                break;
                
            case RESERVATION:

            case PARKING:
                batteries[resource_index].is_occupied = 1;
                cables[resource_index].is_occupied = 1;
                strcpy(batteries[resource_index].booking_id, member_id);
                strcpy(cables[resource_index].booking_id, member_id);
                strcpy(batteries[resource_index].date, date);
                strcpy(cables[resource_index].date, date);
                batteries[resource_index].start_hour = atof(start_time);
                cables[resource_index].start_hour = atof(start_time);
                batteries[resource_index].duration = duration;
                cables[resource_index].duration = duration;
                break;
                
            case ESSENTIALS:
                valetParking[resource_index].is_occupied = 1;
                inflationServices[resource_index].is_occupied = 1;
                strcpy(valetParking[resource_index].booking_id, member_id);
                strcpy(inflationServices[resource_index].booking_id, member_id);
                strcpy(valetParking[resource_index].date, date);
                strcpy(inflationServices[resource_index].date, date);
                valetParking[resource_index].start_hour = atof(start_time);
                inflationServices[resource_index].start_hour = atof(start_time);
                valetParking[resource_index].duration = duration;
                inflationServices[resource_index].duration = duration;
                break;
        }
        return true;
    }
    return false;
}

// 确定预约类型的优先级
static BookingTypePriority get_booking_priority(const char* type) {
    if (strcmp(type, "event") == 0) return EVENT;
    if (strcmp(type, "reservation") == 0) return RESERVATION;
    if (strcmp(type, "parking") == 0) return PARKING;
    return ESSENTIALS; // 默认最低优先级
}


// FCFS算法实现
void process_requests_fcfs() {
    // 按预约添加顺序处理
    for (int i = 0; i < bookingCount; i++) {
        if (bookingQueue[i].status == 0) { // 只处理未处理的预约
            int day = parse_date_to_day(bookingQueue[i].date);
            
            float start_hour = atoi(bookingQueue[i].start_time);
            float duration = bookingQueue[i].duration;
            // 检查时间冲突和资源可用性
            if (!check_time_conflict(bookingQueue[i].date, start_hour, duration) && 
                allocate_essentials(bookingQueue[i].essentials, bookingQueue[i].priority, 
                                    bookingQueue[i].member, bookingQueue[i].date, 
                                    bookingQueue[i].start_time, duration)) {
                
                // 标记时间槽为已占用
                float end_hour = start_hour + (float)duration;
                for (int h = start_hour; h < end_hour; h++1) {
                    time_slots[day][h] = true;
                }
                
                bookingQueue[i].status = 1; // 接受
            } else {
                bookingQueue[i].status = -1; // 拒绝
            }
        }
    }
}


// 全局资源释放函数
void release_resources() {
    for (int i = 0; i < bookingCount; i++) {
        if (bookingQueue[i].status == -1) {
            int day = parse_date_to_day(bookingQueue[i].date);
            if (day != -1) { // 只处理有效日期
                int start_hour = atoi(bookingQueue[i].start_time);
                int end_hour = start_hour + (int)bookingQueue[i].duration;
                
                // 释放时间槽
                for (int h = start_hour; h < end_hour; h++) {
                    time_slots[day][h] = false;
                }
            }
            
            // 释放设施资源
            release_essentials(bookingQueue[i]);
        }
    }
}

void release_essentials(Booking booking) {
    // 释放停车位 (现有代码)
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < parkingSlots[i].booking_count; j++) {
            if (strcmp(parkingSlots[i].bookings[j].booking_id, booking.member) == 0 &&
                strcmp(parkingSlots[i].bookings[j].date, booking.date) == 0 &&
                parkingSlots[i].bookings[j].start_hour == atof(booking.start_time)) {
                
                // 移除这个预约（将后面的预约前移）
                for (int k = j; k < parkingSlots[i].booking_count - 1; k++) {
                    parkingSlots[i].bookings[k] = parkingSlots[i].bookings[k + 1];
                }
                parkingSlots[i].booking_count--;
                break;
            }
        }
    }
    
    // 释放设施资源 (新增代码)
    float start_hour = atof(booking.start_time);
    
    // 检查并释放所有可能的设施
    for (int i = 0; i < 3; i++) {
        // 释放locker和umbrella
        if (strcmp(lockers[i].booking_id, booking.member) == 0 &&
            strcmp(lockers[i].date, booking.date) == 0 &&
            lockers[i].start_hour == start_hour) {
            lockers[i].is_occupied = 0;
            umbrellas[i].is_occupied = 0;
        }
        
        // 释放battery和cable
        if (strcmp(batteries[i].booking_id, booking.member) == 0 &&
            strcmp(batteries[i].date, booking.date) == 0 &&
            batteries[i].start_hour == start_hour) {
            batteries[i].is_occupied = 0;
            cables[i].is_occupied = 0;
        }
        
        // 释放valetParking和inflationServices
        if (strcmp(valetParking[i].booking_id, booking.member) == 0 &&
            strcmp(valetParking[i].date, booking.date) == 0 &&
            valetParking[i].start_hour == start_hour) {
            valetParking[i].is_occupied = 0;
            inflationServices[i].is_occupied = 0;
        }
    }
}

// 释放特定预约的所有资源
void release_booking_resources(int booking_idx) {
    Booking* booking = &bookingQueue[booking_idx];
    int day = parse_date_to_day(booking->date);
    int start_hour = atoi(booking->start_time);
    int end_hour = start_hour + (int)booking->duration;
    
    // 释放时间槽
    for (int h = start_hour; h < end_hour; h++) {
        if (h >= 0 && h < 24) {
            time_slots[day][h] = false;
        }
    }
    
    // 释放设施资源
    release_essentials(*booking);
}

// 重置调度结果以便重新调度
void reset_scheduling_results() {
    // 清空时间槽占用情况
    memset(time_slots, 0, sizeof(time_slots));
    
    // 重置所有设施占用状态
    for (int i = 0; i < 3; i++) {
        lockers[i].is_occupied = 0;
        umbrellas[i].is_occupied = 0;
        batteries[i].is_occupied = 0;
        cables[i].is_occupied = 0;
        valetParking[i].is_occupied = 0;
        inflationServices[i].is_occupied = 0;
    }
    
    // 重置所有停车位预约
    for (int i = 0; i < 10; i++) {
        parkingSlots[i].booking_count = 0;
    }
}

// 优先级比较函数
static int priority_comparator(const void* a, const void* b) {
    const Booking* bookingA = (const Booking*)a;
    const Booking* bookingB = (const Booking*)b;
    
    // 优先级降序排列
    if (bookingA->priority != bookingB->priority) {
        return bookingA->priority - bookingB->priority;
    }
    
    // 同优先级按添加顺序（数组索引）升序排列
    return (bookingA - bookingQueue) - (bookingB - bookingQueue);
}

// PRIO算法实现（优化版）
void process_requests_prio() {
    // 设置所有预约的优先级
    for (int i = 0; i < bookingCount; i++) {
        if (bookingQueue[i].status == 0) {
            bookingQueue[i].priority = get_booking_priority(bookingQueue[i].type);
        }
    }

    // 稳定排序：优先级降序，同优先级保持添加顺序
    qsort(bookingQueue, bookingCount, sizeof(Booking), priority_comparator);

    // 复用FCFS处理逻辑
    process_requests_fcfs();
}

// process_requests函数支持不同算法
void process_requests() {
    // 清空之前的调度结果
    reset_scheduling_results();
    
    // 根据当前选择的算法调用相应的处理函数
    switch (current_algorithm) {
        case ALG_FCFS:
            process_requests_fcfs();
            break;
        case ALG_PRIO:
            process_requests_prio();
            break;
        case ALG_OPTI:
            // 暂未实现的优化算法
           
            break;
        default:
            printf("Unknown algorithm selected\n");
            break;
    }
}
// 设置当前调度算法
void set_scheduling_algorithm(SchedulingAlgorithm algorithm) {
    current_algorithm = algorithm;
}