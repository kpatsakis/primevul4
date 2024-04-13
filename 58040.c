static int cp2112_read(struct cp2112_device *dev, u8 *data, size_t size)
{
	struct hid_device *hdev = dev->hdev;
	struct cp2112_force_read_report report;
	int ret;

	if (size > sizeof(dev->read_data))
		size = sizeof(dev->read_data);
	report.report = CP2112_DATA_READ_FORCE_SEND;
	report.length = cpu_to_be16(size);

	atomic_set(&dev->read_avail, 0);

	ret = cp2112_hid_output(hdev, &report.report, sizeof(report),
				HID_OUTPUT_REPORT);
	if (ret < 0) {
		hid_warn(hdev, "Error requesting data: %d\n", ret);
		return ret;
	}

	ret = cp2112_wait(dev, &dev->read_avail);
	if (ret)
		return ret;

	hid_dbg(hdev, "read %d of %zd bytes requested\n",
		dev->read_length, size);

	if (size > dev->read_length)
		size = dev->read_length;

	memcpy(data, dev->read_data, size);
	return dev->read_length;
}
