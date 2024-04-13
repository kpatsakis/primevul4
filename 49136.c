static int sha512_import(struct shash_desc *desc, const void *in)
{
	struct s390_sha_ctx *sctx = shash_desc_ctx(desc);
	const struct sha512_state *ictx = in;

	if (unlikely(ictx->count[1]))
		return -ERANGE;
	sctx->count = ictx->count[0];

	memcpy(sctx->state, ictx->state, sizeof(ictx->state));
	memcpy(sctx->buf, ictx->buf, sizeof(ictx->buf));
	sctx->func = KIMD_SHA_512;
	return 0;
}
