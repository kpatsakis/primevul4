static int vmac_update(struct shash_desc *pdesc, const u8 *p,
		unsigned int len)
{
	struct crypto_shash *parent = pdesc->tfm;
	struct vmac_ctx_t *ctx = crypto_shash_ctx(parent);
	int expand;
	int min;

	expand = VMAC_NHBYTES - ctx->partial_size > 0 ?
			VMAC_NHBYTES - ctx->partial_size : 0;

	min = len < expand ? len : expand;

	memcpy(ctx->partial + ctx->partial_size, p, min);
	ctx->partial_size += min;

	if (len < expand)
		return 0;

	vhash_update(ctx->partial, VMAC_NHBYTES, &ctx->__vmac_ctx);
	ctx->partial_size = 0;

	len -= expand;
	p += expand;

	if (len % VMAC_NHBYTES) {
		memcpy(ctx->partial, p + len - (len % VMAC_NHBYTES),
			len % VMAC_NHBYTES);
		ctx->partial_size = len % VMAC_NHBYTES;
	}

	vhash_update(p, len - len % VMAC_NHBYTES, &ctx->__vmac_ctx);

	return 0;
}
