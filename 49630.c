static int lzo_decompress(struct crypto_tfm *tfm, const u8 *src,
			      unsigned int slen, u8 *dst, unsigned int *dlen)
{
	int err;
	size_t tmp_len = *dlen; /* size_t(ulong) <-> uint on 64 bit */

	err = lzo1x_decompress_safe(src, slen, dst, &tmp_len);

	if (err != LZO_E_OK)
		return -EINVAL;

	*dlen = tmp_len;
	return 0;

}
