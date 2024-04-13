static int ati_remote2_submit_urbs(struct ati_remote2 *ar2)
{
	int r;

	r = usb_submit_urb(ar2->urb[0], GFP_KERNEL);
	if (r) {
		dev_err(&ar2->intf[0]->dev,
			"%s(): usb_submit_urb() = %d\n", __func__, r);
		return r;
	}
	r = usb_submit_urb(ar2->urb[1], GFP_KERNEL);
	if (r) {
		usb_kill_urb(ar2->urb[0]);
		dev_err(&ar2->intf[1]->dev,
			"%s(): usb_submit_urb() = %d\n", __func__, r);
		return r;
	}

	return 0;
}
