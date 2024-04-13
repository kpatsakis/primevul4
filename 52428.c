static int ims_pcu_suspend(struct usb_interface *intf,
			   pm_message_t message)
{
	struct ims_pcu *pcu = usb_get_intfdata(intf);
	struct usb_host_interface *alt = intf->cur_altsetting;

	if (alt->desc.bInterfaceClass == USB_CLASS_COMM)
		ims_pcu_stop_io(pcu);

	return 0;
}
