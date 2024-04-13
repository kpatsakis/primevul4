static void oz_urb_cancel(struct oz_port *port, u8 ep_num, struct urb *urb)
{
	struct oz_urb_link *urbl = NULL;
	struct list_head *e;
	struct oz_hcd *ozhcd;
	unsigned long irq_state;
	u8 ix;

	if (port == NULL) {
		oz_dbg(ON, "%s: ERROR: (%p) port is null\n", __func__, urb);
		return;
	}
	ozhcd = port->ozhcd;
	if (ozhcd == NULL) {
		oz_dbg(ON, "%s; ERROR: (%p) ozhcd is null\n", __func__, urb);
		return;
	}

	/* Look in the tasklet queue.
	 */
	spin_lock_irqsave(&g_tasklet_lock, irq_state);
	list_for_each(e, &ozhcd->urb_cancel_list) {
		urbl = list_entry(e, struct oz_urb_link, link);
		if (urb == urbl->urb) {
			list_del_init(e);
			spin_unlock_irqrestore(&g_tasklet_lock, irq_state);
			goto out2;
		}
	}
	spin_unlock_irqrestore(&g_tasklet_lock, irq_state);
	urbl = NULL;

	/* Look in the orphanage.
	 */
	spin_lock_irqsave(&ozhcd->hcd_lock, irq_state);
	list_for_each(e, &ozhcd->orphanage) {
		urbl = list_entry(e, struct oz_urb_link, link);
		if (urbl->urb == urb) {
			list_del(e);
			oz_dbg(ON, "Found urb in orphanage\n");
			goto out;
		}
	}
	ix = (ep_num & 0xf);
	urbl = NULL;
	if ((ep_num & USB_DIR_IN) && ix)
		urbl = oz_remove_urb(port->in_ep[ix], urb);
	else
		urbl = oz_remove_urb(port->out_ep[ix], urb);
out:
	spin_unlock_irqrestore(&ozhcd->hcd_lock, irq_state);
out2:
	if (urbl) {
		urb->actual_length = 0;
		oz_free_urb_link(urbl);
		oz_complete_urb(ozhcd->hcd, urb, -EPIPE);
	}
}
