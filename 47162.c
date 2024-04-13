static int oz_dequeue_ep_urb(struct oz_port *port, u8 ep_addr, int in_dir,
			struct urb *urb)
{
	struct oz_urb_link *urbl = NULL;
	struct oz_endpoint *ep;

	spin_lock_bh(&port->ozhcd->hcd_lock);
	if (in_dir)
		ep = port->in_ep[ep_addr];
	else
		ep = port->out_ep[ep_addr];
	if (ep) {
		struct list_head *e;

		list_for_each(e, &ep->urb_list) {
			urbl = list_entry(e, struct oz_urb_link, link);
			if (urbl->urb == urb) {
				list_del_init(e);
				break;
			}
			urbl = NULL;
		}
	}
	spin_unlock_bh(&port->ozhcd->hcd_lock);
	oz_free_urb_link(urbl);
	return urbl ? 0 : -EIDRM;
}
