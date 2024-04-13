static void chmod_sysfs_attrs(struct hid_device *hdev)
{
	struct attribute **attr;
	u8 buf[2];
	int ret;

	ret = cp2112_hid_get(hdev, CP2112_LOCK_BYTE, buf, sizeof(buf),
			     HID_FEATURE_REPORT);
	if (ret != sizeof(buf)) {
		hid_err(hdev, "error reading lock byte: %d\n", ret);
		return;
	}

	for (attr = cp2112_attr_group.attrs; *attr; ++attr) {
		umode_t mode = (buf[1] & 1) ? S_IWUSR | S_IRUGO : S_IRUGO;
		ret = sysfs_chmod_file(&hdev->dev.kobj, *attr, mode);
		if (ret < 0)
			hid_err(hdev, "error chmoding sysfs file %s\n",
				(*attr)->name);
		buf[1] >>= 1;
	}
}
