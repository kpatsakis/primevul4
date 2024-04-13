struct hid_report *picolcd_report(int id, struct hid_device *hdev, int dir)
{
	struct list_head *feature_report_list = &hdev->report_enum[dir].report_list;
	struct hid_report *report = NULL;

	list_for_each_entry(report, feature_report_list, list) {
		if (report->id == id)
			return report;
	}
	hid_warn(hdev, "No report with id 0x%x found\n", id);
	return NULL;
}
