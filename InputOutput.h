#include "Utf8Util.h"
#include "Header.h"


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

void show_clock(int current_day, int current_hour) {
    printf("\n[Часы фермера]\n");
    printf("Текущее время: День %d, %02d:00\n", current_day, current_hour);
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

    inventory[slot] = item_empty;
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



void to_adventure(void) {
    system("cls");
    srand((unsigned int)time(NULL));

    printf("Введите размер карты (5 - 30): ");
    int SIZE = get_safe_int();
    if (SIZE < 5 || SIZE > 30) {
        return;
    }

    int floorCounter = 1;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Исправлен тип приведения в malloc на (char*)
    char** map = (char**)malloc(SIZE * sizeof(char*));
    for (int i = 0; i < SIZE; i++) {
        map[i] = (char*)malloc(SIZE * sizeof(char));
    }

    bool inDungeon = true;

    // Внешний цикл этажей вместо goto
    while (inDungeon) {
        // 1. Генерация карты
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (i == 0 || i == SIZE - 1 || j == 0 || j == SIZE - 1) {
                    map[i][j] = '#';
                }
                else {
                    map[i][j] = (rand() % 100 < 20) ? '!' : '0';
                }
            }
        }

        // 2. Спавн игрока и выхода D
        int playerX, playerY;
        do {
            playerX = 1 + rand() % (SIZE - 2);
            playerY = 1 + rand() % (SIZE - 2);
        } while (map[playerY][playerX] != '0');

        map[playerY][playerX] = 'D';
        char tileUnderPlayer = 'D';

        // 3. Спавн лестницы наверх U
        int ladderX, ladderY;
        do {
            ladderX = 1 + rand() % (SIZE - 2);
            ladderY = 1 + rand() % (SIZE - 2);
        } while (map[ladderY][ladderX] != '0');
        map[ladderY][ladderX] = 'U';

        // Ставим символ игрока поверх клетки D
        map[playerY][playerX] = 'Y';

        bool onCurrentFloor = true;

        // Цикл текущего этажа
        while (onCurrentFloor) {
            // Очистка старого ветра
            for (int i = 1; i < SIZE - 1; i++) {
                for (int j = 1; j < SIZE - 1; j++) {
                    if (map[i][j] == '^' || map[i][j] == 'v' || map[i][j] == '<' || map[i][j] == '>') {
                        map[i][j] = '0';
                    }
                }
            }
            if (tileUnderPlayer == '^' || tileUnderPlayer == 'v' || tileUnderPlayer == '<' || tileUnderPlayer == '>') {
                tileUnderPlayer = '0';
            }

            // Генерация нового ветра
            int windCount = ((SIZE - 2) * (SIZE - 2)) * 15 / 100;
            const char windDirs[] = { '^', 'v', '<', '>' };
            for (int w = 0; w < windCount; w++) {
                int wx = 1 + rand() % (SIZE - 2);
                int wy = 1 + rand() % (SIZE - 2);
                if (map[wy][wx] == '0') {
                    map[wy][wx] = windDirs[rand() % 4];
                }
            }

            // Отрисовка
            system("cls");
            printf("--- ЭТАЖ %d ---\n", floorCounter);

            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    char cell = map[i][j];
                    if (cell == 'Y') SetConsoleTextAttribute(hConsole, 14);
                    else if (cell == 'U') SetConsoleTextAttribute(hConsole, 13);
                    else if (cell == 'D') SetConsoleTextAttribute(hConsole, 9);
                    else if (cell == '^' || cell == 'v' || cell == '<' || cell == '>') SetConsoleTextAttribute(hConsole, 11);

                    printf("%c", cell);
                    SetConsoleTextAttribute(hConsole, 7);
                }
                printf("\n");
            }

            printf("Управление: [W,A,S,D] | [Q - Выйти домой]\n");

            int input = _getch();
            // Игнорируем спецклавиши (стрелки, F1-F12), чтобы не сбрасывать ход
            if (input == 0 || input == 224) {
                _getch();
                continue;
            }

            if (input == 'q' || input == 'Q') {
                if (tileUnderPlayer == 'D') {
                    inDungeon = false;
                    break;
                }
                else {
                    printf("\nВы можете выйти домой только на лестнице D!\n");
                    system("pause");
                    continue;
                }
            }

            int nextX = playerX;
            int nextY = playerY;

            if (input == 'w' || input == 'W') nextY--;
            else if (input == 's' || input == 'S') nextY++;
            else if (input == 'd' || input == 'D') nextX++;
            else if (input == 'a' || input == 'A') nextX--;
            else continue; // Если нажата левая кнопка, не обновляем карту

            // Проверка шага
            if (nextX >= 0 && nextX < SIZE && nextY >= 0 && nextY < SIZE) {
                if (map[nextY][nextX] != '#' && map[nextY][nextX] != '!') {
                    map[playerY][playerX] = tileUnderPlayer;
                    playerX = nextX;
                    playerY = nextY;
                    tileUnderPlayer = map[playerY][playerX];
                    map[playerY][playerX] = 'Y';

                    if (tileUnderPlayer == 'U') {
                        floorCounter++;
                        printf("\nПереход на следующий этаж...\n");
                        system("pause");
                        onCurrentFloor = false;
                        continue;
                    }

                    // Обработка ветра с защитой от бесконечного цикла
                    int windChainLimit = 0;
                    while ((tileUnderPlayer == '^' || tileUnderPlayer == 'v' || tileUnderPlayer == '<' || tileUnderPlayer == '>') && windChainLimit < 5) {
                        windChainLimit++;
                        int dx = 0, dy = 0;
                        if (tileUnderPlayer == '^') dy = -1;
                        else if (tileUnderPlayer == 'v') dy = 1;
                        else if (tileUnderPlayer == '<') dx = -1;
                        else if (tileUnderPlayer == '>') dx = 1;

                        char currentWindTile = tileUnderPlayer;
                        int steps = 0;

                        while (steps < 3) {
                            int checkX = playerX + dx;
                            int checkY = playerY + dy;

                            if (checkX < 0 || checkX >= SIZE || checkY < 0 || checkY >= SIZE) break;
                            if (map[checkY][checkX] == '#' || map[checkY][checkX] == '!') break;

                            map[playerY][playerX] = currentWindTile;
                            playerX = checkX;
                            playerY = checkY;

                            currentWindTile = map[playerY][playerX];
                            map[playerY][playerX] = 'Y';
                            steps++;

                            // Если пролетая попали на лестницу — сразу останавливаемся на ней
                            if (currentWindTile == 'U') break;
                        }

                        tileUnderPlayer = currentWindTile;

                        // Защита от зависания: если перед ветром сразу стена, выходим из цикла
                        if (steps == 0) break;

                        if (tileUnderPlayer == 'U') {
                            floorCounter++;
                            printf("\nВас сдуло прямиком на следующий этаж!...\n");
                            system("pause");
                            onCurrentFloor = false;
                            break;
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < SIZE; i++) {
        free(map[i]);
    }
    free(map);

    printf("Спасибо за игру!\n");
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
    printf("   [11] - Отправиться в путешествие\n");
    printf("\nВведите пункт -> ");
}

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


