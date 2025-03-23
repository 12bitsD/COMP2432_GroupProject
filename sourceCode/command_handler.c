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


// 命令处理函数
// 解析addParking命令
void parse_add_parking(char* main_cmd) {
    char* token = strtok(main_cmd, " ");
    Booking newBooking = {0};
    
    // 提取参数
    while ((token = strtok(NULL, " ")) != NULL) {
        if (strncmp(token, "-member_", 8) == 0) {
            strncpy(newBooking.member, token + 1, sizeof(newBooking.member)-1);
        }
        else if (strchr(token, '-') && strlen(token) == 10) { // 日期格式判断
            strcpy(newBooking.date, token);
        }
        else if (strchr(token, ':') && strlen(token) == 5) { // 时间格式判断
            strcpy(newBooking.start_time, token);
        }
        else if (isdigit(token[0]) && strchr(token, '.')) { // 时长判断
            newBooking.duration = atof(token);
        }
        else if (strncmp(token, "slot", 4) == 0) { // 停车位编号
            newBooking.slot = atoi(token + 4);
        }
        else { // 设施列表
            for (int i = 0; i < MAX_ESSENTIALS; i++) {
                if (strlen(newBooking.essentials[i]) == 0) {
                    strcpy(newBooking.essentials[i], token);
                    break;
                }
            }
        }
    }
    // 添加请求到队列
    if (bookingCount < MAX_BOOKINGS) {
        bookingQueue[bookingCount++] = newBooking;
        printf("Added parking request for %s\n", newBooking.member);
    }
}

void handle_command(const char* command) {
    char cmd_copy[256];
    strcpy(cmd_copy, command);
    
        // 主命令
        char* main_cmd = strtok(cmd_copy, ";");
        
        if (strncmp(main_cmd, "addParking", 10) == 0) {
            parse_add_parking(main_cmd);
        }
        //指令格式模板:
        //strncmp是C语言中的字符串比较函数，用于比较两个字符串的前n个字符是否相等。
        else if (strncmp(main_cmd, "xxx", 11) == 0) {
            // 调用对应函数
            printf("随便打印点东西放在这里", main_cmd);
        }
        else {
            printf("Unknown command: %s\n", main_cmd);
        }
}