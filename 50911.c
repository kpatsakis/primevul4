static bool hid_match_one_id(struct hid_device *hdev,
		const struct hid_device_id *id)
{
	return (id->bus == HID_BUS_ANY || id->bus == hdev->bus) &&
		(id->group == HID_GROUP_ANY || id->group == hdev->group) &&
		(id->vendor == HID_ANY_ID || id->vendor == hdev->vendor) &&
		(id->product == HID_ANY_ID || id->product == hdev->product);
}
