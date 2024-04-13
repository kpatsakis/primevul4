static void cast6_decrypt(struct crypto_tfm *tfm, u8 *outbuf, const u8 *inbuf)
{
	__cast6_decrypt(crypto_tfm_ctx(tfm), outbuf, inbuf);
}
