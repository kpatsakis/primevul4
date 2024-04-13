static int dvb_usb_fe_init(struct dvb_frontend *fe)
{
	int ret;
	struct dvb_usb_adapter *adap = fe->dvb->priv;
	struct dvb_usb_device *d = adap_to_d(adap);
	dev_dbg(&d->udev->dev, "%s: adap=%d fe=%d\n", __func__, adap->id,
			fe->id);

	if (!adap->suspend_resume_active) {
		adap->active_fe = fe->id;
		set_bit(ADAP_INIT, &adap->state_bits);
	}

	ret = dvb_usbv2_device_power_ctrl(d, 1);
	if (ret < 0)
		goto err;

	if (d->props->frontend_ctrl) {
		ret = d->props->frontend_ctrl(fe, 1);
		if (ret < 0)
			goto err;
	}

	if (adap->fe_init[fe->id]) {
		ret = adap->fe_init[fe->id](fe);
		if (ret < 0)
			goto err;
	}
err:
	if (!adap->suspend_resume_active) {
		clear_bit(ADAP_INIT, &adap->state_bits);
		smp_mb__after_atomic();
		wake_up_bit(&adap->state_bits, ADAP_INIT);
	}

	dev_dbg(&d->udev->dev, "%s: ret=%d\n", __func__, ret);
	return ret;
}
