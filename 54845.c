int __snd_seq_driver_register(struct snd_seq_driver *drv, struct module *mod)
{
	if (WARN_ON(!drv->driver.name || !drv->id))
		return -EINVAL;
	drv->driver.bus = &snd_seq_bus_type;
	drv->driver.owner = mod;
	return driver_register(&drv->driver);
}
