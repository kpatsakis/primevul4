static bool hid_hiddev(struct hid_device *hdev)
{
	return !!hid_match_id(hdev, hid_hiddev_list);
}
