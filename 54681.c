static void driver_set_config_work(struct work_struct *work)
{
	struct set_config_request *req =
		container_of(work, struct set_config_request, work);
	struct usb_device *udev = req->udev;

	usb_lock_device(udev);
	spin_lock(&set_config_lock);
	list_del(&req->node);
	spin_unlock(&set_config_lock);

	if (req->config >= -1)		/* Is req still valid? */
		usb_set_configuration(udev, req->config);
	usb_unlock_device(udev);
	usb_put_dev(udev);
	kfree(req);
}
