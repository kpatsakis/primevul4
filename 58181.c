static int dvb_usbv2_remote_init(struct dvb_usb_device *d)
{
	int ret;
	struct rc_dev *dev;
	dev_dbg(&d->udev->dev, "%s:\n", __func__);

	if (dvb_usbv2_disable_rc_polling || !d->props->get_rc_config)
		return 0;

	d->rc.map_name = d->rc_map;
	ret = d->props->get_rc_config(d, &d->rc);
	if (ret < 0)
		goto err;

	/* disable rc when there is no keymap defined */
	if (!d->rc.map_name)
		return 0;

	dev = rc_allocate_device(d->rc.driver_type);
	if (!dev) {
		ret = -ENOMEM;
		goto err;
	}

	dev->dev.parent = &d->udev->dev;
	dev->input_name = d->name;
	usb_make_path(d->udev, d->rc_phys, sizeof(d->rc_phys));
	strlcat(d->rc_phys, "/ir0", sizeof(d->rc_phys));
	dev->input_phys = d->rc_phys;
	usb_to_input_id(d->udev, &dev->input_id);
	/* TODO: likely RC-core should took const char * */
	dev->driver_name = (char *) d->props->driver_name;
	dev->map_name = d->rc.map_name;
	dev->allowed_protocols = d->rc.allowed_protos;
	dev->change_protocol = d->rc.change_protocol;
	dev->priv = d;

	ret = rc_register_device(dev);
	if (ret < 0) {
		rc_free_device(dev);
		goto err;
	}

	d->rc_dev = dev;

	/* start polling if needed */
	if (d->rc.query && !d->rc.bulk_mode) {
		/* initialize a work queue for handling polling */
		INIT_DELAYED_WORK(&d->rc_query_work,
				dvb_usb_read_remote_control);
		dev_info(&d->udev->dev,
				"%s: schedule remote query interval to %d msecs\n",
				KBUILD_MODNAME, d->rc.interval);
		schedule_delayed_work(&d->rc_query_work,
				msecs_to_jiffies(d->rc.interval));
		d->rc_polling_active = true;
	}

	return 0;
err:
	dev_dbg(&d->udev->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}
