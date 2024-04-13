static int ap_device_probe(struct device *dev)
{
	struct ap_device *ap_dev = to_ap_dev(dev);
	struct ap_driver *ap_drv = to_ap_drv(dev->driver);
	int rc;

	ap_dev->drv = ap_drv;

	spin_lock_bh(&ap_device_list_lock);
	list_add(&ap_dev->list, &ap_device_list);
	spin_unlock_bh(&ap_device_list_lock);

	rc = ap_drv->probe ? ap_drv->probe(ap_dev) : -ENODEV;
	if (rc) {
		spin_lock_bh(&ap_device_list_lock);
		list_del_init(&ap_dev->list);
		spin_unlock_bh(&ap_device_list_lock);
	}
	return rc;
}
