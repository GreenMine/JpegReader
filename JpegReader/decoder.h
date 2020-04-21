#pragma once

#define UP_DIAG 3
#define DOWN_DIAG 4

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