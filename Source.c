#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <string.h>
#include <locale.h>

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

// Константы размеров (без магических чисел)
#define MAX_ITEMS        10
#define MAX_FARMER_NAME  32
#define MAX_NAME_LEN     64
#define MAX_LINE_LEN     512
#define MAX_UNPACKED_LEN 1024
#define MAX_RESULT_LEN   2048
#define HOURS_PER_DAY    24

// ============================================================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ВВОДА И UTF-8
// ============================================================================

void skip_utf8_bom(FILE* f) {
    if (f == NULL) return;
    int c1 = fgetc(f);
    if (c1 == 0xEF) {
        int c2 = fgetc(f);
        int c3 = fgetc(f);
        if (c2 != 0xBB || c3 != 0xBF) {
            fseek(f, 0, SEEK_SET);
        }
    }
    else if (c1 != EOF) {
        ungetc(c1, f);
    }
}

int utf8_char_len(unsigned char c) {
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

void read_utf8_word(char* buffer, int max_bytes) {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    if (GetConsoleMode(hIn, &mode)) {
        wchar_t wbuf[MAX_LINE_LEN];
        DWORD read = 0;
        while (true) {
            if (!ReadConsoleW(hIn, wbuf, MAX_LINE_LEN - 1, &read, NULL) || read == 0) {
                buffer[0] = '\0';
                return;
            }
            wbuf[read] = L'\0';

            int start = 0;
            while (wbuf[start] == L' ' || wbuf[start] == L'\t' || wbuf[start] == L'\r' || wbuf[start] == L'\n') {
                start++;
            }
            if (wbuf[start] == L'\0') {
                continue;
            }

            int end = start;
            while (wbuf[end] != L'\0' && wbuf[end] != L' ' && wbuf[end] != L'\t' && wbuf[end] != L'\r' && wbuf[end] != L'\n') {
                end++;
            }
            wbuf[end] = L'\0';

            WideCharToMultiByte(CP_UTF8, 0, &wbuf[start], -1, buffer, max_bytes, NULL, NULL);
            buffer[max_bytes - 1] = '\0';
            return;
        }
    }
    else {
        char format[16];
        sprintf(format, "%%%ds", max_bytes - 1);
        if (scanf(format, buffer) != 1) {
            buffer[0] = '\0';
        }
    }
}

int get_safe_int(void) {
    int value;
    while (scanf("%d", &value) != 1) {
        printf("Ошибка. Введено не число. Повторите ввод -> ");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    return value;
}

// ============================================================================
// ФУНКЦИИ ЛОГИКИ И ОБРАБОТКИ ДАННЫХ (БЕЗ PRINTF / SCANF)
// ============================================================================

void init_default_items(char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
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

bool load_items_from_file(const char* filename, char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    init_default_items(item_names);

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return false;
    }

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

void add_hours_logic(int* current_day, int* current_hour, int hours) {
    *current_hour += hours;
    if (*current_hour >= HOURS_PER_DAY) {
        *current_day += *current_hour / HOURS_PER_DAY;
        *current_hour = *current_hour % HOURS_PER_DAY;
    }
}

void deduplicate_logic(int inventory[]) {
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
}

int find_item_id_logic(char item_names[MAX_ITEMS][MAX_NAME_LEN], const char* search_name) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(item_names[i], search_name) == 0) {
            return i;
        }
    }
    return -1;
}

void replace_tags_in_line(const char* line, char* result, int max_result_size,
    const char* farmer_name, const char* day_str, const char* first_item) {
    result[0] = '\0';
    int len = (int)strlen(line);

    for (int i = 0; i < len; ) {
        if (strncmp(&line[i], "<NAME>", 6) == 0) {
            strncat(result, farmer_name, max_result_size - strlen(result) - 1);
            i += 6;
        }
        else if (strncmp(&line[i], "<DAY>", 5) == 0) {
            strncat(result, day_str, max_result_size - strlen(result) - 1);
            i += 5;
        }
        else if (strncmp(&line[i], "<ITEM>", 6) == 0) {
            strncat(result, first_item, max_result_size - strlen(result) - 1);
            i += 6;
        }
        else {
            int cur_len = (int)strlen(result);
            if (cur_len < max_result_size - 1) {
                result[cur_len] = line[i];
                result[cur_len + 1] = '\0';
            }
            i++;
        }
    }
}

