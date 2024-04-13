static int hid_device_remove(struct device *dev)
{
	struct hid_device *hdev = to_hid_device(dev);
	struct hid_driver *hdrv;
	int ret = 0;

	if (down_interruptible(&hdev->driver_lock))
		return -EINTR;
	if (down_interruptible(&hdev->driver_input_lock)) {
		ret = -EINTR;
		goto unlock_driver_lock;
	}
	hdev->io_started = false;

	hdrv = hdev->driver;
	if (hdrv) {
		if (hdrv->remove)
			hdrv->remove(hdev);
		else /* default remove */
			hid_hw_stop(hdev);
		hid_close_report(hdev);
		hdev->driver = NULL;
	}

	if (!hdev->io_started)
		up(&hdev->driver_input_lock);
unlock_driver_lock:
	up(&hdev->driver_lock);
	return ret;
}
