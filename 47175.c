static void oz_hcd_clear_orphanage(struct oz_hcd *ozhcd, int status)
{
	if (ozhcd) {
		struct oz_urb_link *urbl, *n;

		list_for_each_entry_safe(urbl, n, &ozhcd->orphanage, link) {
			list_del(&urbl->link);
			oz_complete_urb(ozhcd->hcd, urbl->urb, status);
			oz_free_urb_link(urbl);
		}
	}
}
