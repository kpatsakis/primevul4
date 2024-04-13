hiddev_lookup_report(struct hid_device *hid, struct hiddev_report_info *rinfo)
{
	unsigned int flags = rinfo->report_id & ~HID_REPORT_ID_MASK;
	unsigned int rid = rinfo->report_id & HID_REPORT_ID_MASK;
	struct hid_report_enum *report_enum;
	struct hid_report *report;
	struct list_head *list;

	if (rinfo->report_type < HID_REPORT_TYPE_MIN ||
	    rinfo->report_type > HID_REPORT_TYPE_MAX)
		return NULL;

	report_enum = hid->report_enum +
		(rinfo->report_type - HID_REPORT_TYPE_MIN);

	switch (flags) {
	case 0: /* Nothing to do -- report_id is already set correctly */
		break;

	case HID_REPORT_ID_FIRST:
		if (list_empty(&report_enum->report_list))
			return NULL;

		list = report_enum->report_list.next;
		report = list_entry(list, struct hid_report, list);
		rinfo->report_id = report->id;
		break;

	case HID_REPORT_ID_NEXT:
		report = report_enum->report_id_hash[rid];
		if (!report)
			return NULL;

		list = report->list.next;
		if (list == &report_enum->report_list)
			return NULL;

		report = list_entry(list, struct hid_report, list);
		rinfo->report_id = report->id;
		break;

	default:
		return NULL;
	}

	return report_enum->report_id_hash[rinfo->report_id];
}
