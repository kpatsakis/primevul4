static int seed_set_key(struct crypto_tfm *tfm, const u8 *in_key,
		        unsigned int key_len)
{
	struct seed_ctx *ctx = crypto_tfm_ctx(tfm);
	u32 *keyout = ctx->keysched;
	const __be32 *key = (const __be32 *)in_key;
	u32 i, t0, t1, x1, x2, x3, x4;

	x1 = be32_to_cpu(key[0]);
	x2 = be32_to_cpu(key[1]);
	x3 = be32_to_cpu(key[2]);
	x4 = be32_to_cpu(key[3]);

	for (i = 0; i < SEED_NUM_KCONSTANTS; i++) {
		t0 = x1 + x3 - KC[i];
		t1 = x2 + KC[i] - x4;
		*(keyout++) = SS0[byte(t0, 0)] ^ SS1[byte(t0, 1)] ^
				SS2[byte(t0, 2)] ^ SS3[byte(t0, 3)];
		*(keyout++) = SS0[byte(t1, 0)] ^ SS1[byte(t1, 1)] ^
				SS2[byte(t1, 2)] ^ SS3[byte(t1, 3)];

		if (i % 2 == 0) {
			t0 = x1;
			x1 = (x1 >> 8) ^ (x2 << 24);
			x2 = (x2 >> 8) ^ (t0 << 24);
		} else {
			t0 = x3;
			x3 = (x3 << 8) ^ (x4 >> 24);
			x4 = (x4 << 8) ^ (t0 >> 24);
		}
	}

	return 0;
}
