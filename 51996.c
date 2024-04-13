static void destroy_async(struct usb_dev_state *ps, struct list_head *list)
{
	struct urb *urb;
	struct async *as;
	unsigned long flags;

	spin_lock_irqsave(&ps->lock, flags);
	while (!list_empty(list)) {
		as = list_entry(list->next, struct async, asynclist);
		list_del_init(&as->asynclist);
		urb = as->urb;
		usb_get_urb(urb);

		/* drop the spinlock so the completion handler can run */
		spin_unlock_irqrestore(&ps->lock, flags);
		usb_kill_urb(urb);
		usb_put_urb(urb);
		spin_lock_irqsave(&ps->lock, flags);
	}
	spin_unlock_irqrestore(&ps->lock, flags);
}
