static void unlink1_callback(struct urb *urb)
{
	int	status = urb->status;

	/* we "know" -EPIPE (stall) never happens */
	if (!status)
		status = usb_submit_urb(urb, GFP_ATOMIC);
	if (status) {
		urb->status = status;
		complete(urb->context);
	}
}
