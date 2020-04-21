#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "decoder.h"
#include "helper.h"

#define ONE_BITE 0
#define TWO_BITE 1

#define DQT_LENGTH 8

short get_next_2_bytes(FILE* file);

void chack_marker(FILE* file, int second);

int main(int argc, char* args[]) {
	if (argc == 1)
		return 1;
	
	FILE* fp;
	

	if ((fp = fopen(args[1], "rb")) == NULL) {
		printf("Error to open file %s\n", args[1]);
		return 1;
	}

	//printf("%X:%X\n", fgetc(fp), fgetc(fp));
	chack_marker(fp, 0xD8); // CHECKING EXISTENCE OF START MARKER




	chack_marker(fp, 0xFE); // CHECKING EXISTENCE OF COMMENT MARKER

	uint16_t comment_length = get_next_2_bytes(fp) - 2; // GET LENGTH OF COMMENT

	/*PRINT COMMENT*/
	printf("Comment in image(%dbyte.): ", comment_length);

	for (uint16_t i = 0; i < comment_length; i++)
		printf("%c", fgetc(fp));
	putchar('\n');

	chack_marker(fp, 0xDB); // CHECKING EXISTENCE OF DQT MARKER

	uint16_t dqt_length = get_next_2_bytes(fp) - 3;

	uint8_t temp_value = fgetc(fp);
	uint8_t length_value_in_table = temp_value / 0x10;
	uint8_t table_id = temp_value & 0x01;

	printf("Length of table: %d\nLength of value in table = %d\nTable id = %d\n", dqt_length, length_value_in_table, table_id);

	uint8_t dqt_table[DQT_LENGTH][DQT_LENGTH]; // NEED DELETE CONST BTW

	for (int i = 0; i < DQT_LENGTH; i++) 
		for (int j = 0; j < DQT_LENGTH; j++) 
			dqt_table[i][j] = fgetc(fp);
	
	printf("Standart dqt table:\n");
	//printf("%03x\n", dqt_table[2][3]);
	print_table(dqt_table, DQT_LENGTH);

	uint8_t *new_dqt_table = array_to_zigzag_order(dqt_table, 8); // :thinking:

	printf("ZigZag ordering table:\n");
	print_table(new_dqt_table, 8);

	return 0;
}

short get_next_2_bytes(FILE* file) {
	return (fgetc(file) << 8) + fgetc(file);
}

void chack_marker(FILE* file, int second) {
	if (fgetc(file) != 0xFF || fgetc(file) != second) {
		printf("Marker [FF %02X] not be found! Exiting...\n", second);
		system("pause");
		exit(0);
	}
}

