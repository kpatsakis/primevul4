struct hid_report *hid_validate_values(struct hid_device *hid,
				       unsigned int type, unsigned int id,
				       unsigned int field_index,
				       unsigned int report_counts)
{
	struct hid_report *report;

	if (type > HID_FEATURE_REPORT) {
		hid_err(hid, "invalid HID report type %u\n", type);
		return NULL;
	}

	if (id >= HID_MAX_IDS) {
		hid_err(hid, "invalid HID report id %u\n", id);
		return NULL;
	}

	/*
	 * Explicitly not using hid_get_report() here since it depends on
	 * ->numbered being checked, which may not always be the case when
	 * drivers go to access report values.
	 */
	if (id == 0) {
		/*
		 * Validating on id 0 means we should examine the first
		 * report in the list.
		 */
		report = list_entry(
				hid->report_enum[type].report_list.next,
				struct hid_report, list);
	} else {
		report = hid->report_enum[type].report_id_hash[id];
	}
	if (!report) {
		hid_err(hid, "missing %s %u\n", hid_report_names[type], id);
		return NULL;
	}
	if (report->maxfield <= field_index) {
		hid_err(hid, "not enough fields in %s %u\n",
			hid_report_names[type], id);
		return NULL;
	}
	if (report->field[field_index]->report_count < report_counts) {
		hid_err(hid, "not enough values in %s %u field %u\n",
			hid_report_names[type], id, field_index);
		return NULL;
	}
	return report;
}
