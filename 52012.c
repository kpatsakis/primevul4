static int proc_clearhalt(struct usb_dev_state *ps, void __user *arg)
{
	unsigned int ep;
	int pipe;
	int ret;

	if (get_user(ep, (unsigned int __user *)arg))
		return -EFAULT;
	ret = findintfep(ps->dev, ep);
	if (ret < 0)
		return ret;
	ret = checkintf(ps, ret);
	if (ret)
		return ret;
	check_reset_of_active_ep(ps->dev, ep, "CLEAR_HALT");
	if (ep & USB_DIR_IN)
		pipe = usb_rcvbulkpipe(ps->dev, ep & 0x7f);
	else
		pipe = usb_sndbulkpipe(ps->dev, ep & 0x7f);

	return usb_clear_halt(ps->dev, pipe);
}
