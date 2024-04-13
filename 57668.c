IW_IMPL(void) iw_set_output_depth(struct iw_context *ctx, int bps)
{
	ctx->req.output_depth = bps;
}
