static int dvb_usbv2_adapter_stream_exit(struct dvb_usb_adapter *adap)
{
	dev_dbg(&adap_to_d(adap)->udev->dev, "%s: adap=%d\n", __func__,
			adap->id);

	return usb_urb_exitv2(&adap->stream);
}
