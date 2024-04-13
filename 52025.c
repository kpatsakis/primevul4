static int proc_reapurb_compat(struct usb_dev_state *ps, void __user *arg)
{
	struct async *as = reap_as(ps);

	if (as) {
		int retval;

		snoop(&ps->dev->dev, "reap %p\n", as->userurb);
		retval = processcompl_compat(as, (void __user * __user *)arg);
		free_async(as);
		return retval;
	}
	if (signal_pending(current))
		return -EINTR;
	return -ENODEV;
}
