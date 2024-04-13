IW_IMPL(void) iw_set_translate_fn(struct iw_context *ctx, iw_translatefn_type xlatefn)
{
	ctx->translate_fn = xlatefn;
}
