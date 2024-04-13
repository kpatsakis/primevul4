static void ati_remote2_complete_mouse(struct urb *urb)
{
	struct ati_remote2 *ar2 = urb->context;
	int r;

	switch (urb->status) {
	case 0:
		usb_mark_last_busy(ar2->udev);
		ati_remote2_input_mouse(ar2);
		break;
	case -ENOENT:
	case -EILSEQ:
	case -ECONNRESET:
	case -ESHUTDOWN:
		dev_dbg(&ar2->intf[0]->dev,
			"%s(): urb status = %d\n", __func__, urb->status);
		return;
	default:
		usb_mark_last_busy(ar2->udev);
		dev_err(&ar2->intf[0]->dev,
			"%s(): urb status = %d\n", __func__, urb->status);
	}

	r = usb_submit_urb(urb, GFP_ATOMIC);
	if (r)
		dev_err(&ar2->intf[0]->dev,
			"%s(): usb_submit_urb() = %d\n", __func__, r);
}
