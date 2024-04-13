IW_IMPL(void) iw_translatev(struct iw_context *ctx, unsigned int flags,
	char *dst, size_t dstlen, const char *fmt, va_list ap)
{
	char buf1[IW_MSG_MAX];
	char buf2[IW_MSG_MAX];

	if(!ctx || !ctx->translate_fn) {
		iw_vsnprintf(dst,dstlen,fmt,ap);
		return;
	}

	iw_translate(ctx,IW_TRANSLATEFLAG_FORMAT|flags,buf1,sizeof(buf1),fmt);
	iw_vsnprintf(buf2,sizeof(buf2),buf1,ap);
	iw_translate(ctx,IW_TRANSLATEFLAG_POSTFORMAT|flags,dst,dstlen,buf2);
}
