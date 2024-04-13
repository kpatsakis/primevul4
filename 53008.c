static void powermate_config_complete(struct urb *urb)
{
	struct powermate_device *pm = urb->context;
	unsigned long flags;

	if (urb->status)
		printk(KERN_ERR "powermate: config urb returned %d\n", urb->status);

	spin_lock_irqsave(&pm->lock, flags);
	powermate_sync_state(pm);
	spin_unlock_irqrestore(&pm->lock, flags);
}
