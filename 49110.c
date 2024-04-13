static int des3_setkey(struct crypto_tfm *tfm, const u8 *key,
		       unsigned int key_len)
{
	struct s390_des_ctx *ctx = crypto_tfm_ctx(tfm);
	u32 *flags = &tfm->crt_flags;

	if (!(crypto_memneq(key, &key[DES_KEY_SIZE], DES_KEY_SIZE) &&
	    crypto_memneq(&key[DES_KEY_SIZE], &key[DES_KEY_SIZE * 2],
			  DES_KEY_SIZE)) &&
	    (*flags & CRYPTO_TFM_REQ_WEAK_KEY)) {
		*flags |= CRYPTO_TFM_RES_WEAK_KEY;
		return -EINVAL;
	}
	memcpy(ctx->key, key, key_len);
	return 0;
}
