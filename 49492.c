static void anubis_crypt(u32 roundKey[ANUBIS_MAX_ROUNDS + 1][4],
		u8 *ciphertext, const u8 *plaintext, const int R)
{
	const __be32 *src = (const __be32 *)plaintext;
	__be32 *dst = (__be32 *)ciphertext;
	int i, r;
	u32 state[4];
	u32 inter[4];

	/*
	 * map plaintext block to cipher state (mu)
	 * and add initial round key (sigma[K^0]):
	 */
	for (i = 0; i < 4; i++)
		state[i] = be32_to_cpu(src[i]) ^ roundKey[0][i];

	/*
	 * R - 1 full rounds:
	 */

	for (r = 1; r < R; r++) {
		inter[0] =
			T0[(state[0] >> 24)       ] ^
			T1[(state[1] >> 24)       ] ^
			T2[(state[2] >> 24)       ] ^
			T3[(state[3] >> 24)       ] ^
			roundKey[r][0];
		inter[1] =
			T0[(state[0] >> 16) & 0xff] ^
			T1[(state[1] >> 16) & 0xff] ^
			T2[(state[2] >> 16) & 0xff] ^
			T3[(state[3] >> 16) & 0xff] ^
			roundKey[r][1];
		inter[2] =
			T0[(state[0] >>  8) & 0xff] ^
			T1[(state[1] >>  8) & 0xff] ^
			T2[(state[2] >>  8) & 0xff] ^
			T3[(state[3] >>  8) & 0xff] ^
			roundKey[r][2];
		inter[3] =
			T0[(state[0]      ) & 0xff] ^
			T1[(state[1]      ) & 0xff] ^
			T2[(state[2]      ) & 0xff] ^
			T3[(state[3]      ) & 0xff] ^
			roundKey[r][3];
		state[0] = inter[0];
		state[1] = inter[1];
		state[2] = inter[2];
		state[3] = inter[3];
	}

	/*
	 * last round:
	 */

	inter[0] =
		(T0[(state[0] >> 24)       ] & 0xff000000U) ^
		(T1[(state[1] >> 24)       ] & 0x00ff0000U) ^
		(T2[(state[2] >> 24)       ] & 0x0000ff00U) ^
		(T3[(state[3] >> 24)       ] & 0x000000ffU) ^
		roundKey[R][0];
	inter[1] =
		(T0[(state[0] >> 16) & 0xff] & 0xff000000U) ^
		(T1[(state[1] >> 16) & 0xff] & 0x00ff0000U) ^
		(T2[(state[2] >> 16) & 0xff] & 0x0000ff00U) ^
		(T3[(state[3] >> 16) & 0xff] & 0x000000ffU) ^
		roundKey[R][1];
	inter[2] =
		(T0[(state[0] >>  8) & 0xff] & 0xff000000U) ^
		(T1[(state[1] >>  8) & 0xff] & 0x00ff0000U) ^
		(T2[(state[2] >>  8) & 0xff] & 0x0000ff00U) ^
		(T3[(state[3] >>  8) & 0xff] & 0x000000ffU) ^
		roundKey[R][2];
	inter[3] =
		(T0[(state[0]      ) & 0xff] & 0xff000000U) ^
		(T1[(state[1]      ) & 0xff] & 0x00ff0000U) ^
		(T2[(state[2]      ) & 0xff] & 0x0000ff00U) ^
		(T3[(state[3]      ) & 0xff] & 0x000000ffU) ^
		roundKey[R][3];

	/*
	 * map cipher state to ciphertext block (mu^{-1}):
	 */

	for (i = 0; i < 4; i++)
		dst[i] = cpu_to_be32(inter[i]);
}
