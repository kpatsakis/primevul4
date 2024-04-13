static struct oz_urb_link *oz_uncancel_urb(struct oz_hcd *ozhcd,
		struct urb *urb)
{
	struct oz_urb_link *urbl;

	list_for_each_entry(urbl, &ozhcd->urb_cancel_list, link) {
		if (urb == urbl->urb) {
			list_del_init(&urbl->link);
			return urbl;
		}
	}
	return NULL;
}
