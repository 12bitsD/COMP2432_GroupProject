#ifndef INPUTMODULE_H
#define INPUTMODULE_H

#include "booking.h"
#include "output.h"

void handle_command(const char* command);
void add_queue(Booking newBooking);
void parse_parking(char* main_cmd);
void parse_reservation(char* main_cmd);
void parse_event(char* main_cmd);
void parse_essentials(char* main_cmd);
void import_batch(char* filename);

#endif // INPUTMODULE_H