static void hid_scan_collection(struct hid_parser *parser, unsigned type)
{
	struct hid_device *hid = parser->device;
	int i;

	if (((parser->global.usage_page << 16) == HID_UP_SENSOR) &&
	    type == HID_COLLECTION_PHYSICAL)
		hid->group = HID_GROUP_SENSOR_HUB;

	if (hid->vendor == USB_VENDOR_ID_MICROSOFT &&
	    (hid->product == USB_DEVICE_ID_MS_TYPE_COVER_PRO_3 ||
	     hid->product == USB_DEVICE_ID_MS_TYPE_COVER_PRO_3_2 ||
	     hid->product == USB_DEVICE_ID_MS_TYPE_COVER_PRO_3_JP ||
	     hid->product == USB_DEVICE_ID_MS_TYPE_COVER_3 ||
	     hid->product == USB_DEVICE_ID_MS_POWER_COVER) &&
	    hid->group == HID_GROUP_MULTITOUCH)
		hid->group = HID_GROUP_GENERIC;

	if ((parser->global.usage_page << 16) == HID_UP_GENDESK)
		for (i = 0; i < parser->local.usage_index; i++)
			if (parser->local.usage[i] == HID_GD_POINTER)
				parser->scan_flags |= HID_SCAN_FLAG_GD_POINTER;

	if ((parser->global.usage_page << 16) >= HID_UP_MSVENDOR)
		parser->scan_flags |= HID_SCAN_FLAG_VENDOR_SPECIFIC;
}
