static void aes_decrypt(struct crypto_tfm *tfm, u8 *out, const u8 *in)
{
	const struct crypto_aes_ctx *ctx = crypto_tfm_ctx(tfm);
	const __le32 *src = (const __le32 *)in;
	__le32 *dst = (__le32 *)out;
	u32 b0[4], b1[4];
	const int key_len = ctx->key_length;
	const u32 *kp = ctx->key_dec + 4;

	b0[0] = le32_to_cpu(src[0]) ^  ctx->key_dec[0];
	b0[1] = le32_to_cpu(src[1]) ^  ctx->key_dec[1];
	b0[2] = le32_to_cpu(src[2]) ^  ctx->key_dec[2];
	b0[3] = le32_to_cpu(src[3]) ^  ctx->key_dec[3];

	if (key_len > 24) {
		i_nround(b1, b0, kp);
		i_nround(b0, b1, kp);
	}

	if (key_len > 16) {
		i_nround(b1, b0, kp);
		i_nround(b0, b1, kp);
	}

	i_nround(b1, b0, kp);
	i_nround(b0, b1, kp);
	i_nround(b1, b0, kp);
	i_nround(b0, b1, kp);
	i_nround(b1, b0, kp);
	i_nround(b0, b1, kp);
	i_nround(b1, b0, kp);
	i_nround(b0, b1, kp);
	i_nround(b1, b0, kp);
	i_lround(b0, b1, kp);

	dst[0] = cpu_to_le32(b0[0]);
	dst[1] = cpu_to_le32(b0[1]);
	dst[2] = cpu_to_le32(b0[2]);
	dst[3] = cpu_to_le32(b0[3]);
}
