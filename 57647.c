IW_IMPL(void) iw_set_allow_opt(struct iw_context *ctx, int opt, int n)
{
	iw_byte v;
	v = n?1:0;

	switch(opt) {
	case IW_OPT_GRAYSCALE: ctx->opt_grayscale = v; break;
	case IW_OPT_PALETTE: ctx->opt_palette = v; break;
	case IW_OPT_16_TO_8: ctx->opt_16_to_8 = v; break;
	case IW_OPT_STRIP_ALPHA: ctx->opt_strip_alpha = v; break;
	case IW_OPT_BINARY_TRNS: ctx->opt_binary_trns = v; break;
	}
}
