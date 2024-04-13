static void oz_urb_cancel_tasklet(unsigned long unused)
{
	unsigned long irq_state;
	struct urb *urb;
	struct oz_urb_link *urbl, *n;
	struct oz_hcd *ozhcd = oz_hcd_claim();

	if (ozhcd == NULL)
		return;
	spin_lock_irqsave(&g_tasklet_lock, irq_state);
	list_for_each_entry_safe(urbl, n, &ozhcd->urb_cancel_list, link) {
		list_del_init(&urbl->link);
		spin_unlock_irqrestore(&g_tasklet_lock, irq_state);
		urb = urbl->urb;
		if (urb->unlinked)
			oz_urb_cancel(urbl->port, urbl->ep_num, urb);
		oz_free_urb_link(urbl);
		spin_lock_irqsave(&g_tasklet_lock, irq_state);
	}
	spin_unlock_irqrestore(&g_tasklet_lock, irq_state);
	oz_hcd_put(ozhcd);
}
