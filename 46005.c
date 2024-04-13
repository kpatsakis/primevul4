static int do_devconfig_ioctl(struct comedi_device *dev,
			      struct comedi_devconfig __user *arg)
{
	struct comedi_devconfig it;
	int ret;
	unsigned char *aux_data = NULL;
	int aux_len;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (arg == NULL) {
		if (is_device_busy(dev))
			return -EBUSY;
		if (dev->attached) {
			struct module *driver_module = dev->driver->module;
			comedi_device_detach(dev);
			module_put(driver_module);
		}
		return 0;
	}

	if (copy_from_user(&it, arg, sizeof(struct comedi_devconfig)))
		return -EFAULT;

	it.board_name[COMEDI_NAMELEN - 1] = 0;

	if (comedi_aux_data(it.options, 0) &&
	    it.options[COMEDI_DEVCONF_AUX_DATA_LENGTH]) {
		int bit_shift;
		aux_len = it.options[COMEDI_DEVCONF_AUX_DATA_LENGTH];
		if (aux_len < 0)
			return -EFAULT;

		aux_data = vmalloc(aux_len);
		if (!aux_data)
			return -ENOMEM;

		if (copy_from_user(aux_data,
				   comedi_aux_data(it.options, 0), aux_len)) {
			vfree(aux_data);
			return -EFAULT;
		}
		it.options[COMEDI_DEVCONF_AUX_DATA_LO] =
		    (unsigned long)aux_data;
		if (sizeof(void *) > sizeof(int)) {
			bit_shift = sizeof(int) * 8;
			it.options[COMEDI_DEVCONF_AUX_DATA_HI] =
			    ((unsigned long)aux_data) >> bit_shift;
		} else
			it.options[COMEDI_DEVCONF_AUX_DATA_HI] = 0;
	}

	ret = comedi_device_attach(dev, &it);
	if (ret == 0) {
		if (!try_module_get(dev->driver->module)) {
			comedi_device_detach(dev);
			return -ENOSYS;
		}
	}

	if (aux_data)
		vfree(aux_data);

	return ret;
}
