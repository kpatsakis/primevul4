static int oz_hcd_urb_dequeue(struct usb_hcd *hcd, struct urb *urb, int status)
{
	struct oz_hcd *ozhcd = oz_hcd_private(hcd);
	struct oz_urb_link *urbl;
	int rc;
	unsigned long irq_state;

	oz_dbg(URB, "%s: (%p)\n",  __func__, urb);
	urbl = oz_alloc_urb_link();
	if (unlikely(urbl == NULL))
		return -ENOMEM;
	spin_lock_irqsave(&g_tasklet_lock, irq_state);
	/* The following function checks the urb is still in the queue
	 * maintained by the core and that the unlinked field is zero.
	 * If both are true the function sets the unlinked field and returns
	 * zero. Otherwise it returns an error.
	 */
	rc = usb_hcd_check_unlink_urb(hcd, urb, status);
	/* We have to check we haven't completed the urb or are about
	 * to complete it. When we do we set hcpriv to 0 so if this has
	 * already happened we don't put the urb in the cancel queue.
	 */
	if ((rc == 0) && urb->hcpriv) {
		urbl->urb = urb;
		urbl->port = (struct oz_port *)urb->hcpriv;
		urbl->ep_num = usb_pipeendpoint(urb->pipe);
		if (usb_pipein(urb->pipe))
			urbl->ep_num |= USB_DIR_IN;
		list_add_tail(&urbl->link, &ozhcd->urb_cancel_list);
		spin_unlock_irqrestore(&g_tasklet_lock, irq_state);
		tasklet_schedule(&g_urb_cancel_tasklet);
	} else {
		spin_unlock_irqrestore(&g_tasklet_lock, irq_state);
		oz_free_urb_link(urbl);
	}
	return rc;
}
