static inline int _tg3_flag(enum TG3_FLAGS flag, unsigned long *bits)
{
	return test_bit(flag, bits);
}
