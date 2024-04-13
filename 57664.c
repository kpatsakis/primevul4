IW_IMPL(void) iw_set_option(struct iw_context *ctx, const char *name, const char *val)
{
#define IW_MAX_OPTIONS 32
	int i;

	if(val==NULL || val[0]=='\0') {
		val = "1";
	}

	if(!ctx->req.options) {
		ctx->req.options = iw_mallocz(ctx, IW_MAX_OPTIONS*sizeof(struct iw_option_struct));
		if(!ctx->req.options) return;
		ctx->req.options_numalloc = IW_MAX_OPTIONS;
		ctx->req.options_count = 0;
	}

	for(i=0; i<ctx->req.options_count; i++) {
		if(ctx->req.options[i].name && !strcmp(ctx->req.options[i].name, name)) {
			iw_free(ctx, ctx->req.options[i].val);
			ctx->req.options[i].val = iw_strdup(ctx, val);
			return;
		}
	}

	if(ctx->req.options_count>=IW_MAX_OPTIONS) return;
	ctx->req.options[ctx->req.options_count].name = iw_strdup(ctx, name);
	ctx->req.options[ctx->req.options_count].val = iw_strdup(ctx, val);
	ctx->req.options_count++;
}
