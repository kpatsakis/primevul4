static inline u32 G(u32 x, u32 y, u32 z)
{
	return (x & y) | (x & z) | (y & z);
}
