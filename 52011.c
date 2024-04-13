static int proc_claiminterface(struct usb_dev_state *ps, void __user *arg)
{
	unsigned int ifnum;

	if (get_user(ifnum, (unsigned int __user *)arg))
		return -EFAULT;
	return claimintf(ps, ifnum);
}
