#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <string.h>

// Пункты меню
#define menu_exit        0
#define menu_task_1      1
#define menu_task_2      2
#define menu_task_3      3
#define menu_task_4      4
#define menu_task_5      5
#define menu_task_6      6
#define menu_task_7      7
#define menu_task_8      8
#define menu_task_9      9
#define menu_task_10     10

// ID предметов
#define item_empty       0
#define item_wood        1
#define item_stone       2
#define item_seeds       3
#define item_iron        4
#define item_gold        5
#define item_hoe         6
#define item_fertilizer  7
#define item_water_can   8
#define item_milk        9

#define MAX_ITEMS        10
#define MAX_NAME_LEN     32

// Функция для безопасного ввода целого числа
int get_safe_int() {
    int value;
    while (scanf("%d", &value) != 1) {
        printf("Ошибка. Введено не число. Повторите ввод -> ");
        while (getchar() != '\n'); // чистим буфер
    }
    return value;
}

// 1. Показать часы
void show_clock(int current_day, int current_hour) {
    printf("\n[Часы фермера]\n");
    printf("Текущее время: День %d, %02d:00\n", current_day, current_hour);
}

// 2. Перемотка времени
void fast_forward_time(int* current_day, int* current_hour) {
    printf("\n[Перемотка времени]\n");
    printf("Сколько часов вы хотите потратить на работу? -> ");

    int hours = get_safe_int();

    if (hours < 0) {
        printf("\nОшибка. Время не может идти назад!\n");
    }
    else {
        *current_hour += hours;
        if (*current_hour >= 24) {
            *current_day += *current_hour / 24;
            *current_hour = *current_hour % 24;
        }
        printf("\nВы успешно поработали.\n");
    }
}

// 3. Вывод инвентаря
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

// 4. Положить предмет в слот
void put_item(int inventory[]) {
    printf("\n[Положить предмет в слот]\n");

    printf("Номер слота (0..9) -> ");
    int slot = get_safe_int();

    if (slot < 0 || slot >= MAX_ITEMS) {
        printf("Ошибка! Неверный индекс слота.\n");
        return;
    }

    printf("ID предмета (0..9) -> ");
    int id = get_safe_int();

    if (id < 0 || id >= MAX_ITEMS) {
        printf("Ошибка! Неверный ID предмета.\n");
        return;
    }

    inventory[slot] = id;
    printf("Предмет помещен в слот %d.\n", slot);
}

// 5. Выбросить предмет
void drop_item(int inventory[]) {
    printf("\n[Выбросить предмет из слота]\n");

    printf("Номер слота (0..9) -> ");
    int slot = get_safe_int();

    if (slot < 0 || slot >= MAX_ITEMS) {
        printf("Ошибка! Неверный индекс слота.\n");
        return;
    }

    inventory[slot] = item_empty;
    printf("Слот %d теперь пуст.\n", slot);
}

// 6. Устранение дубликатов
void remove_duplicates(int inventory[]) {
    printf("\n[Устранение дубликатов]\n");

    printf("Инвентарь до: ");
    for (int i = 0; i < MAX_ITEMS; i++) {
        printf("%d ", inventory[i]);
    }
    printf("\n");

    for (int i = 0; i < MAX_ITEMS; i++) {
        if (inventory[i] == item_empty) {
            continue;
        }

        for (int j = i + 1; j < MAX_ITEMS; j++) {
            if (inventory[j] == inventory[i]) {
                inventory[j] = item_empty;
            }
        }
    }

    printf("Инвентарь после: ");
    for (int i = 0; i < MAX_ITEMS; i++) {
        printf("%d ", inventory[i]);
    }
    printf("\n");
}

// 7. Поиск предмета по названию
void find_item(int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    char search_name[MAX_NAME_LEN];

    printf("\n[Поиск предмета в рюкзаке]\n");
    printf("Введите название предмета -> ");
    scanf("%s", search_name);

    int target_id = -1;
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(item_names[i], search_name) == 0) {
            target_id = i;
            break;
        }
    }

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

