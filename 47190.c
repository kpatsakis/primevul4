static int oz_hcd_start(struct usb_hcd *hcd)
{
	hcd->power_budget = 200;
	hcd->state = HC_STATE_RUNNING;
	hcd->uses_new_polling = 1;
	return 0;
}
