#include "Adventure.h"
#include "Constants.h"
#include "Player.h"
#include "TimeLogic.h" 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <windows.h>
#include <conio.h>

struct Map {
    char** grid;
    int size;
    int floor_counter;
};

char** allocate_map(int size) {
    char** map = (char**)malloc(size * sizeof(char*));
    for (int i = 0; i < size; i++) {
        map[i] = (char*)malloc(size * sizeof(char));
    }
    return map;
}

void free_map(char** map, int size) {
    for (int i = 0; i < size; i++) {
        free(map[i]);
    }
    free(map);
}

void generate_floor(char** map, int size, int* pX, int* pY) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == 0 || i == size - 1 || j == 0 || j == size - 1) map[i][j] = '#';
            else map[i][j] = (rand() % 100 < 20) ? '!' : '0';
        }
    }

    do {
        *pX = 1 + rand() % (size - 2);
        *pY = 1 + rand() % (size - 2);
    } while (map[*pY][*pX] != '0');
    map[*pY][*pX] = 'D';

    int ladderX, ladderY;
    do {
        ladderX = 1 + rand() % (size - 2);
        ladderY = 1 + rand() % (size - 2);
    } while (map[ladderY][ladderX] != '0');
    map[ladderY][ladderX] = 'U';

    int windCount = ((size - 2) * (size - 2)) * 15 / 100;
    const char windDirs[] = { '^', 'v', '<', '>' };
    for (int w = 0; w < windCount; w++) {
        int wx = 1 + rand() % (size - 2);
        int wy = 1 + rand() % (size - 2);
        if (map[wy][wx] == '0') map[wy][wx] = windDirs[rand() % 4];
    }

    // Спавн Яблок (2-3 штуки)
    int apples = 2 + rand() % 2;
    for (int i = 0; i < apples; i++) {
        int fx, fy;
        do { fx = 1 + rand() % (size - 2); fy = 1 + rand() % (size - 2); } while (map[fy][fx] != '0');
        map[fy][fx] = 'A';
    }

    // Спавн Морковок (2-3 штуки)
    int carrots = 2 + rand() % 2;
    for (int i = 0; i < carrots; i++) {
        int fx, fy;
        do { fx = 1 + rand() % (size - 2); fy = 1 + rand() % (size - 2); } while (map[fy][fx] != '0');
        map[fy][fx] = 'C';
    }
}

void draw_map(char** map, int size, int floorCounter, HANDLE hConsole) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            char cell = map[i][j];
            if (cell == 'Y') SetConsoleTextAttribute(hConsole, 14);
            else if (cell == 'U') SetConsoleTextAttribute(hConsole, 13);
            else if (cell == 'D') SetConsoleTextAttribute(hConsole, 9);
            else if (cell == '^' || cell == 'v' || cell == '<' || cell == '>') SetConsoleTextAttribute(hConsole, 11);
            else if (cell == 'A') SetConsoleTextAttribute(hConsole, 12); // Красное яблоко
            else if (cell == 'C') SetConsoleTextAttribute(hConsole, 6);  // Оранжевая морковка

            printf("%c", cell);
            SetConsoleTextAttribute(hConsole, 7);
        }
        printf("\n");
    }
    printf("Управление: [W,A,S,D] | [E - Поесть] | [Q - Выйти домой]\n");
}

