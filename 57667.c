 IW_IMPL(void) iw_set_output_density(struct iw_context *ctx,
   double x, double y, int code)
{
	ctx->img2.density_code = code;
	ctx->img2.density_x = x;
	ctx->img2.density_y = y;
}
