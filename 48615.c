static int vmac_final(struct shash_desc *pdesc, u8 *out)
{
	struct crypto_shash *parent = pdesc->tfm;
	struct vmac_ctx_t *ctx = crypto_shash_ctx(parent);
	vmac_t mac;
	u8 nonce[16] = {};

	/* vmac() ends up accessing outside the array bounds that
	 * we specify.  In appears to access up to the next 2-word
	 * boundary.  We'll just be uber cautious and zero the
	 * unwritten bytes in the buffer.
	 */
	if (ctx->partial_size) {
		memset(ctx->partial + ctx->partial_size, 0,
			VMAC_NHBYTES - ctx->partial_size);
	}
	mac = vmac(ctx->partial, ctx->partial_size, nonce, NULL, ctx);
	memcpy(out, &mac, sizeof(vmac_t));
	memzero_explicit(&mac, sizeof(vmac_t));
	memset(&ctx->__vmac_ctx, 0, sizeof(struct vmac_ctx));
	ctx->partial_size = 0;
	return 0;
}
