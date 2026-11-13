#define _CRT_SECURE_NO_WARNINGS
#ifndef INVENTORY_H
#define INVENTORY_H

#include "Constants.h"
#include <stdbool.h>

void init_default_items(char item_names[MAX_ITEMS][MAX_NAME_LEN]);
bool load_items_from_file(const char* filename, char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void print_inventory(int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void put_item(int inventory[]);
void drop_item(int inventory[]);
void remove_duplicates(int inventory[]);
void deduplicate_logic(int inventory[]);
int find_item_id_logic(char item_names[MAX_ITEMS][MAX_NAME_LEN], const char* search_name);
void find_item(int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);

#endif // INVENTORY_H
