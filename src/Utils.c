#include "Utils.h"
#include <stdio.h>

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