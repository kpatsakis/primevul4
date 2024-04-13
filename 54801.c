static void uas_cmd_cmplt(struct urb *urb)
{
	if (urb->status)
		dev_err(&urb->dev->dev, "cmd cmplt err %d\n", urb->status);

	usb_free_urb(urb);
}
