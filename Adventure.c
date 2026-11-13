#include "Adventure.h"
#include "Constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <windows.h>
#include <conio.h>

// ... Сюда вставляешь allocate_map, free_map, generate_floor, 
// draw_map, apply_wind и саму to_adventure ...

char** allocate_map(int size) {
    char** map = (char**)malloc(size * sizeof(char*));
    for (int i = 0; i < size; i++) {
        map[i] = (char*)malloc(size * sizeof(char));
    }
    return map;
}

// Очистка памяти
void free_map(char** map, int size) {
    for (int i = 0; i < size; i++) {
        free(map[i]);
    }
    free(map);
}

// Генерация этажа (стены, препятствия, игрок, лестницы, ветер)
void generate_floor(char** map, int size, int* pX, int* pY) {
    // 1. Обычные клетки, стены и препятствия
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

    // 2. Точка входа (лестница 'D' - домой/назад)
    do {
        *pX = 1 + rand() % (size - 2);
        *pY = 1 + rand() % (size - 2);
    } while (map[*pY][*pX] != '0');
    map[*pY][*pX] = 'D';

    // 3. Лестница на следующий этаж 'U'
    int ladderX, ladderY;
    do {
        ladderX = 1 + rand() % (size - 2);
        ladderY = 1 + rand() % (size - 2);
    } while (map[ladderY][ladderX] != '0');
    map[ladderY][ladderX] = 'U';

    // 4. Ветер (Генерируется один раз для этажа, больше не меняется)
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

// Отрисовка карты
void draw_map(char** map, int size, int floorCounter, HANDLE hConsole) {
    system("cls");
    printf("--- ЭТАЖ %d ---\n", floorCounter);

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

// Логика полета по ветру
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

        // Если ветер сдул прямо на лестницу - сообщаем главному циклу
        if (*tileUnder == 'U') {
            return true;
        }
    }
    return false;
}

// --- ОСНОВНАЯ ФУНКЦИЯ ---

void to_adventure(void) {
    srand((unsigned int)time(NULL));
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    int floorCounter = 1;
    bool inDungeon = true;

    while (inDungeon) {
        // 1. Случайный размер этажа от 5 до 30
        int currentSize = 5 + rand() % 26;

        // 2. Выделение памяти для текущего этажа
        char** map = allocate_map(currentSize);

        // 3. Генерация этажа и установка игрока
        int playerX, playerY;
        generate_floor(map, currentSize, &playerX, &playerY);

        char tileUnderPlayer = map[playerY][playerX]; // Сохраняем клетку под игроком ('D')
        map[playerY][playerX] = 'Y';                  // Ставим игрока

        bool onCurrentFloor = true;

        while (onCurrentFloor) {
            draw_map(map, currentSize, floorCounter, hConsole);

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

            int nextX = playerX, nextY = playerY;
            if (input == 'w' || input == 'W') nextY--;
            else if (input == 's' || input == 'S') nextY++;
            else if (input == 'd' || input == 'D') nextX++;
            else if (input == 'a' || input == 'A') nextX--;
            else continue;

            // Проверка на столкновение
            if (nextX >= 0 && nextX < currentSize && nextY >= 0 && nextY < currentSize) {
                if (map[nextY][nextX] != '#' && map[nextY][nextX] != '!') {
                    // Восстанавливаем старую клетку
                    map[playerY][playerX] = tileUnderPlayer;

                    // Перемещаем игрока
                    playerX = nextX;
                    playerY = nextY;
                    tileUnderPlayer = map[playerY][playerX];
                    map[playerY][playerX] = 'Y';

                    // Проверка лестницы
                    if (tileUnderPlayer == 'U') {
                        floorCounter++;
                        printf("\nПереход на следующий этаж...\n");
                        system("pause");
                        onCurrentFloor = false;
                        continue;
                    }

                    // Применение эффекта ветра
                    bool blownToNextFloor = apply_wind(map, currentSize, &playerX, &playerY, &tileUnderPlayer);
                    if (blownToNextFloor) {
                        floorCounter++;
                        printf("\nВас сдуло прямиком на следующий этаж!...\n");
                        system("pause");
                        onCurrentFloor = false;
                        continue;
                    }
                }
            }
        }
        // Обязательная очистка памяти при выходе с текущего этажа (независимо от того, вышли ли мы совсем или идем дальше)
        free_map(map, currentSize);
    }

    system("cls");
    printf("Спасибо за игру!\n");
}