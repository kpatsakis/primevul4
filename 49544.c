int __cast6_setkey(struct cast6_ctx *c, const u8 *in_key,
		   unsigned key_len, u32 *flags)
{
	int i;
	u32 key[8];
	__be32 p_key[8]; /* padded key */

	if (key_len % 4 != 0) {
		*flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
		return -EINVAL;
	}

	memset(p_key, 0, 32);
	memcpy(p_key, in_key, key_len);

	key[0] = be32_to_cpu(p_key[0]);		/* A */
	key[1] = be32_to_cpu(p_key[1]);		/* B */
	key[2] = be32_to_cpu(p_key[2]);		/* C */
	key[3] = be32_to_cpu(p_key[3]);		/* D */
	key[4] = be32_to_cpu(p_key[4]);		/* E */
	key[5] = be32_to_cpu(p_key[5]);		/* F */
	key[6] = be32_to_cpu(p_key[6]);		/* G */
	key[7] = be32_to_cpu(p_key[7]);		/* H */

	for (i = 0; i < 12; i++) {
		W(key, 2 * i);
		W(key, 2 * i + 1);

		c->Kr[i][0] = key[0] & 0x1f;
		c->Kr[i][1] = key[2] & 0x1f;
		c->Kr[i][2] = key[4] & 0x1f;
		c->Kr[i][3] = key[6] & 0x1f;

		c->Km[i][0] = key[7];
		c->Km[i][1] = key[5];
		c->Km[i][2] = key[3];
		c->Km[i][3] = key[1];
	}

	return 0;
}
