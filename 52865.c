static int snd_usbmidi_submit_urb(struct urb *urb, gfp_t flags)
{
	int err = usb_submit_urb(urb, flags);
	if (err < 0 && err != -ENODEV)
		dev_err(&urb->dev->dev, "usb_submit_urb: %d\n", err);
	return err;
}
