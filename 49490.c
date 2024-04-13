static int reset_prng_context(struct prng_context *ctx,
			      unsigned char *key, size_t klen,
			      unsigned char *V, unsigned char *DT)
{
	int ret;
	unsigned char *prng_key;

	spin_lock_bh(&ctx->prng_lock);
	ctx->flags |= PRNG_NEED_RESET;

	prng_key = (key != NULL) ? key : (unsigned char *)DEFAULT_PRNG_KEY;

	if (!key)
		klen = DEFAULT_PRNG_KSZ;

	if (V)
		memcpy(ctx->V, V, DEFAULT_BLK_SZ);
	else
		memcpy(ctx->V, DEFAULT_V_SEED, DEFAULT_BLK_SZ);

	if (DT)
		memcpy(ctx->DT, DT, DEFAULT_BLK_SZ);
	else
		memset(ctx->DT, 0, DEFAULT_BLK_SZ);

	memset(ctx->rand_data, 0, DEFAULT_BLK_SZ);
	memset(ctx->last_rand_data, 0, DEFAULT_BLK_SZ);

	ctx->rand_data_valid = DEFAULT_BLK_SZ;

	ret = crypto_cipher_setkey(ctx->tfm, prng_key, klen);
	if (ret) {
		dbgprint(KERN_CRIT "PRNG: setkey() failed flags=%x\n",
			crypto_cipher_get_flags(ctx->tfm));
		goto out;
	}

	ret = 0;
	ctx->flags &= ~PRNG_NEED_RESET;
out:
	spin_unlock_bh(&ctx->prng_lock);
	return ret;
}
