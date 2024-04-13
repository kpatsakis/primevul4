int usb_driver_set_configuration(struct usb_device *udev, int config)
{
	struct set_config_request *req;

	req = kmalloc(sizeof(*req), GFP_KERNEL);
	if (!req)
		return -ENOMEM;
	req->udev = udev;
	req->config = config;
	INIT_WORK(&req->work, driver_set_config_work);

	spin_lock(&set_config_lock);
	list_add(&req->node, &set_config_list);
	spin_unlock(&set_config_lock);

	usb_get_dev(udev);
	schedule_work(&req->work);
	return 0;
}
