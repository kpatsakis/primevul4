static int sha1_import(struct shash_desc *desc, const void *in)
{
	struct s390_sha_ctx *sctx = shash_desc_ctx(desc);
	const struct sha1_state *ictx = in;

	sctx->count = ictx->count;
	memcpy(sctx->state, ictx->state, sizeof(ictx->state));
	memcpy(sctx->buf, ictx->buffer, sizeof(ictx->buffer));
	sctx->func = KIMD_SHA_1;
	return 0;
}
