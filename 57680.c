IW_IMPL(void) iw_warningv(struct iw_context *ctx, const char *fmt, va_list ap)
{
	char buf[IW_MSG_MAX];

	if(!ctx->warning_fn) return;
	iw_translatev(ctx,IW_TRANSLATEFLAG_WARNINGMSG,buf,sizeof(buf),fmt,ap);
	iw_warning_internal(ctx,buf);
}
