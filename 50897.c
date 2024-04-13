int hid_connect(struct hid_device *hdev, unsigned int connect_mask)
{
	static const char *types[] = { "Device", "Pointer", "Mouse", "Device",
		"Joystick", "Gamepad", "Keyboard", "Keypad",
		"Multi-Axis Controller"
	};
	const char *type, *bus;
	char buf[64] = "";
	unsigned int i;
	int len;
	int ret;

	if (hdev->quirks & HID_QUIRK_HIDDEV_FORCE)
		connect_mask |= (HID_CONNECT_HIDDEV_FORCE | HID_CONNECT_HIDDEV);
	if (hdev->quirks & HID_QUIRK_HIDINPUT_FORCE)
		connect_mask |= HID_CONNECT_HIDINPUT_FORCE;
	if (hdev->bus != BUS_USB)
		connect_mask &= ~HID_CONNECT_HIDDEV;
	if (hid_hiddev(hdev))
		connect_mask |= HID_CONNECT_HIDDEV_FORCE;

	if ((connect_mask & HID_CONNECT_HIDINPUT) && !hidinput_connect(hdev,
				connect_mask & HID_CONNECT_HIDINPUT_FORCE))
		hdev->claimed |= HID_CLAIMED_INPUT;

	if ((connect_mask & HID_CONNECT_HIDDEV) && hdev->hiddev_connect &&
			!hdev->hiddev_connect(hdev,
				connect_mask & HID_CONNECT_HIDDEV_FORCE))
		hdev->claimed |= HID_CLAIMED_HIDDEV;
	if ((connect_mask & HID_CONNECT_HIDRAW) && !hidraw_connect(hdev))
		hdev->claimed |= HID_CLAIMED_HIDRAW;

	if (connect_mask & HID_CONNECT_DRIVER)
		hdev->claimed |= HID_CLAIMED_DRIVER;

	/* Drivers with the ->raw_event callback set are not required to connect
	 * to any other listener. */
	if (!hdev->claimed && !hdev->driver->raw_event) {
		hid_err(hdev, "device has no listeners, quitting\n");
		return -ENODEV;
	}

	if ((hdev->claimed & HID_CLAIMED_INPUT) &&
			(connect_mask & HID_CONNECT_FF) && hdev->ff_init)
		hdev->ff_init(hdev);

	len = 0;
	if (hdev->claimed & HID_CLAIMED_INPUT)
		len += sprintf(buf + len, "input");
	if (hdev->claimed & HID_CLAIMED_HIDDEV)
		len += sprintf(buf + len, "%shiddev%d", len ? "," : "",
				hdev->minor);
	if (hdev->claimed & HID_CLAIMED_HIDRAW)
		len += sprintf(buf + len, "%shidraw%d", len ? "," : "",
				((struct hidraw *)hdev->hidraw)->minor);

	type = "Device";
	for (i = 0; i < hdev->maxcollection; i++) {
		struct hid_collection *col = &hdev->collection[i];
		if (col->type == HID_COLLECTION_APPLICATION &&
		   (col->usage & HID_USAGE_PAGE) == HID_UP_GENDESK &&
		   (col->usage & 0xffff) < ARRAY_SIZE(types)) {
			type = types[col->usage & 0xffff];
			break;
		}
	}

	switch (hdev->bus) {
	case BUS_USB:
		bus = "USB";
		break;
	case BUS_BLUETOOTH:
		bus = "BLUETOOTH";
		break;
	case BUS_I2C:
		bus = "I2C";
		break;
	default:
		bus = "<UNKNOWN>";
	}

	ret = device_create_file(&hdev->dev, &dev_attr_country);
	if (ret)
		hid_warn(hdev,
			 "can't create sysfs country code attribute err: %d\n", ret);

	hid_info(hdev, "%s: %s HID v%x.%02x %s [%s] on %s\n",
		 buf, bus, hdev->version >> 8, hdev->version & 0xff,
		 type, hdev->name, hdev->phys);

	return 0;
}
