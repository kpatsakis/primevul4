static int crypto_cmac_digest_init(struct shash_desc *pdesc)
{
	unsigned long alignmask = crypto_shash_alignmask(pdesc->tfm);
	struct cmac_desc_ctx *ctx = shash_desc_ctx(pdesc);
	int bs = crypto_shash_blocksize(pdesc->tfm);
	u8 *prev = PTR_ALIGN((void *)ctx->ctx, alignmask + 1) + bs;

	ctx->len = 0;
	memset(prev, 0, bs);

	return 0;
}
