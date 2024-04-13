static int hid_uevent(struct device *dev, struct kobj_uevent_env *env)
{
	struct hid_device *hdev = to_hid_device(dev);	

	if (add_uevent_var(env, "HID_ID=%04X:%08X:%08X",
			hdev->bus, hdev->vendor, hdev->product))
		return -ENOMEM;

	if (add_uevent_var(env, "HID_NAME=%s", hdev->name))
		return -ENOMEM;

	if (add_uevent_var(env, "HID_PHYS=%s", hdev->phys))
		return -ENOMEM;

	if (add_uevent_var(env, "HID_UNIQ=%s", hdev->uniq))
		return -ENOMEM;

	if (add_uevent_var(env, "MODALIAS=hid:b%04Xg%04Xv%08Xp%08X",
			   hdev->bus, hdev->group, hdev->vendor, hdev->product))
		return -ENOMEM;

	return 0;
}
