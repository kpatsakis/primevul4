IW_IMPL(void) iw_set_color_count(struct iw_context *ctx, int channeltype, int c)
{
	if(channeltype>=0 && channeltype<IW_NUM_CHANNELTYPES) {
		ctx->req.color_count[channeltype] = c;
	}

	switch(channeltype) {
	case IW_CHANNELTYPE_ALL:
		ctx->req.color_count[IW_CHANNELTYPE_ALPHA] = c;
	case IW_CHANNELTYPE_NONALPHA:
		ctx->req.color_count[IW_CHANNELTYPE_RED] = c;
		ctx->req.color_count[IW_CHANNELTYPE_GREEN] = c;
		ctx->req.color_count[IW_CHANNELTYPE_BLUE] = c;
		ctx->req.color_count[IW_CHANNELTYPE_GRAY] = c;
		break;
	}
}
