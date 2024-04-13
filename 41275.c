static inline void _tg3_flag_set(enum TG3_FLAGS flag, unsigned long *bits)
{
	set_bit(flag, bits);
}
