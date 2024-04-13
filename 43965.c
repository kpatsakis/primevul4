static int kye_tablet_enable(struct hid_device *hdev)
{
	struct list_head *list;
	struct list_head *head;
	struct hid_report *report;
	__s32 *value;

	list = &hdev->report_enum[HID_FEATURE_REPORT].report_list;
	list_for_each(head, list) {
		report = list_entry(head, struct hid_report, list);
		if (report->id == 5)
			break;
	}

	if (head == list) {
		hid_err(hdev, "tablet-enabling feature report not found\n");
		return -ENODEV;
	}

	if (report->maxfield < 1 || report->field[0]->report_count < 7) {
		hid_err(hdev, "invalid tablet-enabling feature report\n");
		return -ENODEV;
	}

	value = report->field[0]->value;

	value[0] = 0x12;
	value[1] = 0x10;
	value[2] = 0x11;
	value[3] = 0x12;
	value[4] = 0x00;
	value[5] = 0x00;
	value[6] = 0x00;
	hid_hw_request(hdev, report, HID_REQ_SET_REPORT);

	return 0;
}
