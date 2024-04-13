static inline u32 F(u32 x, u32 y, u32 z)
{
	return (x & y) | ((~x) & z);
}
