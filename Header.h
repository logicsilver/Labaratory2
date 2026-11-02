#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <string.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

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
#define menu_task_11     11

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

// Константы размеров
#define MAX_ITEMS        10
#define MAX_FARMER_NAME  32
#define MAX_NAME_LEN     64
#define MAX_LINE_LEN     512
#define MAX_UNPACKED_LEN 1024
#define MAX_RESULT_LEN   2048
#define HOURS_PER_DAY    24


void skip_utf8_bom(FILE* f);
int utf8_char_len(unsigned char c);
void read_utf8_word(char* buffer, int max_bytes);

int get_safe_int(void);

void init_default_items(char item_names[MAX_ITEMS][MAX_NAME_LEN]);
bool load_items_from_file(const char* filename, char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void add_hours_logic(int* current_day, int* current_hour, int hours);
void deduplicate_logic(int inventory[]);
void replace_tags_in_line(const char* line, char* result, int max_result_size,
    const char* farmer_name, const char* day_str, const char* first_item);
int unpack_rle_line_logic(const char* line, char* unpacked, int max_unpacked_size);
void show_clock(int current_day, int current_hour);
void fast_forward_time(int* current_day, int* current_hour);
void print_inventory(int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void put_item(int inventory[]);
void drop_item(int inventory[]);
void remove_duplicates(int inventory[]);
void find_item(int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void write_diary(char farmer_name[], int current_day, int current_hour, int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void decode_notes(const char* farmer_name, int current_day, const int* inventory, char item_names[][MAX_NAME_LEN]);
void generate_map(int height);
void output_map(void);
void to_adventure(void);
void print_menu(const char* farmer_name);
