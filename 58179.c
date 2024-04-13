int dvb_usbv2_probe(struct usb_interface *intf,
		const struct usb_device_id *id)
{
	int ret;
	struct dvb_usb_device *d;
	struct usb_device *udev = interface_to_usbdev(intf);
	struct dvb_usb_driver_info *driver_info =
			(struct dvb_usb_driver_info *) id->driver_info;

	dev_dbg(&udev->dev, "%s: bInterfaceNumber=%d\n", __func__,
			intf->cur_altsetting->desc.bInterfaceNumber);

	if (!id->driver_info) {
		dev_err(&udev->dev, "%s: driver_info failed\n", KBUILD_MODNAME);
		ret = -ENODEV;
		goto err;
	}

	d = kzalloc(sizeof(struct dvb_usb_device), GFP_KERNEL);
	if (!d) {
		dev_err(&udev->dev, "%s: kzalloc() failed\n", KBUILD_MODNAME);
		ret = -ENOMEM;
		goto err;
	}

	d->intf = intf;
	d->name = driver_info->name;
	d->rc_map = driver_info->rc_map;
	d->udev = udev;
	d->props = driver_info->props;

	if (intf->cur_altsetting->desc.bInterfaceNumber !=
			d->props->bInterfaceNumber) {
		ret = -ENODEV;
		goto err_free_all;
	}

	mutex_init(&d->usb_mutex);
	mutex_init(&d->i2c_mutex);

	if (d->props->size_of_priv) {
		d->priv = kzalloc(d->props->size_of_priv, GFP_KERNEL);
		if (!d->priv) {
			dev_err(&d->udev->dev, "%s: kzalloc() failed\n",
					KBUILD_MODNAME);
			ret = -ENOMEM;
			goto err_free_all;
		}
	}

	if (d->props->identify_state) {
		const char *name = NULL;
		ret = d->props->identify_state(d, &name);
		if (ret == 0) {
			;
		} else if (ret == COLD) {
			dev_info(&d->udev->dev,
					"%s: found a '%s' in cold state\n",
					KBUILD_MODNAME, d->name);

			if (!name)
				name = d->props->firmware;

			ret = dvb_usbv2_download_firmware(d, name);
			if (ret == 0) {
				/* device is warm, continue initialization */
				;
			} else if (ret == RECONNECTS_USB) {
				/*
				 * USB core will call disconnect() and then
				 * probe() as device reconnects itself from the
				 * USB bus. disconnect() will release all driver
				 * resources and probe() is called for 'new'
				 * device. As 'new' device is warm we should
				 * never go here again.
				 */
				goto exit;
			} else {
				goto err_free_all;
			}
		} else {
			goto err_free_all;
		}
	}

	dev_info(&d->udev->dev, "%s: found a '%s' in warm state\n",
			KBUILD_MODNAME, d->name);

	ret = dvb_usbv2_init(d);
	if (ret < 0)
		goto err_free_all;

	dev_info(&d->udev->dev,
			"%s: '%s' successfully initialized and connected\n",
			KBUILD_MODNAME, d->name);
exit:
	usb_set_intfdata(intf, d);

	return 0;
err_free_all:
	dvb_usbv2_exit(d);
err:
	dev_dbg(&udev->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}
