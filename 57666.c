IW_IMPL(void) iw_set_output_colorspace(struct iw_context *ctx, const struct iw_csdescr *csdescr)
{
	ctx->req.output_cs = *csdescr; // struct copy
	optimize_csdescr(&ctx->req.output_cs);
	ctx->req.output_cs_valid = 1;
}
