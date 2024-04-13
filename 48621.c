static int vmac_setkey(struct crypto_shash *parent,
		const u8 *key, unsigned int keylen)
{
	struct vmac_ctx_t *ctx = crypto_shash_ctx(parent);

	if (keylen != VMAC_KEY_LEN) {
		crypto_shash_set_flags(parent, CRYPTO_TFM_RES_BAD_KEY_LEN);
		return -EINVAL;
	}

	return vmac_set_key((u8 *)key, ctx);
}
