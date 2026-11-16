#include "Player.h"
#include "Utils.h"
#include "Utf8Util.h"
#include <stdio.h>
#include <stdlib.h>

struct Player* create_player(struct Player* current_player) {
    if (current_player != NULL) {
        printf("Персонаж уже существует Хотите удалить его и создать нового? (1 - Да, 0 - Нет): ");
        int choice = get_safe_int();
        if (choice != 1) return current_player;

        printf("Память структуры по адресу %p освобождена\n", (void*)current_player);
        free(current_player);
    }

    struct Player* new_player = (struct Player*)malloc(sizeof(struct Player));

    printf("Введите имя нового фермера -> ");
    read_utf8_word(new_player->name, MAX_NAME_LEN);

    new_player->max_hp = 100;
    new_player->hp = 100;
    new_player->max_stamina = 50;
    new_player->stamina = 50;
    new_player->hunger = 100;
    new_player->steps_taken = 0;
    new_player->gold = 0;
    new_player->x = 0;
    new_player->y = 0;

    for (int i = 0; i < MAX_ITEMS; i++) {
        new_player->inventory[i] = ITEM_EMPTY;
    }
    new_player->inventory[0] = ITEM_HOE;
    new_player->inventory[1] = ITEM_WATER_CAN;

    printf("\nПерсонаж %s успешно создан!\n", new_player->name);
    return new_player;
}

void show_player_card(struct Player* player, int current_day, int current_hour, char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    if (player == NULL) {
        printf("\n[Ошибка] Персонаж еще не создан!\n");
        return;
    }
    printf("\n=== КАРТОЧКА ПЕРСОНАЖА ===\n");
    printf("Имя: %s\n", player->name);
    printf("Здоровье: %d / %d\n", player->hp, player->max_hp);
    printf("Выносливость: %d / %d\n", player->stamina, player->max_stamina);
    printf("Сытость: %d / 100\n", player->hunger);
    printf("Золото: %d\n", player->gold);
    printf("Текущее время: День %d, %02d:00\n", current_day, current_hour);

    printf("\nИнвентарь:\n");
    for (int i = 0; i < MAX_ITEMS; i++) {
        int id = player->inventory[i];
        if (id != ITEM_EMPTY) {
            printf(" [%d] %s\n", i, item_names[id]);
        }
    }
    printf("==========================\n");
}

void free_player(struct Player* player) {
    if (player != NULL) {
        printf("Память структуры по адресу %p освобождена\n", (void*)player);
        free(player);
    }
}