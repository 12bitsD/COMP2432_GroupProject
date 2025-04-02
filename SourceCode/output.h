#ifndef OUTPUT_H
#define OUTPUT_H

void print_booking_schedule_fcfs();
void print_booking_schedule_prio();
void print_booking_schedule_opti();
void print_summary_report();
void print_all();

// 额外工具函数声明
extern float calculate_facility_utilization(Facility *facilities, int num_facilities);
extern float calculate_parking_utilization();
extern void compute_end_time(const char *start_time, float duration, char *end_time, int size);

#endif // OUTPUT_H