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
#include "Player.h"

void print_menu(struct Player* player) {
    if (player != NULL) {
        printf("\n==== ФЕРМА : %s ====\n", player->name);
    }
    else {
        printf("\n==== ФЕРМА ====\n");
    }
    printf("   [0] - Выход\n");
    printf("   [1] - Создать / пересоздать персонажа\n");
    printf("   [2] - Карточка персонажа\n");
    printf("   [3] - Посмотреть на часы\n");
    printf("   [4] - Пропустить время (Ждать)\n");
    printf("   [5] - Отдых (Лечь спать)\n");
    printf("   [6] - Просмотреть инвентарь\n");
    printf("   [7] - Положить предмет в слот\n");
    printf("   [8] - Выбросить предмет\n");
    printf("   [9] - Устранение дубликатов\n");
    printf("   [10] - Поиск предмета в рюкзаке по названию\n");
    printf("   [11] - Записать состояние в дневник фермера\n");
    printf("   [12] - Расшифровать старые записи\n");
    printf("   [13] - Распаковать карту фермы (Вариант 10)\n");
    printf("   [14] - Отправиться в приключение\n");
    printf("\nВведите пункт -> ");
}

int main(void) {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, ".UTF8");

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

    // Изначально персонажа нет казатель равен NULL.
    struct Player* player = NULL;

    while (run) {
        print_menu(player);
        int choice = get_safe_int();

        switch (choice) {
        case 0: // Выход
            printf("\n[Выполняется выход...]\n");
            free_player(player); //  очистка памяти 
            run = false;
            break;
        case 1: // Создать / пересоздать персонажа
            player = create_player(player);
            break;
        case 2: // Карточка персонажа
            show_player_card(player, current_day, current_hour, item_names);
            break;
        case 3: // Часы
            show_clock(current_day, current_hour);
            break;
        case 4: // Ждать
            fast_forward_time(&current_day, &current_hour);
            break;
        case 5: // Спать
            sleep_player(player, &current_day, &current_hour);
            break;


        case 6: // Инвентарь
            if (player == NULL) printf("\n[Ошибка] Персонаж не создан!\n");
            else print_inventory(player->inventory, item_names);
            break;
        case 7: // Положить предмет
            if (player == NULL) printf("\n[Ошибка] Персонаж не создан!\n");
            else put_item(player->inventory);
            break;
        case 8: // Выбросить предмет
            if (player == NULL) printf("\n[Ошибка] Персонаж не создан!\n");
            else drop_item(player->inventory);
            break;
        case 9: // Устранение дубликатов
            if (player == NULL) printf("\n[Ошибка] Персонаж не создан!\n");
            else remove_duplicates(player->inventory);
            break;
        case 10: // Поиск
            if (player == NULL) printf("\n[Ошибка] Персонаж не создан!\n");
            else find_item(player->inventory, item_names);
            break;
        case 11: // Дневник
            if (player == NULL) printf("\n[Ошибка] Персонаж не создан!\n");
            else write_diary(player->name, current_day, current_hour, player->inventory, item_names);
            break;
        case 12: // Расшифровка
            if (player == NULL) printf("\n[Ошибка] Персонаж не создан!\n");
            else decode_notes(player->name, current_day, player->inventory, item_names);
            break;
        case 13: // Распаковать карту
            output_map();
            break;
        case 14: // Приключение
            to_adventure(player, &current_day, &current_hour);
            break;
        default:
            printf("\nНеверный пункт меню, попробуйте еще раз!\n");
            break;
        }
    }
    return 0;
}