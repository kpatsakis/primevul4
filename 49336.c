static int crc32c_intel_setkey(struct crypto_shash *hash, const u8 *key,
			unsigned int keylen)
{
	u32 *mctx = crypto_shash_ctx(hash);

	if (keylen != sizeof(u32)) {
		crypto_shash_set_flags(hash, CRYPTO_TFM_RES_BAD_KEY_LEN);
		return -EINVAL;
	}
	*mctx = le32_to_cpup((__le32 *)key);
	return 0;
}
