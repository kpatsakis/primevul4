static int hid_scan_report(struct hid_device *hid)
{
	struct hid_parser *parser;
	struct hid_item item;
	__u8 *start = hid->dev_rdesc;
	__u8 *end = start + hid->dev_rsize;
	static int (*dispatch_type[])(struct hid_parser *parser,
				      struct hid_item *item) = {
		hid_scan_main,
		hid_parser_global,
		hid_parser_local,
		hid_parser_reserved
	};

	parser = vzalloc(sizeof(struct hid_parser));
	if (!parser)
		return -ENOMEM;

	parser->device = hid;
	hid->group = HID_GROUP_GENERIC;

	/*
	 * The parsing is simpler than the one in hid_open_report() as we should
	 * be robust against hid errors. Those errors will be raised by
	 * hid_open_report() anyway.
	 */
	while ((start = fetch_item(start, end, &item)) != NULL)
		dispatch_type[item.type](parser, &item);

	/*
	 * Handle special flags set during scanning.
	 */
	if ((parser->scan_flags & HID_SCAN_FLAG_MT_WIN_8) &&
	    (hid->group == HID_GROUP_MULTITOUCH))
		hid->group = HID_GROUP_MULTITOUCH_WIN_8;

	/*
	 * Vendor specific handlings
	 */
	switch (hid->vendor) {
	case USB_VENDOR_ID_WACOM:
		hid->group = HID_GROUP_WACOM;
		break;
	case USB_VENDOR_ID_SYNAPTICS:
		if (hid->group == HID_GROUP_GENERIC)
			if ((parser->scan_flags & HID_SCAN_FLAG_VENDOR_SPECIFIC)
			    && (parser->scan_flags & HID_SCAN_FLAG_GD_POINTER))
				/*
				 * hid-rmi should take care of them,
				 * not hid-generic
				 */
				hid->group = HID_GROUP_RMI;
		break;
	}

	vfree(parser);
	return 0;
}
