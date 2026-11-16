#define _CRT_SECURE_NO_WARNINGS
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "Constants.h"

void write_diary(char farmer_name[], int current_day, int current_hour, int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void decode_notes(const char* farmer_name, int current_day, const int* inventory, char item_names[][MAX_NAME_LEN]);
void replace_tags_in_line(const char* line, char* result, int max_result_size, const char* farmer_name, const char* day_str, const char* first_item);
int unpack_rle_line_logic(const char* line, char* unpacked, int max_unpacked_size);
void output_map(void);

#endif // FILEMANAGER_H