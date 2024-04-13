int dvb_usbv2_reset_resume(struct usb_interface *intf)
{
	struct dvb_usb_device *d = usb_get_intfdata(intf);
	int ret;
	dev_dbg(&d->udev->dev, "%s:\n", __func__);

	dvb_usbv2_device_power_ctrl(d, 1);

	if (d->props->init)
		d->props->init(d);

	ret = dvb_usbv2_resume_common(d);

	dvb_usbv2_device_power_ctrl(d, 0);

	return ret;
}
