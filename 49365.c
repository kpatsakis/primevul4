static void ghash_flush(struct ghash_ctx *ctx, struct ghash_desc_ctx *dctx)
{
	u8 *dst = dctx->buffer;

	if (dctx->bytes) {
		u8 *tmp = dst + (GHASH_BLOCK_SIZE - dctx->bytes);

		while (dctx->bytes--)
			*tmp++ ^= 0;

		kernel_fpu_begin();
		clmul_ghash_mul(dst, &ctx->shash);
		kernel_fpu_end();
	}

	dctx->bytes = 0;
}
