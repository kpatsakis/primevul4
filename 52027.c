static int proc_reapurbnonblock_compat(struct usb_dev_state *ps, void __user *arg)
{
	int retval;
	struct async *as;

	as = async_getcompleted(ps);
	if (as) {
		snoop(&ps->dev->dev, "reap %p\n", as->userurb);
		retval = processcompl_compat(as, (void __user * __user *)arg);
		free_async(as);
	} else {
		retval = (connected(ps) ? -EAGAIN : -ENODEV);
	}
	return retval;
}
