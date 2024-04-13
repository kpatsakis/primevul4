static int hid_scan_main(struct hid_parser *parser, struct hid_item *item)
{
	__u32 data;
	int i;

	data = item_udata(item);

	switch (item->tag) {
	case HID_MAIN_ITEM_TAG_BEGIN_COLLECTION:
		hid_scan_collection(parser, data & 0xff);
		break;
	case HID_MAIN_ITEM_TAG_END_COLLECTION:
		break;
	case HID_MAIN_ITEM_TAG_INPUT:
		/* ignore constant inputs, they will be ignored by hid-input */
		if (data & HID_MAIN_ITEM_CONSTANT)
			break;
		for (i = 0; i < parser->local.usage_index; i++)
			hid_scan_input_usage(parser, parser->local.usage[i]);
		break;
	case HID_MAIN_ITEM_TAG_OUTPUT:
		break;
	case HID_MAIN_ITEM_TAG_FEATURE:
		for (i = 0; i < parser->local.usage_index; i++)
			hid_scan_feature_usage(parser, parser->local.usage[i]);
		break;
	}

	/* Reset the local parser environment */
	memset(&parser->local, 0, sizeof(parser->local));

	return 0;
}
