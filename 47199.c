static struct oz_urb_link *oz_remove_urb(struct oz_endpoint *ep,
				struct urb *urb)
{
	struct oz_urb_link *urbl;

	if (unlikely(ep == NULL))
		return NULL;

	list_for_each_entry(urbl, &ep->urb_list, link) {
		if (urbl->urb == urb) {
			list_del_init(&urbl->link);
			if (usb_pipeisoc(urb->pipe)) {
				ep->credit -= urb->number_of_packets;
				if (ep->credit < 0)
					ep->credit = 0;
			}
			return urbl;
		}
	}
	return NULL;
}
