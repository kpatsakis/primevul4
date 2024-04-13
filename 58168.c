static int dvb_usbv2_adapter_exit(struct dvb_usb_device *d)
{
	int i;
	dev_dbg(&d->udev->dev, "%s:\n", __func__);

	for (i = MAX_NO_OF_ADAPTER_PER_DEVICE - 1; i >= 0; i--) {
		if (d->adapter[i].props) {
			dvb_usbv2_adapter_dvb_exit(&d->adapter[i]);
			dvb_usbv2_adapter_stream_exit(&d->adapter[i]);
			dvb_usbv2_adapter_frontend_exit(&d->adapter[i]);
			dvb_usbv2_media_device_unregister(&d->adapter[i]);
		}
	}

	return 0;
}
