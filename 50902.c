void hid_disconnect(struct hid_device *hdev)
{
	device_remove_file(&hdev->dev, &dev_attr_country);
	if (hdev->claimed & HID_CLAIMED_INPUT)
		hidinput_disconnect(hdev);
	if (hdev->claimed & HID_CLAIMED_HIDDEV)
		hdev->hiddev_disconnect(hdev);
	if (hdev->claimed & HID_CLAIMED_HIDRAW)
		hidraw_disconnect(hdev);
	hdev->claimed = 0;
}
