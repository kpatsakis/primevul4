IW_IMPL(void) iw_set_apply_bkgd(struct iw_context *ctx, double r, double g, double b)
{
	struct iw_color clr;
	clr.c[IW_CHANNELTYPE_RED]=r;
	clr.c[IW_CHANNELTYPE_GREEN]=g;
	clr.c[IW_CHANNELTYPE_BLUE]=b;
	clr.c[IW_CHANNELTYPE_ALPHA]=1.0;
	iw_set_apply_bkgd_2(ctx, &clr);
}
