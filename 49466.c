static int nx842_crypto_compress(struct crypto_tfm *tfm, const u8 *src,
			    unsigned int slen, u8 *dst, unsigned int *dlen)
{
	struct nx842_ctx *ctx = crypto_tfm_ctx(tfm);
	struct nx842_crypto_header *hdr;
	unsigned int tmp_len = *dlen;
	size_t lzodlen; /* needed for lzo */
	int err;

	*dlen = 0;
	hdr = (struct nx842_crypto_header *)dst;
	hdr->sentinel = NX842_SENTINEL; /* debug */
	dst += sizeof(struct nx842_crypto_header);
	tmp_len -= sizeof(struct nx842_crypto_header);
	lzodlen = tmp_len;

	if (likely(!nx842_uselzo)) {
		err = nx842_compress(src, slen, dst, &tmp_len, ctx->nx842_wmem);

		if (likely(!err)) {
			hdr->type = NX842_CRYPTO_TYPE_842;
			*dlen = tmp_len + sizeof(struct nx842_crypto_header);
			return 0;
		}

		/* hardware failed */
		nx842_uselzo = 1;

		/* set timer to check for hardware again in 1 second */
		mod_timer(&failover_timer, jiffies + msecs_to_jiffies(1000));
	}

	/* no hardware, use lzo */
	err = lzo1x_1_compress(src, slen, dst, &lzodlen, ctx->nx842_wmem);
	if (err != LZO_E_OK)
		return -EINVAL;

	hdr->type = NX842_CRYPTO_TYPE_LZO;
	*dlen = lzodlen + sizeof(struct nx842_crypto_header);
	return 0;
}
