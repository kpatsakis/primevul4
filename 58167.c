static int dvb_usbv2_adapter_dvb_init(struct dvb_usb_adapter *adap)
{
	int ret;
	struct dvb_usb_device *d = adap_to_d(adap);

	dev_dbg(&d->udev->dev, "%s: adap=%d\n", __func__, adap->id);

	ret = dvb_register_adapter(&adap->dvb_adap, d->name, d->props->owner,
			&d->udev->dev, d->props->adapter_nr);
	if (ret < 0) {
		dev_dbg(&d->udev->dev, "%s: dvb_register_adapter() failed=%d\n",
				__func__, ret);
		goto err_dvb_register_adapter;
	}

	adap->dvb_adap.priv = adap;

	ret = dvb_usbv2_media_device_init(adap);
	if (ret < 0) {
		dev_dbg(&d->udev->dev, "%s: dvb_usbv2_media_device_init() failed=%d\n",
				__func__, ret);
		goto err_dvb_register_mc;
	}

	if (d->props->read_mac_address) {
		ret = d->props->read_mac_address(adap,
				adap->dvb_adap.proposed_mac);
		if (ret < 0)
			goto err_dvb_dmx_init;

		dev_info(&d->udev->dev, "%s: MAC address: %pM\n",
				KBUILD_MODNAME, adap->dvb_adap.proposed_mac);
	}

	adap->demux.dmx.capabilities = DMX_TS_FILTERING | DMX_SECTION_FILTERING;
	adap->demux.priv             = adap;
	adap->demux.filternum        = 0;
	adap->demux.filternum        = adap->max_feed_count;
	adap->demux.feednum          = adap->demux.filternum;
	adap->demux.start_feed       = dvb_usb_start_feed;
	adap->demux.stop_feed        = dvb_usb_stop_feed;
	adap->demux.write_to_decoder = NULL;
	ret = dvb_dmx_init(&adap->demux);
	if (ret < 0) {
		dev_err(&d->udev->dev, "%s: dvb_dmx_init() failed=%d\n",
				KBUILD_MODNAME, ret);
		goto err_dvb_dmx_init;
	}

	adap->dmxdev.filternum       = adap->demux.filternum;
	adap->dmxdev.demux           = &adap->demux.dmx;
	adap->dmxdev.capabilities    = 0;
	ret = dvb_dmxdev_init(&adap->dmxdev, &adap->dvb_adap);
	if (ret < 0) {
		dev_err(&d->udev->dev, "%s: dvb_dmxdev_init() failed=%d\n",
				KBUILD_MODNAME, ret);
		goto err_dvb_dmxdev_init;
	}

	ret = dvb_net_init(&adap->dvb_adap, &adap->dvb_net, &adap->demux.dmx);
	if (ret < 0) {
		dev_err(&d->udev->dev, "%s: dvb_net_init() failed=%d\n",
				KBUILD_MODNAME, ret);
		goto err_dvb_net_init;
	}

	return 0;
err_dvb_net_init:
	dvb_dmxdev_release(&adap->dmxdev);
err_dvb_dmxdev_init:
	dvb_dmx_release(&adap->demux);
err_dvb_dmx_init:
	dvb_usbv2_media_device_unregister(adap);
err_dvb_register_mc:
	dvb_unregister_adapter(&adap->dvb_adap);
err_dvb_register_adapter:
	adap->dvb_adap.priv = NULL;
	return ret;
}
