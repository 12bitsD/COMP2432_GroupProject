#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "booking.h"

typedef enum {
    ALGO_FCFS = 0,
    ALGO_PRIO = 1,
    ALGO_OPTI = 2
} SchedulingAlgorithm;

void process_requests(int algo);
void process_requests_fcfs();
void process_requests_prio();
void process_requests_opti();
void reset_scheduling_results();

#endif // SCHEDULING_H