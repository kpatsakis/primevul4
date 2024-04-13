static void ims_pcu_disconnect(struct usb_interface *intf)
{
	struct ims_pcu *pcu = usb_get_intfdata(intf);
	struct usb_host_interface *alt = intf->cur_altsetting;

	usb_set_intfdata(intf, NULL);

	/*
	 * See if we are dealing with control or data interface. The cleanup
	 * happens when we unbind primary (control) interface.
	 */
	if (alt->desc.bInterfaceClass != USB_CLASS_COMM)
		return;

	sysfs_remove_group(&intf->dev.kobj, &ims_pcu_attr_group);

	ims_pcu_stop_io(pcu);

	if (pcu->bootloader_mode)
		ims_pcu_destroy_bootloader_mode(pcu);
	else
		ims_pcu_destroy_application_mode(pcu);

	ims_pcu_buffers_free(pcu);
	kfree(pcu);
}
