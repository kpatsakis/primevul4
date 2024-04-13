void __hid_request(struct hid_device *hid, struct hid_report *report,
		int reqtype)
{
	char *buf;
	int ret;
	int len;

	buf = hid_alloc_report_buf(report, GFP_KERNEL);
	if (!buf)
		return;

	len = hid_report_len(report);

	if (reqtype == HID_REQ_SET_REPORT)
		hid_output_report(report, buf);

	ret = hid->ll_driver->raw_request(hid, report->id, buf, len,
					  report->type, reqtype);
	if (ret < 0) {
		dbg_hid("unable to complete request: %d\n", ret);
		goto out;
	}

	if (reqtype == HID_REQ_GET_REPORT)
		hid_input_report(hid, report->type, buf, ret, 0);

out:
	kfree(buf);
}
