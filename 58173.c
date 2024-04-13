static int dvb_usbv2_adapter_stream_init(struct dvb_usb_adapter *adap)
{
	dev_dbg(&adap_to_d(adap)->udev->dev, "%s: adap=%d\n", __func__,
			adap->id);

	adap->stream.udev = adap_to_d(adap)->udev;
	adap->stream.user_priv = adap;
	adap->stream.complete = dvb_usb_data_complete;

	return usb_urb_initv2(&adap->stream, &adap->props->stream);
}
