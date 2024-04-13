static void ati_remote2_disconnect(struct usb_interface *interface)
{
	struct ati_remote2 *ar2;
	struct usb_host_interface *alt = interface->cur_altsetting;

	if (alt->desc.bInterfaceNumber)
		return;

	ar2 = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);

	input_unregister_device(ar2->idev);

	sysfs_remove_group(&ar2->udev->dev.kobj, &ati_remote2_attr_group);

	ati_remote2_urb_cleanup(ar2);

	usb_driver_release_interface(&ati_remote2_driver, ar2->intf[1]);

	kfree(ar2);
}
