IW_IMPL(void) iw_set_output_image_size(struct iw_context *ctx, double w, double h)
{
	ctx->req.out_true_width = w;
	if(ctx->req.out_true_width<0.01) ctx->req.out_true_width=0.01;
	ctx->req.out_true_height = h;
	if(ctx->req.out_true_height<0.01) ctx->req.out_true_height=0.01;
	ctx->req.out_true_valid = 1;
}
