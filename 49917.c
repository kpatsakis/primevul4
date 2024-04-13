int ap_driver_register(struct ap_driver *ap_drv, struct module *owner,
		       char *name)
{
	struct device_driver *drv = &ap_drv->driver;

	drv->bus = &ap_bus_type;
	drv->probe = ap_device_probe;
	drv->remove = ap_device_remove;
	drv->owner = owner;
	drv->name = name;
	return driver_register(drv);
}
