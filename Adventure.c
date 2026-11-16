#include "Adventure.h"
#include "Constants.h"
#include "Player.h"
#include "TimeLogic.h" // Для функции add_hours_logic()
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



    int back;
    int hand;
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
            if (i == 0 || i == size - 1 || j == 0 || j == size - 1) {
                map[i][j] = '#';
            }
            else {
                map[i][j] = (rand() % 100 < 20) ? '!' : '0';
            }
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
        if (map[wy][wx] == '0') {
            map[wy][wx] = windDirs[rand() % 4];
        }
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

            printf("%c", cell);
            SetConsoleTextAttribute(hConsole, 7);
        }
        printf("\n");
    }
    printf("Управление: [W,A,S,D] | [Q - Выйти домой]\n");
}

bool apply_wind(char** map, int size, int* pX, int* pY, char* tileUnder) {
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
            int checkX = *pX + dx;
            int checkY = *pY + dy;

            if (checkX < 0 || checkX >= size || checkY < 0 || checkY >= size) break;
            if (map[checkY][checkX] == '#' || map[checkY][checkX] == '!') break;

            map[*pY][*pX] = currentWindTile;
            *pX = checkX;
            *pY = checkY;

            currentWindTile = map[*pY][*pX];
            map[*pY][*pX] = 'Y';
            steps++;

            if (currentWindTile == 'U') break;
        }

        *tileUnder = currentWindTile;
        if (steps == 0) break;

        if (*tileUnder == 'U') {
            return true;
        }
    }
    return false;
}

void to_adventure(struct Player* player, int* current_day, int* current_hour) {
    if (player == NULL) {
        printf("\n[Ошибка] Создайте персонажа, чтобы отправиться в приключение!\n");
        return;
    }
    if (player->stamina <= 0) {
        printf("\n[Ошибка] Вы слишком устали для приключений! Нужно поспать.\n");
        return;
    }
    if (*current_hour >= 2 && *current_hour < 8) {
        printf("\n[Ошибка] На дворе глухая ночь, слишком опасно! Ложитесь спать.\n");
        return;
    }

    srand((unsigned int)time(NULL));
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    struct Map* level_map = (struct Map*)malloc(sizeof(struct Map));
    level_map->floor_counter = 1;

    bool inDungeon = true;

    while (inDungeon) {
        level_map->size = 5 + rand() % 26;
        level_map->grid = allocate_map(level_map->size);

        generate_floor(level_map->grid, level_map->size, &(player->x), &(player->y));

        char tileUnderPlayer = level_map->grid[player->y][player->x];
        level_map->grid[player->y][player->x] = 'Y';

        bool onCurrentFloor = true;

        while (onCurrentFloor) {
            system("cls");
            printf("HP: %d/%d | Выносливость: %d/%d | Золото: %d | Этаж: %d | Время: %02d:00\n",
                player->hp, player->max_hp, player->stamina, player->max_stamina,
                player->gold, level_map->floor_counter, *current_hour);
            printf("----------------------------------------------------------------------\n");

            draw_map(level_map->grid, level_map->size, level_map->floor_counter, hConsole);

            int input = _getch();
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

            int nextX = player->x;
            int nextY = player->y;

            if (input == 'w' || input == 'W') nextY--;
            else if (input == 's' || input == 'S') nextY++;
            else if (input == 'd' || input == 'D') nextX++;
            else if (input == 'a' || input == 'A') nextX--;
            else continue;

            if (nextX >= 0 && nextX < level_map->size && nextY >= 0 && nextY < level_map->size) {
                if (level_map->grid[nextY][nextX] != '#' && level_map->grid[nextY][nextX] != '!') {

                    player->stamina -= 1;

                    level_map->grid[player->y][player->x] = tileUnderPlayer;
                    player->x = nextX;
                    player->y = nextY;
                    tileUnderPlayer = level_map->grid[player->y][player->x];
                    level_map->grid[player->y][player->x] = 'Y';

                    if (player->stamina <= 0 || (*current_hour >= 2 && *current_hour < 8)) {
                        printf("\nВы падаете без сил! Спасатели возвращают вас домой...\n");
                        printf("За услуги спасения вы теряете половину золота!\n");
                        player->gold /= 2;
                        system("pause");
                        inDungeon = false;
                        break;
                    }

                    if (tileUnderPlayer == 'U') {
                        level_map->floor_counter++;
                        player->stamina -= 5;
                        add_hours_logic(current_day, current_hour, 1);

                        int reward = ADVENTURE_BASE_GOLD_MULT * (level_map->floor_counter - 1);
                        player->gold += reward;

                        printf("\nПереход на следующий этаж! Получено золота: %d. Усталость: -5, Прошел 1 час.\n", reward);
                        system("pause");

                        if (player->stamina <= 0 || (*current_hour >= 2 && *current_hour < 8)) {
                            printf("\nВы падаете без сил прямо на лестнице! Спасатели возвращают вас домой...\n");
                            player->gold /= 2;
                            system("pause");
                            inDungeon = false;
                            break;
                        }

                        onCurrentFloor = false;
                        continue;
                    }

                    bool blownToNextFloor = apply_wind(level_map->grid, level_map->size, &(player->x), &(player->y), &tileUnderPlayer);
                    if (blownToNextFloor) {
                        level_map->floor_counter++;
                        player->stamina -= 5;
                        add_hours_logic(current_day, current_hour, 1);

                        int reward = ADVENTURE_BASE_GOLD_MULT * (level_map->floor_counter - 1);
                        player->gold += reward;

                        printf("\nВас сдуло прямиком на следующий этаж! Получено золота: %d. Усталость: -5, Прошел 1 час.\n", reward);
                        system("pause");

                        if (player->stamina <= 0 || (*current_hour >= 2 && *current_hour < 8)) {
                            printf("\nВы падаете без сил прямо на лестнице! Спасатели возвращают вас домой...\n");
                            player->gold /= 2;
                            system("pause");
                            inDungeon = false;
                            break;
                        }

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

    system("cls");
    printf("Приключение завершено. Вы вернулись на ферму!\n");
}