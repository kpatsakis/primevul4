static void hid_scan_feature_usage(struct hid_parser *parser, u32 usage)
{
	if (usage == 0xff0000c5 && parser->global.report_count == 256 &&
	    parser->global.report_size == 8)
		parser->scan_flags |= HID_SCAN_FLAG_MT_WIN_8;
}
