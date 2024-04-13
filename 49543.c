static inline void W(u32 *key, unsigned int i)
{
	u32 I;
	key[6] ^= F1(key[7], Tr[i % 4][0], Tm[i][0]);
	key[5] ^= F2(key[6], Tr[i % 4][1], Tm[i][1]);
	key[4] ^= F3(key[5], Tr[i % 4][2], Tm[i][2]);
	key[3] ^= F1(key[4], Tr[i % 4][3], Tm[i][3]);
	key[2] ^= F2(key[3], Tr[i % 4][4], Tm[i][4]);
	key[1] ^= F3(key[2], Tr[i % 4][5], Tm[i][5]);
	key[0] ^= F1(key[1], Tr[i % 4][6], Tm[i][6]);
	key[7] ^= F2(key[0], Tr[i % 4][7], Tm[i][7]);
}
