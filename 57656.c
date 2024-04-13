IW_IMPL(void) iw_set_grayscale_weights(struct iw_context *ctx,
	double r, double g, double b)
{
	double tot;


	tot = r+g+b;
	if(tot==0.0) tot=1.0;
	ctx->grayscale_weight[0] = r/tot;
	ctx->grayscale_weight[1] = g/tot;
	ctx->grayscale_weight[2] = b/tot;
}
