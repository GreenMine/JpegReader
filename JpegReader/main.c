#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "decoder.h"
#include "helper.h"

#define MARKER_COUNT 3


#define ONE_BITE 0
#define TWO_BITE 1

#define DQT_LENGTH 8

int comment_marker();
int dqt_marker();
int sof0_marker();

short get_next_2_bytes(FILE* file);

void chack_marker(FILE* file, int second);

FILE* fp;

int main(int argc, char* args[]) {
	if (argc == 1)
		return 1;
	
	struct {
		uint8_t type;
		const char* name;
		int(*f)();
	} markers[MARKER_COUNT] = {
		{0xFE, "COMMENT", comment_marker}, {0xDB, "DQT", dqt_marker}, {0xC0, "SOF0", sof0_marker}
	};

	if ((fp = fopen(args[1], "rb")) == NULL) {
		printf("Error to open file %s\n", args[1]);
		return 1;
	}

	//printf("%X:%X\n", fgetc(fp), fgetc(fp));
	chack_marker(fp, 0xD8); // CHECKING EXISTENCE OF START MARKER

	uint8_t current_byte;

	while ((current_byte = fgetc(fp)) != EOF) {
		if (current_byte != 0xFF) {
			printf("UNKNOWN ERROR! Previous marker dont be cleaned full???\n");
			return 1;
		}
	
		int current_marker = fgetc(fp);
		int is_found = false;
		for (int i = 0; i < MARKER_COUNT; i++) {
			if (current_marker == markers[i].type) {
				printf("Marker %s was found! Executing...\n", markers[i].name);
				markers[i].f();
				is_found = true;
			}
		}
		if (!is_found) {
			printf("UNKNOWN MARKER! Marker 0x%02X not be found\n", current_marker);
			return 1;
		}
	}

	return 0;
}

int comment_marker() {

	uint16_t comment_length = get_next_2_bytes(fp) - 2; // GET LENGTH OF COMMENT

	/*PRINT COMMENT*/
	printf("Comment in image(%dbyte.): ", comment_length);

	for (uint16_t i = 0; i < comment_length; i++)
		printf("%c", fgetc(fp));
	putchar('\n');

	return 0;
}

int dqt_marker() {

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

	uint8_t* new_dqt_table = array_to_zigzag_order(dqt_table, 8); // :thinking:

	printf("ZigZag ordering table:\n");
	print_table(new_dqt_table, 8);

	return 0;
}

int sof0_marker() {

	uint16_t marker_length = get_next_2_bytes(fp);
	uint8_t precision = fgetc(fp);
	uint16_t heigth = get_next_2_bytes(fp);
	uint16_t width = get_next_2_bytes(fp);
	size_t count_of_channels = fgetc(fp);

	printf("Length: %d\nPrecision: %d\nImage height: %dpx.\nImage width: %dpx.\nCount of channels: %d\n", marker_length, precision, heigth, width, count_of_channels);

	sof0_channel_t *channels = malloc(sizeof(sof0_channel_t) * count_of_channels);

	for (int i = 0; i < count_of_channels; i++) {
		uint8_t id = fgetc(fp);
		uint8_t thinning = fgetc(fp);
		channels[i].id = id;
		channels[i].horizontal_thinning = get_half_of_byte(thinning, First);
		channels[i].vertical_thinning = get_half_of_byte(thinning, Second);
		channels[i].table_id = fgetc(fp);
		print_channel(channels[i]);
	}

}