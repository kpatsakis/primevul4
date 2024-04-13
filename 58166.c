static int dvb_usbv2_adapter_dvb_exit(struct dvb_usb_adapter *adap)
{
	dev_dbg(&adap_to_d(adap)->udev->dev, "%s: adap=%d\n", __func__,
			adap->id);

	if (adap->dvb_adap.priv) {
		dvb_net_release(&adap->dvb_net);
		adap->demux.dmx.close(&adap->demux.dmx);
		dvb_dmxdev_release(&adap->dmxdev);
		dvb_dmx_release(&adap->demux);
		dvb_unregister_adapter(&adap->dvb_adap);
	}

	return 0;
}
