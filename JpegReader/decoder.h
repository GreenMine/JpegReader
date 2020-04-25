#pragma once

#define UP_DIAG 3
#define DOWN_DIAG 4

typedef struct Sof0_Channel {
	uint8_t id;
	uint8_t horizontal_thinning;
	uint8_t vertical_thinning;
	uint8_t table_id;
} sof0_channel_t;

typedef struct HuffmanCode {
	uint8_t length;
	uint8_t count;
	uint16_t code;
	uint8_t value;
} huffman_code_t;

uint8_t *array_to_zigzag_order(uint8_t input_array[], int length) {
	int i = 0, j = 0, movement;
	uint8_t* returned_array = malloc(sizeof(uint8_t) * length * length);
	for (int num = 0; num < length * length; num++) {
		returned_array[num] = input_array[i * length + j];

		if (i == length - 1 || i == 0) {
			j++;
			num++;
			returned_array[num] = input_array[i * length + j];
		}
		else if (j == length - 1 || j == 0) {
			i++;
			num++;
			returned_array[num] = input_array[i * length + j];
		}
		if(i == length - 1 || j == 0) movement = UP_DIAG;
		if(j == length - 1 || i == 0) movement = DOWN_DIAG;

		if (movement == DOWN_DIAG) {
			j--;
			i++;
		}
		if (movement == UP_DIAG) {
			i--;
			j++;
		}
	}
	return returned_array;
}