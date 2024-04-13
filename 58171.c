static int dvb_usbv2_adapter_init(struct dvb_usb_device *d)
{
	struct dvb_usb_adapter *adap;
	int ret, i, adapter_count;

	/* resolve adapter count */
	adapter_count = d->props->num_adapters;
	if (d->props->get_adapter_count) {
		ret = d->props->get_adapter_count(d);
		if (ret < 0)
			goto err;

		adapter_count = ret;
	}

	for (i = 0; i < adapter_count; i++) {
		adap = &d->adapter[i];
		adap->id = i;
		adap->props = &d->props->adapter[i];

		/* speed - when running at FULL speed we need a HW PID filter */
		if (d->udev->speed == USB_SPEED_FULL &&
				!(adap->props->caps & DVB_USB_ADAP_HAS_PID_FILTER)) {
			dev_err(&d->udev->dev,
					"%s: this USB2.0 device cannot be run on a USB1.1 port (it lacks a hardware PID filter)\n",
					KBUILD_MODNAME);
			ret = -ENODEV;
			goto err;
		} else if ((d->udev->speed == USB_SPEED_FULL &&
				adap->props->caps & DVB_USB_ADAP_HAS_PID_FILTER) ||
				(adap->props->caps & DVB_USB_ADAP_NEED_PID_FILTERING)) {
			dev_info(&d->udev->dev,
					"%s: will use the device's hardware PID filter (table count: %d)\n",
					KBUILD_MODNAME,
					adap->props->pid_filter_count);
			adap->pid_filtering  = 1;
			adap->max_feed_count = adap->props->pid_filter_count;
		} else {
			dev_info(&d->udev->dev,
					"%s: will pass the complete MPEG2 transport stream to the software demuxer\n",
					KBUILD_MODNAME);
			adap->pid_filtering  = 0;
			adap->max_feed_count = 255;
		}

		if (!adap->pid_filtering && dvb_usb_force_pid_filter_usage &&
				adap->props->caps & DVB_USB_ADAP_HAS_PID_FILTER) {
			dev_info(&d->udev->dev,
					"%s: PID filter enabled by module option\n",
					KBUILD_MODNAME);
			adap->pid_filtering  = 1;
			adap->max_feed_count = adap->props->pid_filter_count;
		}

		ret = dvb_usbv2_adapter_stream_init(adap);
		if (ret)
			goto err;

		ret = dvb_usbv2_adapter_dvb_init(adap);
		if (ret)
			goto err;

		ret = dvb_usbv2_adapter_frontend_init(adap);
		if (ret)
			goto err;

		/* use exclusive FE lock if there is multiple shared FEs */
		if (adap->fe[1])
			adap->dvb_adap.mfe_shared = 1;
	}

	return 0;
err:
	dev_dbg(&d->udev->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}
