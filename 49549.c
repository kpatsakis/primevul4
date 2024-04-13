int cast6_setkey(struct crypto_tfm *tfm, const u8 *key, unsigned int keylen)
{
	return __cast6_setkey(crypto_tfm_ctx(tfm), key, keylen,
			      &tfm->crt_flags);
}
