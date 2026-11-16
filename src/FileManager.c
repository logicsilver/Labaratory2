#include "FileManager.h"
#include "Constants.h"
#include "Utf8Util.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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

    FILE* in = fopen("data/input.txt", "r");
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