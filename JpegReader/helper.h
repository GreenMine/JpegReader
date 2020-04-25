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

uint16_t get_next_2_bytes(FILE* file) {
	return (fgetc(file) << 8) + fgetc(file);
}

void chack_marker(FILE* file, int second) {
	if (fgetc(file) != 0xFF || fgetc(file) != second) {
		printf("Marker [FF %02X] not be found! Exiting...\n", second);
		system("pause");
		exit(0);
	}
}

void print_binary(uint16_t code) {
	for (int i = 4; i >= 0; i--) 
		putchar(code & (1 << i) ? '1' : '0');
}

void print_channel(sof0_channel_t channel) {
	printf("Id: %d\nHorizontal thinning: %d\nVertical thinning: %d\nDqt table id: %d\n", channel.id, channel.vertical_thinning, channel.horizontal_thinning, channel.table_id);
}

void print_huffman(huffman_code_t huffman) {
	printf("Length: %d\nCount: %d\nValue: %d\nCode: %d(", huffman.length, huffman.count, huffman.value, huffman.code);
	print_binary(huffman.code);
	printf(")\n---------\n");
}

typedef enum HalfOfByte {
	First,
	Second
} half_of_byte_t;


uint8_t get_half_of_byte(uint8_t value, half_of_byte_t type) {
	return type == First ? value >> 4 & 0xF : value & 0xF;
}