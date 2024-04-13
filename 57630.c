IW_IMPL(const char*) iw_get_errormsg(struct iw_context *ctx, char *buf, int buflen)
{
	if(ctx->error_msg) {
		iw_strlcpy(buf,ctx->error_msg,buflen);
	}
	else {
		iw_translate(ctx,IW_TRANSLATEFLAG_ERRORMSG,buf,buflen,"Error message not available");
	}

	return buf;
}
