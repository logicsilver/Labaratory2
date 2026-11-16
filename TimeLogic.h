#ifndef TIMELOGIC_H
#define TIMELOGIC_H

void show_clock(int current_day, int current_hour);
void fast_forward_time(int* current_day, int* current_hour);
void add_hours_logic(int* current_day, int* current_hour, int hours);
void sleep_player(struct Player* player, int* current_day, int* current_hour);

#endif // TIMELOGIC_H#pragma once
