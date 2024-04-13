static int pl_input_mapping(struct hid_device *hdev, struct hid_input *hi,
		struct hid_field *field, struct hid_usage *usage,
		unsigned long **bit, int *max)
{
	if ((usage->hid & HID_USAGE_PAGE) == HID_UP_LOGIVENDOR) {
		switch (usage->hid & HID_USAGE) {
		case 0x05a: pl_map_key_clear(KEY_TEXT);		break;
		case 0x05b: pl_map_key_clear(KEY_RED);		break;
		case 0x05c: pl_map_key_clear(KEY_GREEN);	break;
		case 0x05d: pl_map_key_clear(KEY_YELLOW);	break;
		case 0x05e: pl_map_key_clear(KEY_BLUE);		break;
		default:
			return 0;
		}
		return 1;
	}

	if ((usage->hid & HID_USAGE_PAGE) == HID_UP_CONSUMER) {
		switch (usage->hid & HID_USAGE) {
		case 0x0f6: pl_map_key_clear(KEY_NEXT);		break;
		case 0x0fa: pl_map_key_clear(KEY_BACK);		break;
		default:
			return 0;
		}
		return 1;
	}

	return 0;
}
