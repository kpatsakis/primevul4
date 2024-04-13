static int proc_resetdevice(struct usb_dev_state *ps)
{
	struct usb_host_config *actconfig = ps->dev->actconfig;
	struct usb_interface *interface;
	int i, number;

	/* Don't allow a device reset if the process has dropped the
	 * privilege to do such things and any of the interfaces are
	 * currently claimed.
	 */
	if (ps->privileges_dropped && actconfig) {
		for (i = 0; i < actconfig->desc.bNumInterfaces; ++i) {
			interface = actconfig->interface[i];
			number = interface->cur_altsetting->desc.bInterfaceNumber;
			if (usb_interface_claimed(interface) &&
					!test_bit(number, &ps->ifclaimed)) {
				dev_warn(&ps->dev->dev,
					"usbfs: interface %d claimed by %s while '%s' resets device\n",
					number,	interface->dev.driver->name, current->comm);
				return -EACCES;
			}
		}
	}

	return usb_reset_device(ps->dev);
}
