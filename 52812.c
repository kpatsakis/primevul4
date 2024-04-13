static int send_bulk_static_data(struct snd_usb_midi_out_endpoint *ep,
				 const void *data, int len)
{
	int err = 0;
	void *buf = kmemdup(data, len, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;
	dump_urb("sending", buf, len);
	if (ep->urbs[0].urb)
		err = usb_bulk_msg(ep->umidi->dev, ep->urbs[0].urb->pipe,
				   buf, len, NULL, 250);
	kfree(buf);
	return err;
}
