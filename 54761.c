static void ctrl_complete(struct urb *urb)
{
	struct ctrl_ctx		*ctx = urb->context;
	struct usb_ctrlrequest	*reqp;
	struct subcase		*subcase;
	int			status = urb->status;

	reqp = (struct usb_ctrlrequest *)urb->setup_packet;
	subcase = container_of(reqp, struct subcase, setup);

	spin_lock(&ctx->lock);
	ctx->count--;
	ctx->pending--;

	/* queue must transfer and complete in fifo order, unless
	 * usb_unlink_urb() is used to unlink something not at the
	 * physical queue head (not tested).
	 */
	if (subcase->number > 0) {
		if ((subcase->number - ctx->last) != 1) {
			ERROR(ctx->dev,
				"subcase %d completed out of order, last %d\n",
				subcase->number, ctx->last);
			status = -EDOM;
			ctx->last = subcase->number;
			goto error;
		}
	}
	ctx->last = subcase->number;

	/* succeed or fault in only one way? */
	if (status == subcase->expected)
		status = 0;

	/* async unlink for cleanup? */
	else if (status != -ECONNRESET) {

		/* some faults are allowed, not required */
		if (subcase->expected > 0 && (
			  ((status == -subcase->expected	/* happened */
			   || status == 0))))			/* didn't */
			status = 0;
		/* sometimes more than one fault is allowed */
		else if (subcase->number == 12 && status == -EPIPE)
			status = 0;
		else
			ERROR(ctx->dev, "subtest %d error, status %d\n",
					subcase->number, status);
	}

	/* unexpected status codes mean errors; ideally, in hardware */
	if (status) {
error:
		if (ctx->status == 0) {
			int		i;

			ctx->status = status;
			ERROR(ctx->dev, "control queue %02x.%02x, err %d, "
					"%d left, subcase %d, len %d/%d\n",
					reqp->bRequestType, reqp->bRequest,
					status, ctx->count, subcase->number,
					urb->actual_length,
					urb->transfer_buffer_length);

			/* FIXME this "unlink everything" exit route should
			 * be a separate test case.
			 */

			/* unlink whatever's still pending */
			for (i = 1; i < ctx->param->sglen; i++) {
				struct urb *u = ctx->urb[
							(i + subcase->number)
							% ctx->param->sglen];

				if (u == urb || !u->dev)
					continue;
				spin_unlock(&ctx->lock);
				status = usb_unlink_urb(u);
				spin_lock(&ctx->lock);
				switch (status) {
				case -EINPROGRESS:
				case -EBUSY:
				case -EIDRM:
					continue;
				default:
					ERROR(ctx->dev, "urb unlink --> %d\n",
							status);
				}
			}
			status = ctx->status;
		}
	}

	/* resubmit if we need to, else mark this as done */
	if ((status == 0) && (ctx->pending < ctx->count)) {
		status = usb_submit_urb(urb, GFP_ATOMIC);
		if (status != 0) {
			ERROR(ctx->dev,
				"can't resubmit ctrl %02x.%02x, err %d\n",
				reqp->bRequestType, reqp->bRequest, status);
			urb->dev = NULL;
		} else
			ctx->pending++;
	} else
		urb->dev = NULL;

	/* signal completion when nothing's queued */
	if (ctx->pending == 0)
		complete(&ctx->complete);
	spin_unlock(&ctx->lock);
}
