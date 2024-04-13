void usbip_pad_iso(struct usbip_device *ud, struct urb *urb)
{
	int np = urb->number_of_packets;
	int i;
	int actualoffset = urb->actual_length;

	if (!usb_pipeisoc(urb->pipe))
		return;

	/* if no packets or length of data is 0, then nothing to unpack */
	if (np == 0 || urb->actual_length == 0)
		return;

	/*
	 * if actual_length is transfer_buffer_length then no padding is
	 * present.
	 */
	if (urb->actual_length == urb->transfer_buffer_length)
		return;

	/*
	 * loop over all packets from last to first (to prevent overwritting
	 * memory when padding) and move them into the proper place
	 */
	for (i = np-1; i > 0; i--) {
		actualoffset -= urb->iso_frame_desc[i].actual_length;
		memmove(urb->transfer_buffer + urb->iso_frame_desc[i].offset,
			urb->transfer_buffer + actualoffset,
			urb->iso_frame_desc[i].actual_length);
	}
}
