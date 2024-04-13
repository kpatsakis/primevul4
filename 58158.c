static void dvb_usb_data_complete(struct usb_data_stream *stream, u8 *buf,
		size_t len)
{
	struct dvb_usb_adapter *adap = stream->user_priv;
	dvb_dmx_swfilter(&adap->demux, buf, len);
}
