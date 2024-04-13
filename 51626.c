void hiddev_disconnect(struct hid_device *hid)
{
	struct hiddev *hiddev = hid->hiddev;
	struct usbhid_device *usbhid = hid->driver_data;

	usb_deregister_dev(usbhid->intf, &hiddev_class);

	mutex_lock(&hiddev->existancelock);
	hiddev->exist = 0;

	if (hiddev->open) {
		mutex_unlock(&hiddev->existancelock);
		usbhid_close(hiddev->hid);
		wake_up_interruptible(&hiddev->wait);
	} else {
		mutex_unlock(&hiddev->existancelock);
		kfree(hiddev);
	}
}
