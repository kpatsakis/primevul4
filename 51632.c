hiddev_lookup_usage(struct hid_device *hid, struct hiddev_usage_ref *uref)
{
	int i, j;
	struct hid_report *report;
	struct hid_report_enum *report_enum;
	struct hid_field *field;

	if (uref->report_type < HID_REPORT_TYPE_MIN ||
	    uref->report_type > HID_REPORT_TYPE_MAX)
		return NULL;

	report_enum = hid->report_enum +
		(uref->report_type - HID_REPORT_TYPE_MIN);

	list_for_each_entry(report, &report_enum->report_list, list) {
		for (i = 0; i < report->maxfield; i++) {
			field = report->field[i];
			for (j = 0; j < field->maxusage; j++) {
				if (field->usage[j].hid == uref->usage_code) {
					uref->report_id = report->id;
					uref->field_index = i;
					uref->usage_index = j;
					return field;
				}
			}
		}
	}

	return NULL;
}
