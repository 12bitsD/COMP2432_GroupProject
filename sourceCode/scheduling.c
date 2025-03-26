#include "booking.h"
#include <stdbool.h>

// 时间槽管理（7天，每天24小时）
static bool time_slots[7][24] = {0};
// 设备资源（3个battery，3个cable）
static bool battery_available[3] = {true};
static bool cable_available[3] = {true};

// 检查时间冲突
static bool check_time_conflict(int day, int start_hour, float duration) {
    int end_hour = start_hour + (int)duration;
    for (int h = start_hour; h < end_hour; h++) {
        if (time_slots[day][h]) return true;
    }
    return false;
}

// 分配设备资源
static bool allocate_essentials(const char essentials[][20], int count) {
    for (int i = 0; i < count; i++) {
        if (strcmp(essentials[i], "battery") == 0) {
            for (int j = 0; j < 3; j++) {
                if (battery_available[j]) {
                    battery_available[j] = false;
                    return true;
                }
            }
        }
        
    }
    return false;
}

// FCFS调度算法
void process_fcfs() {
    for (int i = 0; i < bookingCount; i++) {
        if (bookingQueue[i].status == 0) { // 仅处理待处理请求
            // 时间解析（简化处理，实际需要解析日期）
            int day = 0; // 示例日期转换
            int start_hour = atoi(bookingQueue[i].start_time);
            
            if (!check_time_conflict(day, start_hour, bookingQueue[i].duration) 
                && allocate_essentials(bookingQueue[i].essentials, MAX_ESSENTIALS)) {
                bookingQueue[i].status = 1;
                // 标记时间槽
                int end_hour = start_hour + (int)bookingQueue[i].duration;
                for (int h = start_hour; h < end_hour; h++) {
                    time_slots[day][h] = true;
                }
            } else {
                bookingQueue[i].status = -1;
            }
        }
    }
}