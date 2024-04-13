static void xeta_decrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	u32 y, z, sum;
	struct tea_ctx *ctx = crypto_tfm_ctx(tfm);
	const __le32 *in = (const __le32 *)src;
	__le32 *out = (__le32 *)dst;

	y = le32_to_cpu(in[0]);
	z = le32_to_cpu(in[1]);

	sum = XTEA_DELTA * XTEA_ROUNDS;

	while (sum) {
		z -= (y << 4 ^ y >> 5) + (y ^ sum) + ctx->KEY[sum>>11 & 3];
		sum -= XTEA_DELTA;
		y -= (z << 4 ^ z >> 5) + (z ^ sum) + ctx->KEY[sum & 3];
	}
	
	out[0] = cpu_to_le32(y);
	out[1] = cpu_to_le32(z);
}
