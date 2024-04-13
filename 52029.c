static int proc_releaseinterface(struct usb_dev_state *ps, void __user *arg)
{
	unsigned int ifnum;
	int ret;

	if (get_user(ifnum, (unsigned int __user *)arg))
		return -EFAULT;
	ret = releaseintf(ps, ifnum);
	if (ret < 0)
		return ret;
	destroy_async_on_interface(ps, ifnum);
	return 0;
}
