void hiddev_hid_event(struct hid_device *hid, struct hid_field *field,
		      struct hid_usage *usage, __s32 value)
{
	unsigned type = field->report_type;
	struct hiddev_usage_ref uref;

	uref.report_type =
	  (type == HID_INPUT_REPORT) ? HID_REPORT_TYPE_INPUT :
	  ((type == HID_OUTPUT_REPORT) ? HID_REPORT_TYPE_OUTPUT :
	   ((type == HID_FEATURE_REPORT) ? HID_REPORT_TYPE_FEATURE : 0));
	uref.report_id = field->report->id;
	uref.field_index = field->index;
	uref.usage_index = (usage - field->usage);
	uref.usage_code = usage->hid;
	uref.value = value;

	hiddev_send_event(hid, &uref);
}
