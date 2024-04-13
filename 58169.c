static int dvb_usbv2_adapter_frontend_exit(struct dvb_usb_adapter *adap)
{
	int ret, i;
	struct dvb_usb_device *d = adap_to_d(adap);

	dev_dbg(&d->udev->dev, "%s: adap=%d\n", __func__, adap->id);

	for (i = MAX_NO_OF_FE_PER_ADAP - 1; i >= 0; i--) {
		if (adap->fe[i]) {
			dvb_unregister_frontend(adap->fe[i]);
			dvb_frontend_detach(adap->fe[i]);
		}
	}

	if (d->props->tuner_detach) {
		ret = d->props->tuner_detach(adap);
		if (ret < 0) {
			dev_dbg(&d->udev->dev, "%s: tuner_detach() failed=%d\n",
					__func__, ret);
		}
	}

	if (d->props->frontend_detach) {
		ret = d->props->frontend_detach(adap);
		if (ret < 0) {
			dev_dbg(&d->udev->dev,
					"%s: frontend_detach() failed=%d\n",
					__func__, ret);
		}
	}

	return 0;
}
