IW_IMPL(void) iw_set_input_max_color_code(struct iw_context *ctx, int input_channel, int c)
{
	if(input_channel>=0 && input_channel<IW_CI_COUNT) {
		ctx->img1_ci[input_channel].maxcolorcode_int = c;
	}
}
