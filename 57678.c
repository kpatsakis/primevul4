static void iw_warning_internal(struct iw_context *ctx, const char *s)
{
	if(!ctx->warning_fn) return;

	(*ctx->warning_fn)(ctx,s);
}
