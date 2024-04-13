IW_IMPL(void) iw_warningf(struct iw_context *ctx, const char *fmt, ...)
{
	va_list ap;

	if(!ctx->warning_fn) return;
	va_start(ap, fmt);
	iw_warningv(ctx,fmt,ap);
	va_end(ap);
}