// === ЛОГИКА ВЕТРА И УРОНА ОБ СТЕНУ ===
bool apply_wind(char** map, int size, struct Player* player, char* tileUnder) {
    int windChainLimit = 0;
    while ((*tileUnder == '^' || *tileUnder == 'v' || *tileUnder == '<' || *tileUnder == '>') && windChainLimit < 5) {
        windChainLimit++;
        int dx = 0, dy = 0;
        if (*tileUnder == '^') dy = -1;
        else if (*tileUnder == 'v') dy = 1;
        else if (*tileUnder == '<') dx = -1;
        else if (*tileUnder == '>') dx = 1;

        char currentWindTile = *tileUnder;
        int steps = 0;

        while (steps < 3) {
            int checkX = player->x + dx;
            int checkY = player->y + dy;

            if (checkX < 0 || checkX >= size || checkY < 0 || checkY >= size ||
                map[checkY][checkX] == '#' || map[checkY][checkX] == '!') {

                int damage = 0;
                if (steps == 0) damage = 10;      // Сразу препятствие
                else if (steps == 1) damage = 5;  // Через 1 клетку
                else if (steps == 2) damage = 2;  // Через 2 клетки

                if (damage > 0) {
                    player->hp -= damage;
                    printf("\nВетер швырнул вас в препятствие! Вы потеряли %d HP.\n", damage);
                    system("pause");
                }
                break; // Прерываем полет
            }

            map[player->y][player->x] = currentWindTile;
            player->x = checkX;
            player->y = checkY;

            currentWindTile = map[player->y][player->x];
            map[player->y][player->x] = 'Y';
            steps++;

            if (currentWindTile == 'U') break;
        }

        *tileUnder = currentWindTile;
        if (steps == 0) break;
        if (*tileUnder == 'U') return true;
    }
    return false;
}

