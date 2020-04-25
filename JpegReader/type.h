typedef struct TypeBetween {
	uint8_t first;
	uint8_t second;
} type_between_t;


#define new_two_value(first, second) {first, second}
#define new_value(first) {first}

bool is_equals(uint8_t value, type_between_t type) {
	if (type.second == NULL) {
		if (value == type.first)
			return true;
	} else {
		if (value >= type.first && value <= type.second)
			return true;
	}
	return false;
}
