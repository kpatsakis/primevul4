IW_IMPL(const char*) iw_get_option(struct iw_context *ctx, const char *name)
{
	int i;
	for(i=0; i<ctx->req.options_count; i++) {
		if(ctx->req.options[i].name && !strcmp(ctx->req.options[i].name, name)) {
			return ctx->req.options[i].val;
		}
	}
	return NULL;
}
