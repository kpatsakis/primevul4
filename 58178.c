static void dvb_usbv2_media_device_unregister(struct dvb_usb_adapter *adap)
{
#ifdef CONFIG_MEDIA_CONTROLLER_DVB

	if (!adap->dvb_adap.mdev)
		return;

	media_device_unregister(adap->dvb_adap.mdev);
	media_device_cleanup(adap->dvb_adap.mdev);
	kfree(adap->dvb_adap.mdev);
	adap->dvb_adap.mdev = NULL;

#endif
}
