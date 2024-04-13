static int cprng_init(struct crypto_tfm *tfm)
{
	struct prng_context *ctx = crypto_tfm_ctx(tfm);

	spin_lock_init(&ctx->prng_lock);
	ctx->tfm = crypto_alloc_cipher("aes", 0, 0);
	if (IS_ERR(ctx->tfm)) {
		dbgprint(KERN_CRIT "Failed to alloc tfm for context %p\n",
				ctx);
		return PTR_ERR(ctx->tfm);
	}

	if (reset_prng_context(ctx, NULL, DEFAULT_PRNG_KSZ, NULL, NULL) < 0)
		return -EINVAL;

	/*
	 * after allocation, we should always force the user to reset
	 * so they don't inadvertently use the insecure default values
	 * without specifying them intentially
	 */
	ctx->flags |= PRNG_NEED_RESET;
	return 0;
}
