static struct urb *oz_find_urb_by_id(struct oz_port *port, int ep_ix,
		u8 req_id)
{
	struct oz_hcd *ozhcd = port->ozhcd;
	struct urb *urb = NULL;
	struct oz_urb_link *urbl;
	struct oz_endpoint *ep;

	spin_lock_bh(&ozhcd->hcd_lock);
	ep = port->out_ep[ep_ix];
	if (ep) {
		struct list_head *e;

		list_for_each(e, &ep->urb_list) {
			urbl = list_entry(e, struct oz_urb_link, link);
			if (urbl->req_id == req_id) {
				urb = urbl->urb;
				list_del_init(e);
				break;
			}
		}
	}
	spin_unlock_bh(&ozhcd->hcd_lock);
	/* If urb is non-zero then we we must have an urb link to delete.
	 */
	if (urb)
		oz_free_urb_link(urbl);
	return urb;
}
