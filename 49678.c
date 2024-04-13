static void salsa20_ivsetup(struct salsa20_ctx *ctx, const u8 *iv)
{
	ctx->input[6] = U8TO32_LITTLE(iv + 0);
	ctx->input[7] = U8TO32_LITTLE(iv + 4);
	ctx->input[8] = 0;
	ctx->input[9] = 0;
}
