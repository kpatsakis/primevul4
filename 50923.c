int hid_report_raw_event(struct hid_device *hid, int type, u8 *data, int size,
		int interrupt)
{
	struct hid_report_enum *report_enum = hid->report_enum + type;
	struct hid_report *report;
	struct hid_driver *hdrv;
	unsigned int a;
	int rsize, csize = size;
	u8 *cdata = data;
	int ret = 0;

	report = hid_get_report(report_enum, data);
	if (!report)
		goto out;

	if (report_enum->numbered) {
		cdata++;
		csize--;
	}

	rsize = ((report->size - 1) >> 3) + 1;

	if (rsize > HID_MAX_BUFFER_SIZE)
		rsize = HID_MAX_BUFFER_SIZE;

	if (csize < rsize) {
		dbg_hid("report %d is too short, (%d < %d)\n", report->id,
				csize, rsize);
		memset(cdata + csize, 0, rsize - csize);
	}

	if ((hid->claimed & HID_CLAIMED_HIDDEV) && hid->hiddev_report_event)
		hid->hiddev_report_event(hid, report);
	if (hid->claimed & HID_CLAIMED_HIDRAW) {
		ret = hidraw_report_event(hid, data, size);
		if (ret)
			goto out;
	}

	if (hid->claimed != HID_CLAIMED_HIDRAW && report->maxfield) {
		for (a = 0; a < report->maxfield; a++)
			hid_input_field(hid, report->field[a], cdata, interrupt);
		hdrv = hid->driver;
		if (hdrv && hdrv->report)
			hdrv->report(hid, report);
	}

	if (hid->claimed & HID_CLAIMED_INPUT)
		hidinput_report_event(hid, report);
out:
	return ret;
}
