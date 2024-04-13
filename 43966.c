static int lg_dinovo_mapping(struct hid_input *hi, struct hid_usage *usage,
		unsigned long **bit, int *max)
{
	if ((usage->hid & HID_USAGE_PAGE) != HID_UP_LOGIVENDOR)
		return 0;

	switch (usage->hid & HID_USAGE) {

	case 0x00d: lg_map_key_clear(KEY_MEDIA);	break;
	default:
		return 0;

	}
	return 1;
}
