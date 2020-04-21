#pragma once


void print_table(uint8_t input_array[], size_t length) {
	for (int i = 0; i < length; i++) {
		putchar('[');
		for (int j = 0; j < length; j++) {
			printf("%02X", input_array[i * length + j]);
			if(j != length - 1) putchar(' ');
		}
		printf("]\n");
	}
}