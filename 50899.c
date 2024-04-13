static int hid_device_probe(struct device *dev)
{
	struct hid_driver *hdrv = to_hid_driver(dev->driver);
	struct hid_device *hdev = to_hid_device(dev);
	const struct hid_device_id *id;
	int ret = 0;

	if (down_interruptible(&hdev->driver_lock))
		return -EINTR;
	if (down_interruptible(&hdev->driver_input_lock)) {
		ret = -EINTR;
		goto unlock_driver_lock;
	}
	hdev->io_started = false;

	if (!hdev->driver) {
		id = hid_match_device(hdev, hdrv);
		if (id == NULL) {
			ret = -ENODEV;
			goto unlock;
		}

		hdev->driver = hdrv;
		if (hdrv->probe) {
			ret = hdrv->probe(hdev, id);
		} else { /* default probe */
			ret = hid_open_report(hdev);
			if (!ret)
				ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
		}
		if (ret) {
			hid_close_report(hdev);
			hdev->driver = NULL;
		}
	}
unlock:
	if (!hdev->io_started)
		up(&hdev->driver_input_lock);
unlock_driver_lock:
	up(&hdev->driver_lock);
	return ret;
}
