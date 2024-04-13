static int dvb_usbv2_media_device_init(struct dvb_usb_adapter *adap)
{
#ifdef CONFIG_MEDIA_CONTROLLER_DVB
	struct media_device *mdev;
	struct dvb_usb_device *d = adap_to_d(adap);
	struct usb_device *udev = d->udev;

	mdev = kzalloc(sizeof(*mdev), GFP_KERNEL);
	if (!mdev)
		return -ENOMEM;

	media_device_usb_init(mdev, udev, d->name);

	dvb_register_media_controller(&adap->dvb_adap, mdev);

	dev_info(&d->udev->dev, "media controller created\n");
#endif
	return 0;
}
