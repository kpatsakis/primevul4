static void aiptek_disconnect(struct usb_interface *intf)
{
	struct aiptek *aiptek = usb_get_intfdata(intf);

	/* Disassociate driver's struct with usb interface
	 */
	usb_set_intfdata(intf, NULL);
	if (aiptek != NULL) {
		/* Free & unhook everything from the system.
		 */
		usb_kill_urb(aiptek->urb);
		input_unregister_device(aiptek->inputdev);
		sysfs_remove_group(&intf->dev.kobj, &aiptek_attribute_group);
		usb_free_urb(aiptek->urb);
		usb_free_coherent(interface_to_usbdev(intf),
				  AIPTEK_PACKET_LENGTH,
				  aiptek->data, aiptek->data_dma);
		kfree(aiptek);
	}
}
