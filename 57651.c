IW_IMPL(void) iw_set_channel_offset(struct iw_context *ctx, int channeltype, int dimension, double offs)
{
	if(channeltype<0 || channeltype>2) return;
	if(dimension<0 || dimension>1) dimension=0;
	ctx->resize_settings[dimension].channel_offset[channeltype] = offs;
}
