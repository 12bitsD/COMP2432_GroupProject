#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "booking.h"

// 全局预约请求队列（bookingQueue数），最多存储MAX_BOOKINGS个预约
// bookingQueue数组每个元素保存一个用户的停车预约信息，包含成员ID、时间、车位号等
// bookingCount记录当前有效的预约数量，添加新预约时会自动递增这个计数器
// 这两个变量会被不同的命令处理函数共享使用，比如添加/取消预约时都需要操作这个队列

// 定义全局
Booking bookingQueue[MAX_BOOKINGS];
int bookingCount = 0;


 // 添加请求到队列
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
    Booking newBooking = {0};
    strcpy(newBooking.type, "parking");
    newBooking.status = 0;
    
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
                }else{
                    printf("Error: Too many facilities specified\n");
                    return;
                }
                break;
        }
        param_index++;
    }
    // 检查是否包含必需设施：battery和cable
    bool has_battery = false;
    bool has_cable = false;
    
    // 遍历essentials数组检查必需设施
    for (int i = 0; i < MAX_ESSENTIALS && newBooking.essentials[i][0] != '\0'; i++) {
        if (strcmp(newBooking.essentials[i], "battery") == 0) {
            has_battery = true;
        } else if (strcmp(newBooking.essentials[i], "cable") == 0) {
            has_cable = true;
        }
    }
    
    // 验证必需设施是否都存在
    if (has_battery && has_cable) {
        add_queue(newBooking);
    } else {
        printf("Error: Reservation requires both battery and cable facilities\n");
    }
}

// 处理普通预约
void parse_reservation(char* main_cmd) {
    char* token = strtok(main_cmd, " ");
    Booking newBooking = {0};
    
    // 设置预约类型和状态
    strcpy(newBooking.type, "reservation");  // 普通预约
    newBooking.status = 0;  // 设置为待处理状态
    
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
                }else{
                    printf("Error: Too many facilities specified\n");
                    return;
                }
                break;
        }
        param_index++;
        
    }
    // 检查是否包含必需设施：battery和cable
    bool has_battery = false;
    bool has_cable = false;
    
    // 遍历essentials数组检查必需设施
    for (int i = 0; i < MAX_ESSENTIALS && newBooking.essentials[i][0] != '\0'; i++) {
        if (strcmp(newBooking.essentials[i], "battery") == 0) {
            has_battery = true;
        } else if (strcmp(newBooking.essentials[i], "cable") == 0) {
            has_cable = true;
        }
    }
    
    // 验证必需设施是否都存在
    if (has_battery && has_cable) {
        add_queue(newBooking);
    } else {
        printf("Error: Reservation requires both battery and cable facilities\n");
    }
}

// 处理事件预约（higher priority）
void parse_event(char* main_cmd) {
    char* token = strtok(main_cmd, " ");
    Booking newBooking = {0};
    
    // 设置预约类型和状态
    strcpy(newBooking.type, "event");  // 高优先级事件
    newBooking.status = 0;  // 设置为待处理状态
    
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
                if (param_index -4 < MAX_ESSENTIALS) {
            
                    strcpy(newBooking.essentials[param_index - 4], token);
                }else{
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
    
        // 主命令
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
        //指令格式模板:
        //strncmp是C语言中的字符串比较函数，用于比较两个字符串的前n个字符是否相等。
        else if (strncmp(main_cmd, "xxx", 11) == 0) {
            // 调用对应函数
            printf("随便打印点东西放在这里", main_cmd);
        }
        else if(strcmp(main_cmd,"endPraogram")){
            exit(0);
        }
        else {
            printf("Unknown command: %s\n", main_cmd);
        }
}