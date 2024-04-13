static void hid_device_release(struct device *dev)
{
	struct hid_device *hid = to_hid_device(dev);

	hid_close_report(hid);
	kfree(hid->dev_rdesc);
	kfree(hid);
}
