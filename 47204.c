static void oz_urb_process_tasklet(unsigned long unused)
{
	unsigned long irq_state;
	struct urb *urb;
	struct oz_hcd *ozhcd = oz_hcd_claim();
	struct oz_urb_link *urbl, *n;
	int rc = 0;

	if (ozhcd == NULL)
		return;
	/* This is called from a tasklet so is in softirq context but the urb
	 * list is filled from any context so we need to lock
	 * appropriately while removing urbs.
	 */
	spin_lock_irqsave(&g_tasklet_lock, irq_state);
	list_for_each_entry_safe(urbl, n, &ozhcd->urb_pending_list, link) {
		list_del_init(&urbl->link);
		spin_unlock_irqrestore(&g_tasklet_lock, irq_state);
		urb = urbl->urb;
		oz_free_urb_link(urbl);
		rc = oz_urb_process(ozhcd, urb);
		if (rc)
			oz_complete_urb(ozhcd->hcd, urb, rc);
		spin_lock_irqsave(&g_tasklet_lock, irq_state);
	}
	spin_unlock_irqrestore(&g_tasklet_lock, irq_state);
	oz_hcd_put(ozhcd);
}
