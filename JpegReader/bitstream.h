#pragma once

typedef struct BitStream {
	FILE* fp;
	uint8_t current_byte;
	int8_t iterator;
} bitstream_t;


bitstream_t* bitstream_initialize(FILE *fp) {
	bitstream_t* stream = malloc(sizeof(bitstream_t));
	stream->iterator = -1;
	stream->current_byte = 0;
	stream->fp = fp;
	return stream;
}

uint8_t get_next_bit(bitstream_t* stream) {
	if (stream->iterator == -1) {
		stream->current_byte = fgetc(stream->fp);
		stream->iterator = 7;
	}
	return stream->current_byte & (1 << stream->iterator--) ? 1 : 0;
}