IW_IMPL(int) iw_check_image_dimensions(struct iw_context *ctx, int w, int h)
{
	if(w>ctx->max_width || h>ctx->max_height) {
		iw_set_errorf(ctx,"Image dimensions too large (%d\xc3\x97%d)",w,h);
		return 0;
	}

	if(w<1 || h<1) {
		iw_set_errorf(ctx,"Invalid image dimensions (%d\xc3\x97%d)",w,h);
		return 0;
	}

	return 1;
}
