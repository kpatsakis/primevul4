static int nx842_crypto_decompress(struct crypto_tfm *tfm, const u8 *src,
			      unsigned int slen, u8 *dst, unsigned int *dlen)
{
	struct nx842_ctx *ctx = crypto_tfm_ctx(tfm);
	struct nx842_crypto_header *hdr;
	unsigned int tmp_len = *dlen;
	size_t lzodlen; /* needed for lzo */
	int err;

	*dlen = 0;
	hdr = (struct nx842_crypto_header *)src;

	if (unlikely(hdr->sentinel != NX842_SENTINEL))
		return -EINVAL;

	src += sizeof(struct nx842_crypto_header);
	slen -= sizeof(struct nx842_crypto_header);

	if (likely(hdr->type == NX842_CRYPTO_TYPE_842)) {
		err = nx842_decompress(src, slen, dst, &tmp_len,
			ctx->nx842_wmem);
		if (err)
			return -EINVAL;
		*dlen = tmp_len;
	} else if (hdr->type == NX842_CRYPTO_TYPE_LZO) {
		lzodlen = tmp_len;
		err = lzo1x_decompress_safe(src, slen, dst, &lzodlen);
		if (err != LZO_E_OK)
			return -EINVAL;
		*dlen = lzodlen;
	} else
		return -EINVAL;

	return 0;
}
