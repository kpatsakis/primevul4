IW_IMPL(void) iw_set_input_bkgd_label_2(struct iw_context *ctx, const struct iw_color *clr)
{
	ctx->img1_bkgd_label_set = 1;
	ctx->img1_bkgd_label_inputcs = *clr;
}
