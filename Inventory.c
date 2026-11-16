#include "Inventory.h"
#include "Utils.h"
#include "Utf8Util.h"
#include <stdio.h>
#include <string.h>

void init_default_items(char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    strcpy(item_names[ITEM_EMPTY], "Пусто");
    strcpy(item_names[ITEM_WOOD], "Дерево");
    strcpy(item_names[ITEM_STONE], "Камень");
    strcpy(item_names[ITEM_SEEDS], "Семена");
    strcpy(item_names[ITEM_IRON], "Железо");
    strcpy(item_names[ITEM_GOLD], "Золото");
    strcpy(item_names[ITEM_HOE], "Мотыга");
    strcpy(item_names[ITEM_FERTILIZER], "Удобрение");
    strcpy(item_names[ITEM_WATER_CAN], "Лейка");
    strcpy(item_names[ITEM_MILK], "Молоко");
}

bool load_items_from_file(const char* filename, char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    init_default_items(item_names);
    FILE* file = fopen(filename, "r");
    if (file == NULL) return false;

    skip_utf8_bom(file);
    int temp_id;
    char temp_name[MAX_NAME_LEN];
    char format[16];
    sprintf(format, "%%d %%%ds", MAX_NAME_LEN - 1);

    while (fscanf(file, format, &temp_id, temp_name) == 2) {
        if (temp_id >= 0 && temp_id < MAX_ITEMS) {
            strcpy(item_names[temp_id], temp_name);
        }
    }
    fclose(file);
    return true;
}


void print_inventory(int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    printf("\n[Содержимое инвентаря]\n");

    for (int i = 0; i < MAX_ITEMS; i++) {
        int item_id = inventory[i];
        if (item_id >= 0 && item_id < MAX_ITEMS) {
            printf("Слот %d: [%d] - %s.\n", i, item_id, item_names[item_id]);
        }
        else {
            printf("Слот %d: [%d] - Неизвестный предмет.\n", i, item_id);
        }
    }
}

void put_item(int inventory[]) {
    printf("\n[Положить предмет в слот]\n");

    printf("Номер слота (0..%d) -> ", MAX_ITEMS - 1);
    int slot = get_safe_int();

    if (slot < 0 || slot >= MAX_ITEMS) {
        printf("Ошибка! Неверный индекс слота.\n");
        return;
    }

    printf("ID предмета (0..%d) -> ", MAX_ITEMS - 1);
    int id = get_safe_int();

    if (id < 0 || id >= MAX_ITEMS) {
        printf("Ошибка! Неверный ID предмета.\n");
        return;
    }

    inventory[slot] = id;
    printf("Предмет помещен в слот %d.\n", slot);
}

void drop_item(int inventory[]) {
    printf("\n[Выбросить предмет из слота]\n");

    printf("Номер слота (0..%d) -> ", MAX_ITEMS - 1);
    int slot = get_safe_int();

    if (slot < 0 || slot >= MAX_ITEMS) {
        printf("Ошибка! Неверный индекс слота.\n");
        return;
    }

    inventory[slot] = ITEM_EMPTY;
    printf("Слот %d теперь пуст.\n", slot);
}
void remove_duplicates(int inventory[]) {
    printf("\n[Устранение дубликатов]\n");

    printf("Инвентарь до: ");
    for (int i = 0; i < MAX_ITEMS; i++) {
        printf("%d ", inventory[i]);
    }
    printf("\n");

    deduplicate_logic(inventory);

    printf("Инвентарь после: ");
    for (int i = 0; i < MAX_ITEMS; i++) {
        printf("%d ", inventory[i]);
    }
    printf("\n");
}
void find_item(int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    char search_name[MAX_NAME_LEN];

    printf("\n[Поиск предмета в рюкзаке]\n");
    printf("Введите название предмета -> ");
    read_utf8_word(search_name, MAX_NAME_LEN);

    int target_id = find_item_id_logic(item_names, search_name);

    if (target_id == -1) {
        printf("Такого предмета нет в каталоге!\n");
        return;
    }

    int found = 0;
    printf("Предмет найден в слотах: ");
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (inventory[i] == target_id) {
            printf("%d ", i);
            found = 1;
        }
    }

    if (!found) {
        printf("Предмет отсутствует в рюкзаке.");
    }
    printf("\n");
}

void deduplicate_logic(int inventory[]) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (inventory[i] == ITEM_EMPTY) {
            continue;
        }
        for (int j = i + 1; j < MAX_ITEMS; j++) {
            if (inventory[j] == inventory[i]) {
                inventory[j] = ITEM_EMPTY;
            }
        }
    }
}

int find_item_id_logic(char item_names[MAX_ITEMS][MAX_NAME_LEN], const char* search_name) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(item_names[i], search_name) == 0) {
            return i;
        }
    }
    return -1;
}
