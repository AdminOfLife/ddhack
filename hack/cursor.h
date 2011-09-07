#define B 0x00000000,
#define W 0x00FFFFFF,
#define T 0xFF000000,

static unsigned int cursor[] = {
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
};

#undef B
#undef W
#undef T
