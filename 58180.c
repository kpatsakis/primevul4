static int dvb_usbv2_remote_exit(struct dvb_usb_device *d)
{
	dev_dbg(&d->udev->dev, "%s:\n", __func__);

	if (d->rc_dev) {
		cancel_delayed_work_sync(&d->rc_query_work);
		rc_unregister_device(d->rc_dev);
		d->rc_dev = NULL;
	}

	return 0;
}
