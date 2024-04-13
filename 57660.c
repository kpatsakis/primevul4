IW_IMPL(void) iw_set_input_crop(struct iw_context *ctx, int x, int y, int w, int h)
{
	ctx->input_start_x = x;
	ctx->input_start_y = y;
	ctx->input_w = w;
	ctx->input_h = h;
}
