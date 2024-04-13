static int proc_resetep(struct usb_dev_state *ps, void __user *arg)
{
	unsigned int ep;
	int ret;

	if (get_user(ep, (unsigned int __user *)arg))
		return -EFAULT;
	ret = findintfep(ps->dev, ep);
	if (ret < 0)
		return ret;
	ret = checkintf(ps, ret);
	if (ret)
		return ret;
	check_reset_of_active_ep(ps->dev, ep, "RESETEP");
	usb_reset_endpoint(ps->dev, ep);
	return 0;
}
