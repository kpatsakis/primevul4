static int dvb_usbv2_adapter_frontend_init(struct dvb_usb_adapter *adap)
{
	int ret, i, count_registered = 0;
	struct dvb_usb_device *d = adap_to_d(adap);
	dev_dbg(&d->udev->dev, "%s: adap=%d\n", __func__, adap->id);

	memset(adap->fe, 0, sizeof(adap->fe));
	adap->active_fe = -1;

	if (d->props->frontend_attach) {
		ret = d->props->frontend_attach(adap);
		if (ret < 0) {
			dev_dbg(&d->udev->dev,
					"%s: frontend_attach() failed=%d\n",
					__func__, ret);
			goto err_dvb_frontend_detach;
		}
	} else {
		dev_dbg(&d->udev->dev, "%s: frontend_attach() do not exists\n",
				__func__);
		ret = 0;
		goto err;
	}

	for (i = 0; i < MAX_NO_OF_FE_PER_ADAP && adap->fe[i]; i++) {
		adap->fe[i]->id = i;
		/* re-assign sleep and wakeup functions */
		adap->fe_init[i] = adap->fe[i]->ops.init;
		adap->fe[i]->ops.init = dvb_usb_fe_init;
		adap->fe_sleep[i] = adap->fe[i]->ops.sleep;
		adap->fe[i]->ops.sleep = dvb_usb_fe_sleep;

		ret = dvb_register_frontend(&adap->dvb_adap, adap->fe[i]);
		if (ret < 0) {
			dev_err(&d->udev->dev,
					"%s: frontend%d registration failed\n",
					KBUILD_MODNAME, i);
			goto err_dvb_unregister_frontend;
		}

		count_registered++;
	}

	if (d->props->tuner_attach) {
		ret = d->props->tuner_attach(adap);
		if (ret < 0) {
			dev_dbg(&d->udev->dev, "%s: tuner_attach() failed=%d\n",
					__func__, ret);
			goto err_dvb_unregister_frontend;
		}
	}

	ret = dvb_create_media_graph(&adap->dvb_adap, true);
	if (ret < 0)
		goto err_dvb_unregister_frontend;

	ret = dvb_usbv2_media_device_register(adap);

	return ret;

err_dvb_unregister_frontend:
	for (i = count_registered - 1; i >= 0; i--)
		dvb_unregister_frontend(adap->fe[i]);

err_dvb_frontend_detach:
	for (i = MAX_NO_OF_FE_PER_ADAP - 1; i >= 0; i--) {
		if (adap->fe[i]) {
			dvb_frontend_detach(adap->fe[i]);
			adap->fe[i] = NULL;
		}
	}

err:
	dev_dbg(&d->udev->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}
