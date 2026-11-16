#ifndef CONSTANTS_H
#define CONSTANTS_H

// Перечисление для пунктов меню
enum MenuOption {
    MENU_EXIT = 0,
    MENU_CREATE_PLAYER = 1,
    MENU_PLAYER_CARD = 2,
    MENU_CLOCK = 3,
    MENU_WAIT = 4,
    MENU_SLEEP = 5,
    MENU_INV_SHOW = 6,
    MENU_INV_PUT = 7,
    MENU_INV_DROP = 8,
    MENU_INV_DEDUPLICATE = 9,
    MENU_INV_SEARCH = 10,
    MENU_DIARY = 11,
    MENU_DECODE = 12,
    MENU_UNPACK_MAP = 13,
    MENU_ADVENTURE = 14
};

#define MAX_ITEMS        12
#define MAX_NAME_LEN     64
#define MAX_LINE_LEN     512
#define MAX_UNPACKED_LEN 1024
#define MAX_RESULT_LEN   2048

#define HOURS_PER_DAY    24
#define ADVENTURE_BASE_GOLD_MULT 50 // Базовый множитель золота за этаж

enum ItemID {
    ITEM_EMPTY = 0,
    ITEM_WOOD = 1,
    ITEM_STONE = 2,
    ITEM_SEEDS = 3,
    ITEM_IRON = 4,
    ITEM_GOLD = 5,
    ITEM_HOE = 6,
    ITEM_FERTILIZER = 7,
    ITEM_WATER_CAN = 8,
    ITEM_MILK = 9,
    ITEM_APPLE = 10,
    ITEM_CARROT = 11
};

#endif