static void unlink_queued_callback(struct urb *urb)
{
	int			status = urb->status;
	struct queued_ctx	*ctx = urb->context;

	if (ctx->status)
		goto done;
	if (urb == ctx->urbs[ctx->num - 4] || urb == ctx->urbs[ctx->num - 2]) {
		if (status == -ECONNRESET)
			goto done;
		/* What error should we report if the URB completed normally? */
	}
	if (status != 0)
		ctx->status = status;

 done:
	if (atomic_dec_and_test(&ctx->pending))
		complete(&ctx->complete);
}
