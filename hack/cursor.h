#define B 0x00, 0x00, 0x00, 0xFF,
#define W 0xFF, 0xFF, 0xFF, 0xFF,
#define T 0x00, 0x00, 0x00, 0x00,

static unsigned char cursor[] = {
	B T T T T T T T T T T T
	B B T T T T T T T T T T
	B W B T T T T T T T T T
	B W W B T T T T T T T T
	B W W W B T T T T T T T
	B W W W W B T T T T T T
	B W W W W W B T T T T T
	B W W W W W W B T T T T
	B W W W W W W W B T T T
	B W W W W W W W W B T T
	B W W W W W W W W W B T
	B W W W W W W W W W W B
	B W W W W W W B B B B B
	B W W W B W W B T T T T
	B W W B T B W W B T T T
	B W B T T B W W B T T T
	B B T T T T B W W B T T
	T T T T T T B W W B T T
	T T T T T T T B B T T T
	T T T T T T T T T T T T
};

#undef B
#undef W
#undef T
