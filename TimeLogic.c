#include "TimeLogic.h"
#include "Constants.h"
#include "Utils.h"
#include <stdio.h>

void show_clock(int current_day, int current_hour) {
    printf("\n[Часы фермера]\n");
    printf("Текущее время: День %d, %02d:00\n", current_day, current_hour);
}

void add_hours_logic(int* current_day, int* current_hour, int hours) {
    *current_hour += hours;
    if (*current_hour >= HOURS_PER_DAY) {
        *current_day += *current_hour / HOURS_PER_DAY;
        *current_hour = *current_hour % HOURS_PER_DAY;
    }
}

void fast_forward_time(int* current_day, int* current_hour) {
    printf("\n[Перемотка времени]\n");
    printf("Сколько часов вы хотите потратить на работу? -> ");
    int hours = get_safe_int();

    if (hours < 0) {
        printf("\nОшибка. Время не может идти назад!\n");
    }
    else {
        add_hours_logic(current_day, current_hour, hours);
        printf("\nВы успешно поработали.\n");
    }
}
