static ssize_t pstr_store(struct device *kdev,
			  struct device_attribute *kattr, const char *buf,
			  size_t count)
{
	struct hid_device *hdev = to_hid_device(kdev);
	struct cp2112_pstring_attribute *attr =
		container_of(kattr, struct cp2112_pstring_attribute, attr);
	struct cp2112_string_report report;
	int ret;

	memset(&report, 0, sizeof(report));

	ret = utf8s_to_utf16s(buf, count, UTF16_LITTLE_ENDIAN,
			      report.string, ARRAY_SIZE(report.string));
	report.report = attr->report;
	report.length = ret * sizeof(report.string[0]) + 2;
	report.type = USB_DT_STRING;

	ret = cp2112_hid_output(hdev, &report.report, report.length + 1,
				HID_FEATURE_REPORT);
	if (ret != report.length + 1) {
		hid_err(hdev, "error writing %s string: %d\n", kattr->attr.name,
			ret);
		if (ret < 0)
			return ret;
		return -EIO;
	}

	chmod_sysfs_attrs(hdev);
	return count;
}
