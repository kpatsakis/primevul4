static int cp2112_hid_get(struct hid_device *hdev, unsigned char report_number,
			  u8 *data, size_t count, unsigned char report_type)
{
	u8 *buf;
	int ret;

	buf = kmalloc(count, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	ret = hid_hw_raw_request(hdev, report_number, buf, count,
				       report_type, HID_REQ_GET_REPORT);
	memcpy(data, buf, count);
	kfree(buf);
	return ret;
}
