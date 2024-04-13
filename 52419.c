static int ims_pcu_resume(struct usb_interface *intf)
{
	struct ims_pcu *pcu = usb_get_intfdata(intf);
	struct usb_host_interface *alt = intf->cur_altsetting;
	int retval = 0;

	if (alt->desc.bInterfaceClass == USB_CLASS_COMM) {
		retval = ims_pcu_start_io(pcu);
		if (retval == 0)
			retval = ims_pcu_line_setup(pcu);
	}

	return retval;
}
