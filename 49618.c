static int lz4_decompress_crypto(struct crypto_tfm *tfm, const u8 *src,
			      unsigned int slen, u8 *dst, unsigned int *dlen)
{
	int err;
	size_t tmp_len = *dlen;
	size_t __slen = slen;

	err = lz4_decompress_unknownoutputsize(src, __slen, dst, &tmp_len);
	if (err < 0)
		return -EINVAL;

	*dlen = tmp_len;
	return err;
}
