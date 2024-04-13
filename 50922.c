static void hid_remove_device(struct hid_device *hdev)
{
	if (hdev->status & HID_STAT_ADDED) {
		device_del(&hdev->dev);
		hid_debug_unregister(hdev);
		hdev->status &= ~HID_STAT_ADDED;
	}
	kfree(hdev->dev_rdesc);
	hdev->dev_rdesc = NULL;
	hdev->dev_rsize = 0;
}
