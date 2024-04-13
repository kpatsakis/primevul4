IW_IMPL(void) iw_set_input_image(struct iw_context *ctx, const struct iw_image *img)
{
	ctx->img1 = *img; // struct copy
}
