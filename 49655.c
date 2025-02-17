static int michael_update(struct shash_desc *desc, const u8 *data,
			   unsigned int len)
{
	struct michael_mic_desc_ctx *mctx = shash_desc_ctx(desc);
	const __le32 *src;

	if (mctx->pending_len) {
		int flen = 4 - mctx->pending_len;
		if (flen > len)
			flen = len;
		memcpy(&mctx->pending[mctx->pending_len], data, flen);
		mctx->pending_len += flen;
		data += flen;
		len -= flen;

		if (mctx->pending_len < 4)
			return 0;

		src = (const __le32 *)mctx->pending;
		mctx->l ^= le32_to_cpup(src);
		michael_block(mctx->l, mctx->r);
		mctx->pending_len = 0;
	}

	src = (const __le32 *)data;

	while (len >= 4) {
		mctx->l ^= le32_to_cpup(src++);
		michael_block(mctx->l, mctx->r);
		len -= 4;
	}

	if (len > 0) {
		mctx->pending_len = len;
		memcpy(mctx->pending, src, len);
	}

	return 0;
}
