#include "Player.h"
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
    printf("\nСколько часов ждать? -> ");
    int hours = get_safe_int();

    if (hours < 0) {
        printf("\nОшибка. Время не может идти назад!\n");
        return;
    }

    int end_hour = *current_hour + hours;
    if (end_hour > 26 || (end_hour >= 2 && *current_hour < 2 && end_hour < 24)) {
        printf("\nФермер не может не спать всю ночь! Максимум можно ждать до 02:00.\n");
        return;
    }

    add_hours_logic(current_day, current_hour, hours);
    printf("\nВы прождали %d часов.\n", hours);
}

void sleep_player(struct Player* player, int* current_day, int* current_hour) {
    if (player == NULL) {
        printf("\n[Ошибка] Некому спать, создайте персонажа!\n");
        return;
    }
    printf("\nФермер %s ложится спать...\n", player->name);

    if (*current_hour >= 8 && *current_hour <= 23) {
        *current_day += 1; 
    }
    *current_hour = 8;

    player->hp = player->max_hp;
    player->stamina = player->max_stamina;

    printf("Zzz... Наступило утро (День %d, 08:00). Силы полностью восстановлены!\n", *current_day);
}