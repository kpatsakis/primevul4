static int oz_hcd_get_frame_number(struct usb_hcd *hcd)
{
	oz_dbg(ON, "oz_hcd_get_frame_number\n");
	return oz_usb_get_frame_number();
}