int unpack_rle_line_logic(const char* line, char* unpacked, int max_unpacked_size) {
    int out_pos = 0;
    int char_count = 0;
    int i = 0;
    int len = (int)strlen(line);

    while (i < len) {
        int count = 0;
        while (i < len && line[i] >= '0' && line[i] <= '9') {
            count = count * 10 + (line[i] - '0');
            i++;
        }
        if (count == 0) {
            count = 1;
        }

        if (i < len) {
            int clen = utf8_char_len((unsigned char)line[i]);
            if (i + clen > len) {
                clen = len - i;
            }

            for (int k = 0; k < count; k++) {
                if (out_pos + clen < max_unpacked_size - 1) {
                    memcpy(&unpacked[out_pos], &line[i], clen);
                    out_pos += clen;
                    char_count++;
                }
            }
            i += clen;
        }
    }
    unpacked[out_pos] = '\0';
    return char_count;
}

// ============================================================================
// ФУНКЦИИ ПУНКТОВ МЕНЮ (ВВОД / ВЫВОД)
// ============================================================================

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
        add_hours_logic(current_day, current_hour, hours);
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

// 5. Выбросить предмет
void drop_item(int inventory[]) {
    printf("\n[Выбросить предмет из слота]\n");

    printf("Номер слота (0..%d) -> ", MAX_ITEMS - 1);
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

    deduplicate_logic(inventory);

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

// 8. Запись состояния в дневник фермера
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

void decode_notes(const char* farmer_name, int current_day, const int* inventory, char item_names[][MAX_NAME_LEN]) {
    printf("\n[Расшифровка старых записей]\n");

    FILE* in = fopen("diary.txt", "r");
    if (in == NULL) {
        printf("Ошибка: не удалось открыть diary.txt\n");
        return;
    }

    skip_utf8_bom(in);

    char line[MAX_LINE_LEN];
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
        char result[MAX_RESULT_LEN];
        replace_tags_in_line(line, result, sizeof(result), farmer_name, day_str, first_item);

        printf("%s", result);
    }

    fclose(in);
}


void output_map(void) {
    printf("\n[Распаковка карты (Вариант 10)]\n");

    FILE* in = fopen("input.txt", "r");
    if (in == NULL) {
        printf("Ошибка: не удалось открыть input.txt!\n");
        return;
    }
    skip_utf8_bom(in);

    FILE* out = fopen("output.txt", "w");
    if (out == NULL) {
        printf("Ошибка: не удалось создать output.txt!\n");
        fclose(in);
        return;
    }

    char line[MAX_LINE_LEN];
    int line_number = 0;
    int expected_length = -1;
    bool geometry_ok = true;

    printf("\n--- Карта фермы ---\n");

    while (fgets(line, sizeof(line), in) != NULL) {
        line_number++;
        line[strcspn(line, "\r\n")] = '\0';

        if (strlen(line) == 0) {
            continue;
        }

        char unpacked[MAX_UNPACKED_LEN];
        int char_count = unpack_rle_line_logic(line, unpacked, sizeof(unpacked));

        // Проверка геометрии по количеству реальных символов (а не байт)
        if (expected_length == -1) {
            expected_length = char_count;
        }
        else if (char_count != expected_length) {
            geometry_ok = false;
            printf("[Ошибка геометрии]: Строка %d имеет длину %d (ожидалось %d)!\n",
                line_number, char_count, expected_length);
            fprintf(out, "[Ошибка геометрии]: Строка %d имеет длину %d (ожидалось %d)!\n",
                line_number, char_count, expected_length);
            continue;
        }

        printf("%s\n", unpacked);
        fprintf(out, "%s\n", unpacked);
    }

    fclose(in);
    fclose(out);

    if (geometry_ok && line_number > 0) {
        printf("\nГеометрия карты верна! Сохранено в output.txt\n");
    }
    else if (!geometry_ok) {
        printf("\nОбнаружены ошибки геометрии! Подробности записаны в output.txt\n");
    }
}

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
    printf("\nВведите пункт -> ");
}

// ============================================================================
// MAIN
// ============================================================================

int main(void) {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, ".UTF8");

    char farmer_name[MAX_FARMER_NAME];
    printf("Введите имя вашего фермера -> ");
    read_utf8_word(farmer_name, MAX_FARMER_NAME);

    char item_names[MAX_ITEMS][MAX_NAME_LEN] = { 0 };
    if (load_items_from_file("items.txt", item_names)) {
        printf("Предметы загружены из items.txt!\n");
    }
    else {
        printf("\n[Предупреждение]: Файл items.txt не найден, загружены стандартные названия.\n");
    }

    bool run = true;
    int current_day = 1;
    int current_hour = 8;

    int inventory[MAX_ITEMS] = {
        item_hoe, item_water_can, item_seeds, item_wood, item_seeds,
        item_stone, item_hoe, item_seeds, item_seeds, item_stone
    };

    while (run) {
        print_menu(farmer_name);
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