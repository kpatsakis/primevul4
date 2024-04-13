IW_IMPL(void) iw_reorient_image(struct iw_context *ctx, unsigned int x)
{
	static const unsigned int transpose_tbl[8] = { 4,6,5,7,0,2,1,3 };
	int tmpi;
	double tmpd;

	x = x & 0x07;

	if(x&0x04) {
		ctx->img1.orient_transform = transpose_tbl[ctx->img1.orient_transform];

		tmpi = ctx->img1.width;
		ctx->img1.width = ctx->img1.height;
		ctx->img1.height = tmpi;

		tmpd = ctx->img1.density_x;
		ctx->img1.density_x = ctx->img1.density_y;
		ctx->img1.density_y = tmpd;
	}

	ctx->img1.orient_transform ^= (x&0x03);
}
