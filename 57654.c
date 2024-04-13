static void iw_set_error_internal(struct iw_context *ctx, const char *s)
{
	if(ctx->error_flag) return; // Only record the first error.
	ctx->error_flag = 1;

	if(!ctx->error_msg) {
		ctx->error_msg=iw_malloc_ex(ctx,IW_MALLOCFLAG_NOERRORS,IW_MSG_MAX*sizeof(char));
		if(!ctx->error_msg) {
			return;
		}
	}

	iw_strlcpy(ctx->error_msg,s,IW_MSG_MAX);
}
