int __hid_register_driver(struct hid_driver *hdrv, struct module *owner,
		const char *mod_name)
{
	int ret;

	hdrv->driver.name = hdrv->name;
	hdrv->driver.bus = &hid_bus_type;
	hdrv->driver.owner = owner;
	hdrv->driver.mod_name = mod_name;

	INIT_LIST_HEAD(&hdrv->dyn_list);
	spin_lock_init(&hdrv->dyn_lock);

	ret = driver_register(&hdrv->driver);
	if (ret)
		return ret;

	ret = driver_create_file(&hdrv->driver, &driver_attr_new_id);
	if (ret)
		driver_unregister(&hdrv->driver);

	return ret;
}
