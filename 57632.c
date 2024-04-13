IW_IMPL(void) iw_get_output_colorspace(struct iw_context *ctx, struct iw_csdescr *csdescr)
{
	*csdescr = ctx->img2cs; // struct copy
}
