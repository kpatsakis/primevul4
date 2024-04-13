int hid_input_report(struct hid_device *hid, int type, u8 *data, int size, int interrupt)
{
	struct hid_report_enum *report_enum;
	struct hid_driver *hdrv;
	struct hid_report *report;
	int ret = 0;

	if (!hid)
		return -ENODEV;

	if (down_trylock(&hid->driver_input_lock))
		return -EBUSY;

	if (!hid->driver) {
		ret = -ENODEV;
		goto unlock;
	}
	report_enum = hid->report_enum + type;
	hdrv = hid->driver;

	if (!size) {
		dbg_hid("empty report\n");
		ret = -1;
		goto unlock;
	}

	/* Avoid unnecessary overhead if debugfs is disabled */
	if (!list_empty(&hid->debug_list))
		hid_dump_report(hid, type, data, size);

	report = hid_get_report(report_enum, data);

	if (!report) {
		ret = -1;
		goto unlock;
	}

	if (hdrv && hdrv->raw_event && hid_match_report(hid, report)) {
		ret = hdrv->raw_event(hid, report, data, size);
		if (ret < 0)
			goto unlock;
	}

	ret = hid_report_raw_event(hid, type, data, size, interrupt);

unlock:
	up(&hid->driver_input_lock);
	return ret;
}
