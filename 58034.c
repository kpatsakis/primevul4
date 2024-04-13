static int cp2112_hid_output(struct hid_device *hdev, u8 *data, size_t count,
			     unsigned char report_type)
{
	u8 *buf;
	int ret;

	buf = kmemdup(data, count, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	if (report_type == HID_OUTPUT_REPORT)
		ret = hid_hw_output_report(hdev, buf, count);
	else
		ret = hid_hw_raw_request(hdev, buf[0], buf, count, report_type,
				HID_REQ_SET_REPORT);

	kfree(buf);
	return ret;
}
