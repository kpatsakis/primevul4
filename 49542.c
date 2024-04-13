static inline void QBAR(u32 *block, u8 *Kr, u32 *Km)
{
	u32 I;
	block[3] ^= F1(block[0], Kr[3], Km[3]);
	block[0] ^= F3(block[1], Kr[2], Km[2]);
	block[1] ^= F2(block[2], Kr[1], Km[1]);
	block[2] ^= F1(block[3], Kr[0], Km[0]);
}
