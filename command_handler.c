#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "booking.h"

// 定义全局变量
Booking bookingQueue[MAX_BOOKINGS];
int bookingCount = 0;

// 处理批量文件
void process_batch_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) > 0) {
            handle_command(line);
        }
    }
    fclose(file);
}

// 命令处理函数
void handle_command(const char* command) {
    char cmd_copy[256];
    strcpy(cmd_copy, command);
    
    // 分割主命令
    char* main_cmd = strtok(cmd_copy, ";");
    
    if (strncmp(main_cmd, "addParking", 10) == 0) {
        // 解析addParking命令
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
    else if (strncmp(main_cmd, "importBatch", 11) == 0) {
        // 解析批量导入命令
        char* file_name = strtok(main_cmd + 11, " ");
        if (file_name) {
            process_batch_file(file_name);
        } else {
            printf("Error: Missing batch file name\n");
        }
    }
    else {
        printf("Unknown command: %s\n", main_cmd);
    }
}