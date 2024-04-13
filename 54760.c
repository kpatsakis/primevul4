static void complicated_callback(struct urb *urb)
{
	struct transfer_context	*ctx = urb->context;

	spin_lock(&ctx->lock);
	ctx->count--;

	ctx->packet_count += urb->number_of_packets;
	if (urb->error_count > 0)
		ctx->errors += urb->error_count;
	else if (urb->status != 0)
		ctx->errors += (ctx->is_iso ? urb->number_of_packets : 1);
	else if (urb->actual_length != urb->transfer_buffer_length)
		ctx->errors++;
	else if (check_guard_bytes(ctx->dev, urb) != 0)
		ctx->errors++;

	if (urb->status == 0 && ctx->count > (ctx->pending - 1)
			&& !ctx->submit_error) {
		int status = usb_submit_urb(urb, GFP_ATOMIC);
		switch (status) {
		case 0:
			goto done;
		default:
			dev_err(&ctx->dev->intf->dev,
					"resubmit err %d\n",
					status);
			/* FALLTHROUGH */
		case -ENODEV:			/* disconnected */
		case -ESHUTDOWN:		/* endpoint disabled */
			ctx->submit_error = 1;
			break;
		}
	}

	ctx->pending--;
	if (ctx->pending == 0) {
		if (ctx->errors)
			dev_err(&ctx->dev->intf->dev,
				"during the test, %lu errors out of %lu\n",
				ctx->errors, ctx->packet_count);
		complete(&ctx->done);
	}
done:
	spin_unlock(&ctx->lock);
}
