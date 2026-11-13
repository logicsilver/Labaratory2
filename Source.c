#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <locale.h>

#include "Constants.h"
#include "Utf8Util.h"
#include "Utils.h"
#include "TimeLogic.h"
#include "Inventory.h"
#include "FileManager.h"
#include "Adventure.h"

void print_menu(const char* farmer_name) {
    printf("\n==== ФЕРМА : %s ====\n", farmer_name);
    printf("   [0] - Выход\n");
    printf("   [1] - Посмотреть на часы\n");
    printf("   [2] - Промотать время\n");
    printf("   [3] - Просмотреть инвентарь\n");
    printf("   [4] - Положить предмет в слот\n");
    printf("   [5] - Выбросить предмет\n");
    printf("   [6] - Устранение дубликатов\n");
    printf("   [7] - Поиск предмета в рюкзаке по названию\n");
    printf("   [8] - Записать состояние в дневник фермера\n");
    printf("   [9] - Расшифровать старые записи\n");
    printf("   [10] - Распаковать карту фермы (Вариант 10)\n");
    printf("   [11] - Отправиться в приключение\n");
    printf("\nВведите пункт -> ");
}

int main(void) {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, ".UTF8");

    char farmer_name[MAX_NAME_LEN];
    printf("Введите имя вашего фермера -> ");
    read_utf8_word(farmer_name, MAX_NAME_LEN);

    char item_names[MAX_ITEMS][MAX_NAME_LEN] = { 0 };

    // Загрузка делегирована модулю Inventory
    if (load_items_from_file("items.txt", item_names)) {
        printf("Предметы загружены из items.txt!\n");
    }
    else {
        printf("\n[Предупреждение]: Файл items.txt не найден, загружены стандартные названия.\n");
    }

    bool run = true;
    int current_day = 1;
    int current_hour = 8;

    // Начальный инвентарь
    int inventory[MAX_ITEMS] = {
        item_hoe, item_water_can, item_seeds, item_wood, item_seeds,
        item_stone, item_hoe, item_seeds, item_seeds, item_stone
    };

    while (run) {
        print_menu(farmer_name);
        int choice = get_safe_int();

        switch (choice) {
        case menu_task_1:  show_clock(current_day, current_hour); break;
        case menu_task_2:  fast_forward_time(&current_day, &current_hour); break;
        case menu_task_3:  print_inventory(inventory, item_names); break;
        case menu_task_4:  put_item(inventory); break;
        case menu_task_5:  drop_item(inventory); break;
        case menu_task_6:  remove_duplicates(inventory); break;
        case menu_task_7:  find_item(inventory, item_names); break;
        case menu_task_8:  write_diary(farmer_name, current_day, current_hour, inventory, item_names); break;
        case menu_task_9:  decode_notes(farmer_name, current_day, inventory, item_names); break;
        case menu_task_10: output_map(); break;
        case menu_task_11: to_adventure(); break;
        case menu_exit:
            printf("\n[Выполняется выход...]\n");
            run = false;
            break;
        default:
            printf("\nНеверный пункт меню, попробуйте еще раз!\n");
            break;
        }
    }
    return 0;
}