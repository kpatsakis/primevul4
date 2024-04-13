IW_IMPL(void) iw_set_error(struct iw_context *ctx, const char *s)
{
	char buf[IW_MSG_MAX];

	if(ctx->error_flag) return; // Only record the first error.
	iw_translate(ctx,IW_TRANSLATEFLAG_ERRORMSG,buf,sizeof(buf),s);
	iw_set_error_internal(ctx,buf);
}
