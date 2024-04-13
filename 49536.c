static void cast5_encrypt(struct crypto_tfm *tfm, u8 *outbuf, const u8 *inbuf)
{
	__cast5_encrypt(crypto_tfm_ctx(tfm), outbuf, inbuf);
}
