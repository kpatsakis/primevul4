static int null_compress(struct crypto_tfm *tfm, const u8 *src,
			 unsigned int slen, u8 *dst, unsigned int *dlen)
{
	if (slen > *dlen)
		return -EINVAL;
	memcpy(dst, src, slen);
	*dlen = slen;
	return 0;
}
