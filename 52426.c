static int ims_pcu_start_io(struct ims_pcu *pcu)
{
	int error;

	error = usb_submit_urb(pcu->urb_ctrl, GFP_KERNEL);
	if (error) {
		dev_err(pcu->dev,
			"Failed to start control IO - usb_submit_urb failed with result: %d\n",
			error);
		return -EIO;
	}

	error = usb_submit_urb(pcu->urb_in, GFP_KERNEL);
	if (error) {
		dev_err(pcu->dev,
			"Failed to start IO - usb_submit_urb failed with result: %d\n",
			error);
		usb_kill_urb(pcu->urb_ctrl);
		return -EIO;
	}

	return 0;
}
