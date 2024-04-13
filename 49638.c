static inline u32 lshift(u32 x, unsigned int s)
{
	x &= 0xFFFFFFFF;
	return ((x << s) & 0xFFFFFFFF) | (x >> (32 - s));
}
