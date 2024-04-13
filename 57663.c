IW_IMPL(void) iw_set_max_malloc(struct iw_context *ctx, size_t n)
{
	ctx->max_malloc = n;
}
