static void choose_devnum(struct usb_device *udev)
{
	int		devnum;
	struct usb_bus	*bus = udev->bus;

	/* be safe when more hub events are proceed in parallel */
	mutex_lock(&bus->usb_address0_mutex);
	if (udev->wusb) {
		devnum = udev->portnum + 1;
		BUG_ON(test_bit(devnum, bus->devmap.devicemap));
	} else {
		/* Try to allocate the next devnum beginning at
		 * bus->devnum_next. */
		devnum = find_next_zero_bit(bus->devmap.devicemap, 128,
					    bus->devnum_next);
		if (devnum >= 128)
			devnum = find_next_zero_bit(bus->devmap.devicemap,
						    128, 1);
		bus->devnum_next = (devnum >= 127 ? 1 : devnum + 1);
	}
	if (devnum < 128) {
		set_bit(devnum, bus->devmap.devicemap);
		udev->devnum = devnum;
	}
	mutex_unlock(&bus->usb_address0_mutex);
}
