IW_IMPL(void) iw_destroy_context(struct iw_context *ctx)
{
	int i;
	if(!ctx) return;
	if(ctx->req.options) {
		for(i=0; i<=ctx->req.options_count; i++) {
			iw_free(ctx, ctx->req.options[i].name);
			iw_free(ctx, ctx->req.options[i].val);
		}
		iw_free(ctx, ctx->req.options);
	}
	if(ctx->img1.pixels) iw_free(ctx,ctx->img1.pixels);
	if(ctx->img2.pixels) iw_free(ctx,ctx->img2.pixels);
	if(ctx->error_msg) iw_free(ctx,ctx->error_msg);
	if(ctx->optctx.tmp_pixels) iw_free(ctx,ctx->optctx.tmp_pixels);
	if(ctx->optctx.palette) iw_free(ctx,ctx->optctx.palette);
	if(ctx->input_color_corr_table) iw_free(ctx,ctx->input_color_corr_table);
	if(ctx->output_rev_color_corr_table) iw_free(ctx,ctx->output_rev_color_corr_table);
	if(ctx->nearest_color_table) iw_free(ctx,ctx->nearest_color_table);
	if(ctx->prng) iwpvt_prng_destroy(ctx,ctx->prng);
	iw_free(ctx,ctx);
}
