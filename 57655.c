IW_IMPL(void) iw_set_errorv(struct iw_context *ctx, const char *fmt, va_list ap)
{
	char buf[IW_MSG_MAX];

	if(ctx->error_flag) return; // Only record the first error.
	iw_translatev(ctx,IW_TRANSLATEFLAG_ERRORMSG,buf,sizeof(buf),fmt,ap);
	iw_set_error_internal(ctx,buf);
}
