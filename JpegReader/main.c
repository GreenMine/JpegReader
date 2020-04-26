#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>
#include "decoder.h"
#include "helper.h"
#include "huffman.h"
#include "type.h"
#include "bitstream.h"

#define MARKER_COUNT 6

#define ONE_BITE 0
#define TWO_BITE 1

#define DQT_LENGTH 8

#define HUFFMAN_CODE_LENGTH 16

int comment_marker();
int dqt_marker();
int sof0_marker();
int huffman_marker();
int app_marker();

short get_next_2_bytes(FILE* file);

void chack_marker(FILE* file, int second);

FILE* fp;

double PCFreq;

LONGLONG CounterStart;

void StartCounter()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		printf("Error\n");

	PCFreq = ((double)li.QuadPart) / 1000.0;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}
double GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return ((double)(li.QuadPart - CounterStart)) / PCFreq;
}

int main2(int argc, char* args[]) {


	avl_tree_t* tree = create_element();

	add_value(tree, (huffman_code_t){ 1, 1, 0, 03 });
	putchar('\n');
	add_value(tree, (huffman_code_t){ 2, 1, 2, 02 });

	print2D(tree, 0);

	if ((fp = fopen(args[1], "rb")) == NULL) {
		printf("Error to open file %s\n", args[1]);
		return 1;
	}

	bitstream_t* stream = bitstream_initialize(fp);

	for (int i = 0; i < 256; i++) {
		printf("%d", get_next_bit(stream));
	}

	return 0;
}

int main(int argc, char* args[]) {
	if (argc == 1)
		return 1;
	
	struct {
		type_between_t type;
		const char* name;
		int(*f)();
	} markers[MARKER_COUNT] = {
		{new_value(0xFE), "COMMENT", comment_marker}, {new_value(0xDB), "DQT", dqt_marker}, {new_value(0xC0), "SOF0", sof0_marker}, {new_value(0xC4), "TABLE OF HUFFMAN", huffman_marker}, {new_two_value(0xE0, 0xEF), "APP", app_marker}
	};

	if ((fp = fopen(args[1], "rb")) == NULL) {
		printf("Error to open file %s\n", args[1]);
		return 1;
	}
	StartCounter();
	//printf("%X:%X\n", fgetc(fp), fgetc(fp));
	chack_marker(fp, 0xD8); // CHECKING EXISTENCE OF START MARKER

	uint8_t current_byte;

	while ((current_byte = fgetc(fp)) != EOF) {
		if (current_byte != 0xFF) {
			printf("UNKNOWN ERROR! Previous marker dont be cleaned full???\n");
			return 1;
		}
	
		uint16_t current_marker = fgetc(fp);
		bool is_found = false;
		
		for (int i = 0; i < MARKER_COUNT; i++) {
			if (is_equals(current_marker, markers[i].type)) {
				printf("Marker %s was found! Executing...\n", markers[i].name);
				markers[i].f();
				is_found = true;
			}
		}
		if (!is_found) {
			printf("UNKNOWN MARKER! Marker 0x%02X not be found\n", current_marker);
			printf("%f\n", GetCounter());
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
	uint8_t length_value_in_table = get_half_of_byte(temp_value, First);
	uint8_t table_id = get_half_of_byte(temp_value, Second);

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
		channels[i] = (sof0_channel_t){ .id = id, .horizontal_thinning = get_half_of_byte(thinning, First), .vertical_thinning = get_half_of_byte(thinning, Second), .table_id = fgetc(fp) };
		print_channel(channels[i]);
	}
	return 0;
}

int huffman_marker() {

	uint16_t marker_length = get_next_2_bytes(fp);
	uint8_t temp_for_1_byte = fgetc(fp);
	uint8_t class = get_half_of_byte(temp_for_1_byte, First);
	uint8_t table_id = get_half_of_byte(temp_for_1_byte, Second);
	uint16_t codes_length = marker_length - 3 - HUFFMAN_CODE_LENGTH;

	printf("Length: %d\nClass: %s coefficients\nTable id: %d\nCount of Huffman codes: %d\n", marker_length, class == 0 ? "DC" : "AC", table_id, codes_length);

	int i;

	uint16_t code = 0;

	huffman_code_t codes[HUFFMAN_CODE_LENGTH] = {0x00};

	for (i = 0; i < codes_length; i++) 
		codes[i] = (huffman_code_t){ .length = i + 1, .count = fgetc(fp) };

	fseek(fp, HUFFMAN_CODE_LENGTH - codes_length, SEEK_CUR);

	avl_tree_t* tree = create_element();

	for (i = 0; i < codes_length; i++) {
		for (int j = 0; j < codes[i].count; j++) {
			codes[i].value = fgetc(fp);
			codes[i].code = code;
			add_value(tree, codes[i]);
			print_huffman(codes[i]);
			code++;
		}
		code <<= 1;
	}

	return 0;
}

int app_marker() {
	uint16_t marker_length = get_next_2_bytes(fp) - 2;

	printf("Information about packager application(%d):\n");

	for (int i = 1; i <= marker_length; i++) {
		printf("%02X ", fgetc(fp));
		if (!(i % 16)) putchar('\n');
	}
	putchar('\n');
	return 0;
}