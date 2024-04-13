static int proc_reapurbnonblock(struct usb_dev_state *ps, void __user *arg)
{
	int retval;
	struct async *as;

	as = async_getcompleted(ps);
	if (as) {
		snoop(&ps->dev->dev, "reap %p\n", as->userurb);
		retval = processcompl(as, (void __user * __user *)arg);
		free_async(as);
	} else {
		retval = (connected(ps) ? -EAGAIN : -ENODEV);
	}
	return retval;
}
