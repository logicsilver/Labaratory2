#ifndef PLAYER_H
#define PLAYER_H

#include "Constants.h"

struct Food {
    char name[32];
    int restore_hunger;
    int restore_stamina;
    char map_symbol;
    int item_id;
};

struct Player {
    char name[MAX_NAME_LEN];
    int hp;
    int max_hp;
    int stamina;
    int max_stamina;
    int hunger;
    int steps_taken;
    int gold;
    int x;
    int y;
    int inventory[MAX_ITEMS];
};

struct Player* create_player(struct Player* current_player);
void show_player_card(struct Player* player, int current_day, int current_hour, char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void free_player(struct Player* player);

#endif 
