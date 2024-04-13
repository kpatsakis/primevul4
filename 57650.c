IW_IMPL(void) iw_set_bkgd_checkerboard_origin(struct iw_context *ctx, int x, int y)
{
	ctx->bkgd_check_origin[IW_DIMENSION_H] = x;
	ctx->bkgd_check_origin[IW_DIMENSION_V] = y;
}
