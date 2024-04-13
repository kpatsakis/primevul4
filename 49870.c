static void hash_incrementlength(struct hash_req_ctx *ctx, u32 incr)
{
	ctx->state.length.low_word += incr;

	/* Check for wrap-around */
	if (ctx->state.length.low_word < incr)
		ctx->state.length.high_word++;
}
