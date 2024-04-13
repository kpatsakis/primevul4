IW_IMPL(void) iw_set_warning_fn(struct iw_context *ctx, iw_warningfn_type warnfn)
{
	ctx->warning_fn = warnfn;
}
