static int proc_unlinkurb(struct usb_dev_state *ps, void __user *arg)
{
	struct urb *urb;
	struct async *as;
	unsigned long flags;

	spin_lock_irqsave(&ps->lock, flags);
	as = async_getpending(ps, arg);
	if (!as) {
		spin_unlock_irqrestore(&ps->lock, flags);
		return -EINVAL;
	}

	urb = as->urb;
	usb_get_urb(urb);
	spin_unlock_irqrestore(&ps->lock, flags);

	usb_kill_urb(urb);
	usb_put_urb(urb);

	return 0;
}
