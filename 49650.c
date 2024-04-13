static int michael_final(struct shash_desc *desc, u8 *out)
{
	struct michael_mic_desc_ctx *mctx = shash_desc_ctx(desc);
	u8 *data = mctx->pending;
	__le32 *dst = (__le32 *)out;

	/* Last block and padding (0x5a, 4..7 x 0) */
	switch (mctx->pending_len) {
	case 0:
		mctx->l ^= 0x5a;
		break;
	case 1:
		mctx->l ^= data[0] | 0x5a00;
		break;
	case 2:
		mctx->l ^= data[0] | (data[1] << 8) | 0x5a0000;
		break;
	case 3:
		mctx->l ^= data[0] | (data[1] << 8) | (data[2] << 16) |
			0x5a000000;
		break;
	}
	michael_block(mctx->l, mctx->r);
	/* l ^= 0; */
	michael_block(mctx->l, mctx->r);

	dst[0] = cpu_to_le32(mctx->l);
	dst[1] = cpu_to_le32(mctx->r);

	return 0;
}
