IW_IMPL(void) iw_translatef(struct iw_context *ctx, unsigned int flags,
	char *dst, size_t dstlen, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	iw_translatev(ctx,flags,dst,dstlen,fmt,ap);
	va_end(ap);
}
