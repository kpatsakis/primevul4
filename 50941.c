static ssize_t store_new_id(struct device_driver *drv, const char *buf,
		size_t count)
{
	struct hid_driver *hdrv = to_hid_driver(drv);
	struct hid_dynid *dynid;
	__u32 bus, vendor, product;
	unsigned long driver_data = 0;
	int ret;

	ret = sscanf(buf, "%x %x %x %lx",
			&bus, &vendor, &product, &driver_data);
	if (ret < 3)
		return -EINVAL;

	dynid = kzalloc(sizeof(*dynid), GFP_KERNEL);
	if (!dynid)
		return -ENOMEM;

	dynid->id.bus = bus;
	dynid->id.group = HID_GROUP_ANY;
	dynid->id.vendor = vendor;
	dynid->id.product = product;
	dynid->id.driver_data = driver_data;

	spin_lock(&hdrv->dyn_lock);
	list_add_tail(&dynid->list, &hdrv->dyn_list);
	spin_unlock(&hdrv->dyn_lock);

	ret = driver_attach(&hdrv->driver);

	return ret ? : count;
}
