int dvb_usbv2_suspend(struct usb_interface *intf, pm_message_t msg)
{
	struct dvb_usb_device *d = usb_get_intfdata(intf);
	int ret = 0, i, active_fe;
	struct dvb_frontend *fe;
	dev_dbg(&d->udev->dev, "%s:\n", __func__);

	/* stop remote controller poll */
	if (d->rc_polling_active)
		cancel_delayed_work_sync(&d->rc_query_work);

	for (i = MAX_NO_OF_ADAPTER_PER_DEVICE - 1; i >= 0; i--) {
		active_fe = d->adapter[i].active_fe;
		if (d->adapter[i].dvb_adap.priv && active_fe != -1) {
			fe = d->adapter[i].fe[active_fe];
			d->adapter[i].suspend_resume_active = true;

			if (d->props->streaming_ctrl)
				d->props->streaming_ctrl(fe, 0);

			/* stop usb streaming */
			usb_urb_killv2(&d->adapter[i].stream);

			ret = dvb_frontend_suspend(fe);
		}
	}

	return ret;
}
