static void oz_complete_urb(struct usb_hcd *hcd, struct urb *urb,
		int status)
{
	struct oz_hcd *ozhcd = oz_hcd_private(hcd);
	unsigned long irq_state;
	struct oz_urb_link *cancel_urbl;

	spin_lock_irqsave(&g_tasklet_lock, irq_state);
	usb_hcd_unlink_urb_from_ep(hcd, urb);
	/* Clear hcpriv which will prevent it being put in the cancel list
	 * in the event that an attempt is made to cancel it.
	 */
	urb->hcpriv = NULL;
	/* Walk the cancel list in case the urb is already sitting there.
	 * Since we process the cancel list in a tasklet rather than in
	 * the dequeue function this could happen.
	 */
	cancel_urbl = oz_uncancel_urb(ozhcd, urb);
	/* Note: we release lock but do not enable local irqs.
	 * It appears that usb_hcd_giveback_urb() expects irqs to be disabled,
	 * or at least other host controllers disable interrupts at this point
	 * so we do the same. We must, however, release the lock otherwise a
	 * deadlock will occur if an urb is submitted to our driver in the urb
	 * completion function. Because we disable interrupts it is possible
	 * that the urb_enqueue function can be called with them disabled.
	 */
	spin_unlock(&g_tasklet_lock);
	if (oz_forget_urb(urb)) {
		oz_dbg(ON, "ERROR Unknown URB %p\n", urb);
	} else {
		atomic_dec(&g_pending_urbs);
		usb_hcd_giveback_urb(hcd, urb, status);
	}
	spin_lock(&g_tasklet_lock);
	spin_unlock_irqrestore(&g_tasklet_lock, irq_state);
	oz_free_urb_link(cancel_urbl);
}
