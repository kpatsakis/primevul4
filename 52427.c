static void ims_pcu_stop_io(struct ims_pcu *pcu)
{
	usb_kill_urb(pcu->urb_in);
	usb_kill_urb(pcu->urb_ctrl);
}
