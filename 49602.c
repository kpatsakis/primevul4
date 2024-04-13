static void fcrypt_decrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	const struct fcrypt_ctx *ctx = crypto_tfm_ctx(tfm);
	struct {
		__be32 l, r;
	} X;

	memcpy(&X, src, sizeof(X));

	F_ENCRYPT(X.l, X.r, ctx->sched[0xf]);
	F_ENCRYPT(X.r, X.l, ctx->sched[0xe]);
	F_ENCRYPT(X.l, X.r, ctx->sched[0xd]);
	F_ENCRYPT(X.r, X.l, ctx->sched[0xc]);
	F_ENCRYPT(X.l, X.r, ctx->sched[0xb]);
	F_ENCRYPT(X.r, X.l, ctx->sched[0xa]);
	F_ENCRYPT(X.l, X.r, ctx->sched[0x9]);
	F_ENCRYPT(X.r, X.l, ctx->sched[0x8]);
	F_ENCRYPT(X.l, X.r, ctx->sched[0x7]);
	F_ENCRYPT(X.r, X.l, ctx->sched[0x6]);
	F_ENCRYPT(X.l, X.r, ctx->sched[0x5]);
	F_ENCRYPT(X.r, X.l, ctx->sched[0x4]);
	F_ENCRYPT(X.l, X.r, ctx->sched[0x3]);
	F_ENCRYPT(X.r, X.l, ctx->sched[0x2]);
	F_ENCRYPT(X.l, X.r, ctx->sched[0x1]);
	F_ENCRYPT(X.r, X.l, ctx->sched[0x0]);

	memcpy(dst, &X, sizeof(X));
}
