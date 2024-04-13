static void seed_encrypt(struct crypto_tfm *tfm, u8 *out, const u8 *in)
{
	const struct seed_ctx *ctx = crypto_tfm_ctx(tfm);
	const __be32 *src = (const __be32 *)in;
	__be32 *dst = (__be32 *)out;
	u32 x1, x2, x3, x4, t0, t1;
	const u32 *ks = ctx->keysched;

	x1 = be32_to_cpu(src[0]);
	x2 = be32_to_cpu(src[1]);
	x3 = be32_to_cpu(src[2]);
	x4 = be32_to_cpu(src[3]);

	OP(x1, x2, x3, x4, 0);
	OP(x3, x4, x1, x2, 2);
	OP(x1, x2, x3, x4, 4);
	OP(x3, x4, x1, x2, 6);
	OP(x1, x2, x3, x4, 8);
	OP(x3, x4, x1, x2, 10);
	OP(x1, x2, x3, x4, 12);
	OP(x3, x4, x1, x2, 14);
	OP(x1, x2, x3, x4, 16);
	OP(x3, x4, x1, x2, 18);
	OP(x1, x2, x3, x4, 20);
	OP(x3, x4, x1, x2, 22);
	OP(x1, x2, x3, x4, 24);
	OP(x3, x4, x1, x2, 26);
	OP(x1, x2, x3, x4, 28);
	OP(x3, x4, x1, x2, 30);

	dst[0] = cpu_to_be32(x3);
	dst[1] = cpu_to_be32(x4);
	dst[2] = cpu_to_be32(x1);
	dst[3] = cpu_to_be32(x2);
}
