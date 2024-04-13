static void async_removepending(struct async *as)
{
	struct usb_dev_state *ps = as->ps;
	unsigned long flags;

	spin_lock_irqsave(&ps->lock, flags);
	list_del_init(&as->asynclist);
	spin_unlock_irqrestore(&ps->lock, flags);
}
