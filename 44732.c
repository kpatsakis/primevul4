static inline int is_selected(int dor, int unit)
{
	return ((dor & (0x10 << unit)) && (dor & 3) == unit);
}
