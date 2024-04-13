static int oz_hcd_urb_enqueue(struct usb_hcd *hcd, struct urb *urb,
				gfp_t mem_flags)
{
	struct oz_hcd *ozhcd = oz_hcd_private(hcd);
	int rc;
	int port_ix;
	struct oz_port *port;
	unsigned long irq_state;
	struct oz_urb_link *urbl;

	oz_dbg(URB, "%s: (%p)\n",  __func__, urb);
	if (unlikely(ozhcd == NULL)) {
		oz_dbg(URB, "Refused urb(%p) not ozhcd\n", urb);
		return -EPIPE;
	}
	if (unlikely(hcd->state != HC_STATE_RUNNING)) {
		oz_dbg(URB, "Refused urb(%p) not running\n", urb);
		return -EPIPE;
	}
	port_ix = oz_get_port_from_addr(ozhcd, urb->dev->devnum);
	if (port_ix < 0)
		return -EPIPE;
	port =  &ozhcd->ports[port_ix];
	if (port == NULL)
		return -EPIPE;
	if (!(port->flags & OZ_PORT_F_PRESENT) ||
				(port->flags & OZ_PORT_F_CHANGED)) {
		oz_dbg(ON, "Refusing URB port_ix = %d devnum = %d\n",
		       port_ix, urb->dev->devnum);
		return -EPIPE;
	}
	urb->hcpriv = port;
	/* Put request in queue for processing by tasklet.
	 */
	urbl = oz_alloc_urb_link();
	if (unlikely(urbl == NULL))
		return -ENOMEM;
	urbl->urb = urb;
	spin_lock_irqsave(&g_tasklet_lock, irq_state);
	rc = usb_hcd_link_urb_to_ep(hcd, urb);
	if (unlikely(rc)) {
		spin_unlock_irqrestore(&g_tasklet_lock, irq_state);
		oz_free_urb_link(urbl);
		return rc;
	}
	list_add_tail(&urbl->link, &ozhcd->urb_pending_list);
	spin_unlock_irqrestore(&g_tasklet_lock, irq_state);
	tasklet_schedule(&g_urb_process_tasklet);
	atomic_inc(&g_pending_urbs);
	return 0;
}
