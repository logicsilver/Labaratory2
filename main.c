#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>

#define menu_exit    0
#define menu_task_1  1
#define menu_task_2  2
#define menu_task_3  3
#define menu_task_4  4
#define menu_task_5  5
#define menu_task_6  6




#define item_empty          0  //Пустота
#define item_wood           1  //Дерево
#define item_stone          2  //Камень
#define item_seeds          3  //Семена
#define item_iron           4  //Железо
#define item_gold           5  //Золото
#define item_hoe            6  //Мотыга
#define item_fertilizer     7  //Удобрение
#define item_water_can      8  //Лейка
#define item_milk           9  //Молоко


int main() {

	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);

	bool run = true;

	int current_day = 1;
	int current_hour = 8;
	int inventory[10] = { 0 };


	inventory[0] = item_hoe;
	inventory[1] = item_water_can;
	inventory[2] = item_seeds;
	inventory[3] = item_wood;
	inventory[4] = item_seeds;
	inventory[5] = item_stone;
	inventory[6] = item_hoe;
	inventory[7] = item_seeds;
	inventory[8] = item_seeds;
	inventory[9] = item_stone;

	int index_item;
	int new_id_item;

	while (run) {
		int choice;

		printf("\n====MAIN_MENU====\n");
		printf("\n   [0] - Выход\n");
		printf("\n   [1] - Посмотреть на часы\n");
		printf("\n   [2] - Промотать время\n");
		printf("\n   [3] - Просмотреть инвентарь\n");
		printf("\n   [4] - Положить предмет в слот\n");
		printf("\n   [5] - Выбросить предмет\n");
		printf("\n   [6] - Устранение дубликатов\n");
		printf("\nВведите пунт -> ");


		while (scanf("%d", &choice) != 1) {
			printf("Ошибка. Введено не число. Повторите ввод -> ");
			int c; 
			while ((c = getchar()) != '\n' && c != EOF); // Очистка буфера, чтобы scanf не зацикливался при вводе букв
		};

		switch (choice) {

		case menu_task_1:
			printf("\n[Часы фермера]\n");
			printf("\nТекущее время: День %d, %02d:00\n", current_day, current_hour);
			break;

		case menu_task_2:
			printf("\n[Перемотка времени]\n");
			printf("\nСколько часов вы хотите потратить на работу? -> ");

			int hours_to_work;
			while (scanf("%d", &hours_to_work) != 1) {
				printf("\nОшибка. Введено не число. Повторите ввод -> ");
				int c; 
				while ((c = getchar()) != '\n' && c != EOF);
			}

			if (hours_to_work < 0) {
				printf("\nОшибка. Время не может  идти назад!\n");
			}
			else {
				current_hour += hours_to_work;
				if (current_hour >= 24) {
					current_day += current_hour / 24;
					current_hour = current_hour % 24;
				}
				printf("\nВы успешно поработали.\n");
			}
			break;

		case menu_task_3:
			printf("\n[Содержимое инвентаря]\n");

			for (int i = 0; i < 10; i++) {
				printf("Слот %d: [%d] ", i, inventory[i]);

				switch (inventory[i]) {
				case item_empty:
					printf("(Пусто)\n");
					break;
				case item_wood:
					printf("(Дерево)\n");
					break;
				case item_stone:
					printf("(Камень)\n");
					break;
				case item_seeds:
					printf("(Семена)\n");
					break;
				case item_iron:
					printf("(Железо)\n");
					break;
				case item_gold:
					printf("(Золото)\n");
					break;
				case item_hoe:
					printf("(Мотыга)\n");
					break;
				case item_fertilizer:
					printf("(Удобрение)\n");
					break;
				case item_water_can:
					printf("(Лейка)\n");
					break;
				case item_milk:
					printf("(Молоко)\n");
					break;
				default:
					printf("(Неизвестный предмет)\n");
					break;
				}
			}
			break;

		case menu_task_4:
			printf("\n[Положить прдемет в слот]\n");

			printf("\nПорядок предмета -> ");
			while (scanf("%d", &index_item) != 1) {
				printf("Ошибка. Введено не число. Повторите ввод -> ");
				int c; 
				while ((c = getchar()) != '\n' && c != EOF);
			}

			if (index_item < 0 || index_item > 9) {
				printf("Ошибка! Неверный индекс слота. Допустимо только от 0 до 9.\n");
				break;
			}

			printf("\nId предмета -> ");
			while (scanf("%d", &new_id_item) != 1) {
				printf("\nОшибка. Введено не число. Повторите ввод -> ");
				int c; 
				while ((c = getchar()) != '\n' && c != EOF);
			}

			if (new_id_item < 0 || new_id_item > 9) {
				printf("\nОшибка! Неверный id предмета. Допустимо только от 0 до 9.\n");
				break;
			}

			inventory[index_item] = new_id_item;

			break;
		case menu_task_5:
			printf("\n[Выбросить предмет из слота]\n");

			printf("\nПорядок предмета -> ");
			while (scanf("%d", &index_item) != 1) {
				printf("Ошибка. Введено не число. Повторите ввод -> ");
				int c; 
				while ((c = getchar()) != '\n' && c != EOF);
			}
			if (index_item < 0 || index_item > 9) {
				printf("Ошибка! Неверный индекс слота. Допустимо только от 0 до 9.\n");
				break;
			}

			inventory[index_item] = 0;

			break;
		case menu_task_6:
			printf("\n[Устранение дубликатов]\n");

			printf("Инвент до -> ");
			for (int i = 0; i < 10; i++) {
				printf("%d ", inventory[i]);
			}
			printf("\n");

			for (int i = 0; i < 10; i++) {
				if (inventory[i] == item_empty) {
					continue;
				}

				for (int j = i + 1; j < 10; j++) {
					if (inventory[j] == inventory[i]) {
						inventory[j] = item_empty;
					}
				}
			}

			printf("Инветн после -> ");
			for (int i = 0; i < 10; i++) {
				printf("%d ", inventory[i]);
			}
			printf("\n");
			break;

		case menu_exit:
			printf("\n[Выполняется  выход]\n");
			run = false;
			break;

		}


	}
}
