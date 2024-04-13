static int hidp_start(struct hid_device *hid)
{
	struct hidp_session *session = hid->driver_data;
	struct hid_report *report;

	if (hid->quirks & HID_QUIRK_NO_INIT_REPORTS)
		return 0;

	list_for_each_entry(report, &hid->report_enum[HID_INPUT_REPORT].
			report_list, list)
		hidp_send_report(session, report);

	list_for_each_entry(report, &hid->report_enum[HID_FEATURE_REPORT].
			report_list, list)
		hidp_send_report(session, report);

	return 0;
}