void to_adventure(struct Player* player, int* current_day, int* current_hour) {
    if (player == NULL) {
        printf("\n[Ошибка] Создайте персонажа!\n");
        return;
    }
    if (player->stamina <= 0) {
        printf("\n[Ошибка] Вы слишком устали для приключений! Нужно поспать.\n");
        return;
    }
    if (*current_hour >= 2 && *current_hour < 8) {
        printf("\n[Ошибка] Ночью слишком опасно!\n");
        return;
    }

    srand((unsigned int)time(NULL));
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    struct Map* level_map = (struct Map*)malloc(sizeof(struct Map));
    level_map->floor_counter = 1;
    bool inDungeon = true;

    while (inDungeon) {
        level_map->size = 7 + rand() % 20;
        level_map->grid = allocate_map(level_map->size);
        generate_floor(level_map->grid, level_map->size, &(player->x), &(player->y));

        char tileUnderPlayer = level_map->grid[player->y][player->x];
        level_map->grid[player->y][player->x] = 'Y';

        bool onCurrentFloor = true;

        while (onCurrentFloor) {
            system("cls");
            printf("HP: %d/%d | Выносливость: %d/%d | Сытость: %d/100 | Золото: %d | Этаж: %d\n",
                player->hp, player->max_hp, player->stamina, player->max_stamina, player->hunger,
                player->gold, level_map->floor_counter);
            printf("----------------------------------------------------------------------\n");

            draw_map(level_map->grid, level_map->size, level_map->floor_counter, hConsole);

            int input = _getch();
            if (input == 0 || input == 224) { _getch(); continue; }

            // === МЕХАНИКА ЕДЫ ===
            if (input == 'e' || input == 'E') {
                int apples = 0, carrots = 0;
                for (int i = 0; i < MAX_ITEMS; i++) {
                    if (player->inventory[i] == ITEM_APPLE) apples++;
                    else if (player->inventory[i] == ITEM_CARROT) carrots++;
                }

                if (apples == 0 && carrots == 0) {
                    printf("\nУ вас нет еды в инвентаре!\n");
                    system("pause");
                    continue;
                }

                printf("\nУ вас есть %d яблока, у вас есть %d морковки.\n", apples, carrots);
                if (apples > 0) printf("Нажмите 1 чтобы съесть яблоко (+15 сытости, +5 сил)\n");
                if (carrots > 0) printf("Нажмите 2 чтобы съесть морковку (+25 сытости, +10 сил)\n");
                printf("Нажмите 0 для отмены\n");

                int choice = _getch();
                int target = -1;

                if (choice == '1' && apples > 0) {
                    target = ITEM_APPLE;
                    player->hunger += 15;
                    player->stamina += 5;
                }
                else if (choice == '2' && carrots > 0) {
                    target = ITEM_CARROT;
                    player->hunger += 25;
                    player->stamina += 10;
                }
                else {
                    continue;
                }

                // Забираем 1 шт из инвентаря
                for (int i = 0; i < MAX_ITEMS; i++) {
                    if (player->inventory[i] == target) {
                        player->inventory[i] = ITEM_EMPTY;
                        break;
                    }
                }
                if (player->hunger > 100) player->hunger = 100;
                if (player->stamina > player->max_stamina) player->stamina = player->max_stamina;

                printf("\nОм-ном-ном! Вы вкусно поели.\n");
                system("pause");
                continue;
            }

            if (input == 'q' || input == 'Q') {
                if (tileUnderPlayer == 'D') { inDungeon = false; break; }
                else { printf("\nВы можете выйти домой только на лестнице D!\n"); system("pause"); continue; }
            }

            int nextX = player->x, nextY = player->y;
            if (input == 'w' || input == 'W') nextY--;
            else if (input == 's' || input == 'S') nextY++;
            else if (input == 'd' || input == 'D') nextX++;
            else if (input == 'a' || input == 'A') nextX--;
            else continue;

            if (nextX >= 0 && nextX < level_map->size && nextY >= 0 && nextY < level_map->size) {
                char targetCell = level_map->grid[nextY][nextX];

                if (targetCell != '#' && targetCell != '!') {

                    // === ПОДБОР ЕДЫ ===
                    if (targetCell == 'A' || targetCell == 'C') {
                        int item_id = (targetCell == 'A') ? ITEM_APPLE : ITEM_CARROT;
                        bool picked = false;
                        for (int i = 0; i < MAX_ITEMS; i++) {
                            if (player->inventory[i] == ITEM_EMPTY) {
                                player->inventory[i] = item_id;
                                picked = true;
                                break;
                            }
                        }
                        if (!picked) {
                            printf("\nИнвентарь полон! Вы не можете взять еду.\n");
                            system("pause");
                            continue; // Стоим на месте
                        }
                        printf("\nВы подобрали %s!\n", (targetCell == 'A') ? "Яблоко" : "Морковку");
                        level_map->grid[nextY][nextX] = '0'; // Еда исчезает
                        targetCell = '0';
                    }

                    // === ТРАТА СИЛ И ГОЛОД ===
                    player->stamina -= 1;
                    player->steps_taken++;
                    if (player->steps_taken % 5 == 0) player->hunger -= 1; // Голод падает каждые 5 шагов

                    if (player->hunger < 20) {
                        player->hp -= 1;
                    }

                    level_map->grid[player->y][player->x] = tileUnderPlayer;
                    player->x = nextX;
                    player->y = nextY;
                    tileUnderPlayer = targetCell;
                    level_map->grid[player->y][player->x] = 'Y';

                    // Проверка смерти
                    if (player->hp <= 0) {
                        printf("\nВы погибли от истощения и ран...\n");
                        system("pause");
                        inDungeon = false;
                        break;
                    }

                    if (player->stamina <= 0 || (*current_hour >= 2 && *current_hour < 8)) {
                        printf("\nВы падаете без сил! Спасатели возвращают вас домой (-50%% золота)\n");
                        player->gold /= 2;
                        system("pause");
                        inDungeon = false;
                        break;
                    }

                    if (tileUnderPlayer == 'U') {
                        level_map->floor_counter++;
                        player->stamina -= 5;
                        player->hunger -= 5; // Смена этажа отнимает голод
                        add_hours_logic(current_day, current_hour, 1);
                        player->gold += ADVENTURE_BASE_GOLD_MULT * (level_map->floor_counter - 1);
                        onCurrentFloor = false;
                        continue;
                    }

                    bool blown = apply_wind(level_map->grid, level_map->size, player, &tileUnderPlayer);

                    if (player->hp <= 0) {
                        printf("\nВы разбились насмерть об препятствие...\n");
                        system("pause");
                        inDungeon = false;
                        break;
                    }

                    if (blown) {
                        level_map->floor_counter++;
                        player->stamina -= 5;
                        player->hunger -= 5;
                        add_hours_logic(current_day, current_hour, 1);
                        player->gold += ADVENTURE_BASE_GOLD_MULT * (level_map->floor_counter - 1);
                        onCurrentFloor = false;
                        continue;
                    }
                }
            }
        }
        free_map(level_map->grid, level_map->size);
    }
    printf("\nПамять структуры карты по адресу %p освобождена\n", (void*)level_map);
    free(level_map);
}