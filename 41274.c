static inline void _tg3_flag_clear(enum TG3_FLAGS flag, unsigned long *bits)
{
	clear_bit(flag, bits);
}
