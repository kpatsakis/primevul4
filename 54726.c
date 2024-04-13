static int hid_post_reset(struct usb_interface *intf)
{
	struct usb_device *dev = interface_to_usbdev (intf);
	struct hid_device *hid = usb_get_intfdata(intf);
	struct usbhid_device *usbhid = hid->driver_data;
	struct usb_host_interface *interface = intf->cur_altsetting;
	int status;
	char *rdesc;

	/* Fetch and examine the HID report descriptor. If this
	 * has changed, then rebind. Since usbcore's check of the
	 * configuration descriptors passed, we already know that
	 * the size of the HID report descriptor has not changed.
	 */
	rdesc = kmalloc(hid->dev_rsize, GFP_KERNEL);
	if (!rdesc)
		return -ENOMEM;

	status = hid_get_class_descriptor(dev,
				interface->desc.bInterfaceNumber,
				HID_DT_REPORT, rdesc, hid->dev_rsize);
	if (status < 0) {
		dbg_hid("reading report descriptor failed (post_reset)\n");
		kfree(rdesc);
		return status;
	}
	status = memcmp(rdesc, hid->dev_rdesc, hid->dev_rsize);
	kfree(rdesc);
	if (status != 0) {
		dbg_hid("report descriptor changed\n");
		return -EPERM;
	}

	/* No need to do another reset or clear a halted endpoint */
	spin_lock_irq(&usbhid->lock);
	clear_bit(HID_RESET_PENDING, &usbhid->iofl);
	clear_bit(HID_CLEAR_HALT, &usbhid->iofl);
	spin_unlock_irq(&usbhid->lock);
	hid_set_idle(dev, intf->cur_altsetting->desc.bInterfaceNumber, 0, 0);

	hid_restart_io(hid);

	return 0;
}
