static int logi_djdevice_probe(struct hid_device *hdev,
			 const struct hid_device_id *id)
{
	int ret;
	struct dj_device *dj_dev = hdev->driver_data;

	if (!is_dj_device(dj_dev))
		return -ENODEV;

	ret = hid_parse(hdev);
	if (!ret)
		ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);

	return ret;
}
