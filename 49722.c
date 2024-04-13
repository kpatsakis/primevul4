static void tea_decrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	u32 y, z, n, sum;
	u32 k0, k1, k2, k3;
	struct tea_ctx *ctx = crypto_tfm_ctx(tfm);
	const __le32 *in = (const __le32 *)src;
	__le32 *out = (__le32 *)dst;

	y = le32_to_cpu(in[0]);
	z = le32_to_cpu(in[1]);

	k0 = ctx->KEY[0];
	k1 = ctx->KEY[1];
	k2 = ctx->KEY[2];
	k3 = ctx->KEY[3];

	sum = TEA_DELTA << 5;

	n = TEA_ROUNDS;

	while (n-- > 0) {
		z -= ((y << 4) + k2) ^ (y + sum) ^ ((y >> 5) + k3);
		y -= ((z << 4) + k0) ^ (z + sum) ^ ((z >> 5) + k1);
		sum -= TEA_DELTA;
	}
	
	out[0] = cpu_to_le32(y);
	out[1] = cpu_to_le32(z);
}
