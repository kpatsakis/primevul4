IW_IMPL(void) iw_set_bkgd_checkerboard_2(struct iw_context *ctx, int checkersize,
	const struct iw_color *clr)
{
	ctx->req.bkgd_checkerboard=1;
	ctx->bkgd_check_size=checkersize;
	ctx->req.bkgd2 = *clr;
}
