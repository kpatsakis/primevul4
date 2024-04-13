static void catc_ctrl_run(struct catc *catc)
{
	struct ctrl_queue *q = catc->ctrl_queue + catc->ctrl_tail;
	struct usb_device *usbdev = catc->usbdev;
	struct urb *urb = catc->ctrl_urb;
	struct usb_ctrlrequest *dr = &catc->ctrl_dr;
	int status;

	dr->bRequest = q->request;
	dr->bRequestType = 0x40 | q->dir;
	dr->wValue = cpu_to_le16(q->value);
	dr->wIndex = cpu_to_le16(q->index);
	dr->wLength = cpu_to_le16(q->len);

        urb->pipe = q->dir ? usb_rcvctrlpipe(usbdev, 0) : usb_sndctrlpipe(usbdev, 0);
	urb->transfer_buffer_length = q->len;
	urb->transfer_buffer = catc->ctrl_buf;
	urb->setup_packet = (void *) dr;
	urb->dev = usbdev;

	if (!q->dir && q->buf && q->len)
		memcpy(catc->ctrl_buf, q->buf, q->len);

	if ((status = usb_submit_urb(catc->ctrl_urb, GFP_ATOMIC)))
		dev_err(&catc->usbdev->dev, "submit(ctrl_urb) status %d\n",
			status);
}
