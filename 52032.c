static int proc_setconfig(struct usb_dev_state *ps, void __user *arg)
{
	int u;
	int status = 0;
	struct usb_host_config *actconfig;

	if (get_user(u, (int __user *)arg))
		return -EFAULT;

	actconfig = ps->dev->actconfig;

	/* Don't touch the device if any interfaces are claimed.
	 * It could interfere with other drivers' operations, and if
	 * an interface is claimed by usbfs it could easily deadlock.
	 */
	if (actconfig) {
		int i;

		for (i = 0; i < actconfig->desc.bNumInterfaces; ++i) {
			if (usb_interface_claimed(actconfig->interface[i])) {
				dev_warn(&ps->dev->dev,
					"usbfs: interface %d claimed by %s "
					"while '%s' sets config #%d\n",
					actconfig->interface[i]
						->cur_altsetting
						->desc.bInterfaceNumber,
					actconfig->interface[i]
						->dev.driver->name,
					current->comm, u);
				status = -EBUSY;
				break;
			}
		}
	}

	/* SET_CONFIGURATION is often abused as a "cheap" driver reset,
	 * so avoid usb_set_configuration()'s kick to sysfs
	 */
	if (status == 0) {
		if (actconfig && actconfig->desc.bConfigurationValue == u)
			status = usb_reset_configuration(ps->dev);
		else
			status = usb_set_configuration(ps->dev, u);
	}

	return status;
}
