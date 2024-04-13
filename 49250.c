static int camellia_setkey(struct crypto_tfm *tfm, const u8 *in_key,
			   unsigned int key_len)
{
	return __camellia_setkey(crypto_tfm_ctx(tfm), in_key, key_len,
				 &tfm->crt_flags);
}
