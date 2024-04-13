static int proc_reapurb(struct usb_dev_state *ps, void __user *arg)
{
	struct async *as = reap_as(ps);

	if (as) {
		int retval;

		snoop(&ps->dev->dev, "reap %p\n", as->userurb);
		retval = processcompl(as, (void __user * __user *)arg);
		free_async(as);
		return retval;
	}
	if (signal_pending(current))
		return -EINTR;
	return -ENODEV;
}
