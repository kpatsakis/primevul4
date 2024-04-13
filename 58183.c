static int dvb_usbv2_resume_common(struct dvb_usb_device *d)
{
	int ret = 0, i, active_fe;
	struct dvb_frontend *fe;
	dev_dbg(&d->udev->dev, "%s:\n", __func__);

	for (i = 0; i < MAX_NO_OF_ADAPTER_PER_DEVICE; i++) {
		active_fe = d->adapter[i].active_fe;
		if (d->adapter[i].dvb_adap.priv && active_fe != -1) {
			fe = d->adapter[i].fe[active_fe];

			ret = dvb_frontend_resume(fe);

			/* resume usb streaming */
			usb_urb_submitv2(&d->adapter[i].stream, NULL);

			if (d->props->streaming_ctrl)
				d->props->streaming_ctrl(fe, 1);

			d->adapter[i].suspend_resume_active = false;
		}
	}

	/* start remote controller poll */
	if (d->rc_polling_active)
		schedule_delayed_work(&d->rc_query_work,
				msecs_to_jiffies(d->rc.interval));

	return ret;
}