// 8. Запись в дневник
void write_diary(char farmer_name[], int current_day, int current_hour, int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    printf("\n[Запись в дневник фермера]\n");

    FILE* f = fopen("diary.txt", "a");
    if (f == NULL) {
        printf("Ошибка открытия файла diary.txt!\n");
        return;
    }

    fprintf(f, "Дневник фермера: %s\n", farmer_name);
    fprintf(f, "Время: День %d, %02d:00\n", current_day, current_hour);
    fprintf(f, "Инвентарь:\n");
    for (int i = 0; i < MAX_ITEMS; i++) {
        int id = inventory[i];
        if (id >= 0 && id < MAX_ITEMS) {
            fprintf(f, "Слот %d: [%d] - %s\n", i, id, item_names[id]);
        }
    }
    fprintf(f, "-----------------------------------\n");

    fclose(f);
    printf("Запись успешно добавлена в diary.txt!\n");
}

// 9. Расшифровка записей
void decode_notes(char farmer_name[], int current_day, int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    printf("\n[Расшифровка старых записей]\n");

    FILE* in = fopen("input.txt", "r");
    if (in == NULL) {
        printf("Ошибка: не удалось открыть input.txt!\n");
        return;
    }

    FILE* out = fopen("output.txt", "w");
    if (out == NULL) {
        printf("Ошибка: не удалось создать output.txt!\n");
        fclose(in);
        return;
    }

    char line[256];
    char day_str[16];
    sprintf(day_str, "%d", current_day);

    char first_item[MAX_NAME_LEN];
    if (inventory[0] >= 0 && inventory[0] < MAX_ITEMS) {
        strcpy(first_item, item_names[inventory[0]]);
    }
    else {
        strcpy(first_item, "Пусто");
    }

    while (fgets(line, sizeof(line), in) != NULL) {
        char result[1024] = "";
        int len = strlen(line);

        for (int i = 0; i < len; ) {
            if (strncmp(&line[i], "<NAME>", 6) == 0) {
                strcat(result, farmer_name);
                i += 6;
            }
            else if (strncmp(&line[i], "<DAY>", 5) == 0) {
                strcat(result, day_str);
                i += 5;
            }
            else if (strncmp(&line[i], "<ITEM>", 6) == 0) {
                strcat(result, first_item);
                i += 6;
            }
            else {
                int cur_len = strlen(result);
                result[cur_len] = line[i];
                result[cur_len + 1] = '\0';
                i++;
            }
        }

        printf("%s", result);
        fprintf(out, "%s", result);
    }

    fclose(in);
    fclose(out);
    printf("\nРезультат сохранен в output.txt!\n");
}

// 10. Вывод карты из файла map.txt
void output_map() {
    printf("\n[Карта фермы]\n");

    FILE* map = fopen("input.txt", "r");
    if (map == NULL) {
        printf("Ошибка: не удалось открыть map.txt!\n");
        return;
    }

    int coeficent;
    char ch;
    while (fscanf(map, "%d", &coeficent) == 1) {
        ch = fgetc(map);
        if (ch != EOF && (ch < '0' || ch > '9')) {
            for (int i = 0; i < coeficent; i++) {
                putchar(ch);
            }
        }
    }
    printf("\n");

    fclose(map);
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    char farmer_name[MAX_NAME_LEN];
    printf("Введите имя вашего фермера -> ");
    scanf("%s", farmer_name);

    char item_names[MAX_ITEMS][MAX_NAME_LEN];

    FILE* file = fopen("items.txt", "r");
    if (file != NULL) {
        int temp_id;
        char temp_name[MAX_NAME_LEN];

        while (fscanf(file, "%d %s", &temp_id, temp_name) == 2) {
            if (temp_id >= 0 && temp_id < MAX_ITEMS) {
                strcpy(item_names[temp_id], temp_name);
            }
        }
        fclose(file);
        printf("Предметы загружены из items.txt!\n");
    }
    else {
        printf("\n[Предупреждение]: Файл items.txt не найден\n");
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

    // Начальный инвентарь
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
        printf("   [10] - Вывести карту\n");
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