IW_IMPL(void) iw_set_input_colorspace(struct iw_context *ctx, const struct iw_csdescr *csdescr)
{
	ctx->img1cs = *csdescr; // struct copy
	optimize_csdescr(&ctx->img1cs);
}
