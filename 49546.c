static void cast6_encrypt(struct crypto_tfm *tfm, u8 *outbuf, const u8 *inbuf)
{
	__cast6_encrypt(crypto_tfm_ctx(tfm), outbuf, inbuf);
}
