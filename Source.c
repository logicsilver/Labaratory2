#include "Modules.h"


int main(void) {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, ".UTF8");

    char farmer_name[MAX_NAME_LEN];
    printf("Введите имя вашего фермера -> ");
    read_utf8_word(farmer_name, MAX_NAME_LEN);

    char item_names[MAX_ITEMS][MAX_NAME_LEN] = { 0 };

    FILE* file = fopen("items.txt", "r");
    if (file != NULL) {
        skip_utf8_bom(file);
        int temp_id;
        char temp_name[MAX_NAME_LEN];

        while (fscanf(file, "%d %63s", &temp_id, temp_name) == 2) {
            if (temp_id >= 0 && temp_id < MAX_ITEMS) {
                strcpy(item_names[temp_id], temp_name);
            }
        }
        fclose(file);
        printf("Предметы загружены из items.txt!\n");
    }
    else {
        printf("\n[Предупреждение]: Файл items.txt не найден, загружены стандартные названия.\n");
        strcpy(item_names[item_empty], "Пусто");
        strcpy(item_names[item_wood], "Дерево");
        strcpy(item_names[item_stone], "Камень");
        strcpy(item_names[item_seeds], "Семена");
        strcpy(item_names[item_iron], "Железо");
        strcpy(item_names[item_gold], "Золото");
        strcpy(item_names[item_hoe], "Мотыга");
        strcpy(item_names[item_fertilizer], "Удобрение");
        strcpy(item_names[item_water_can], "Лейка");
        strcpy(item_names[item_milk], "Молоко");
    }

    bool run = true;
    int current_day = 1;
    int current_hour = 8;

    int inventory[MAX_ITEMS] = { 0 };
    inventory[0] = item_hoe;
    inventory[1] = item_water_can;
    inventory[2] = item_seeds;
    inventory[3] = item_wood;
    inventory[4] = item_seeds;
    inventory[5] = item_stone;
    inventory[6] = item_hoe;
    inventory[7] = item_seeds;
    inventory[8] = item_seeds;
    inventory[9] = item_stone;

    while (run) {
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
        printf("   [11] - Отправиться в приключение (Бродилка)\n");
        printf("\nВведите пункт -> ");

        int choice = get_safe_int();

        switch (choice) {
        case menu_task_1:
            show_clock(current_day, current_hour);
            break;
        case menu_task_2:
            fast_forward_time(&current_day, &current_hour);
            break;
        case menu_task_3:
            print_inventory(inventory, item_names);
            break;
        case menu_task_4:
            put_item(inventory);
            break;
        case menu_task_5:
            drop_item(inventory);
            break;
        case menu_task_6:
            remove_duplicates(inventory);
            break;
        case menu_task_7:
            find_item(inventory, item_names);
            break;
        case menu_task_8:
            write_diary(farmer_name, current_day, current_hour, inventory, item_names);
            break;
        case menu_task_9:
            decode_notes(farmer_name, current_day, inventory, item_names);
            break;
        case menu_task_10:
            output_map();
            break;
        case menu_task_11:
            to_adventure();
            break;
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