IW_IMPL(void) iw_set_output_bkgd_label(struct iw_context *ctx, double r, double g, double b)
{
	struct iw_color clr;
	clr.c[0] = r;
	clr.c[1] = g;
	clr.c[2] = b;
	clr.c[3] = 1.0;
	iw_set_output_bkgd_label_2(ctx, &clr);
}
