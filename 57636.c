IW_IMPL(double) iw_get_value_dbl(struct iw_context *ctx, int code)
{
	double ret = 0.0;

	switch(code) {
	case IW_VAL_TRANSLATE_X:
		ret = ctx->resize_settings[IW_DIMENSION_H].translate;
		break;
	case IW_VAL_TRANSLATE_Y:
		ret = ctx->resize_settings[IW_DIMENSION_V].translate;
		break;
	}

	return ret;
}
