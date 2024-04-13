IW_IMPL(void) iw_warning(struct iw_context *ctx, const char *s)
{
	char buf[IW_MSG_MAX];

	if(!ctx->warning_fn) return;
	iw_translate(ctx,IW_TRANSLATEFLAG_WARNINGMSG,buf,sizeof(buf),s);
	iw_warning_internal(ctx,buf);
}
