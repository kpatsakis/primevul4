void hid_destroy_device(struct hid_device *hdev)
{
	hid_remove_device(hdev);
	put_device(&hdev->dev);
}
