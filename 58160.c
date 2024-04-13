static void dvb_usb_data_complete_raw(struct usb_data_stream *stream, u8 *buf,
		size_t len)
{
	struct dvb_usb_adapter *adap = stream->user_priv;
	dvb_dmx_swfilter_raw(&adap->demux, buf, len);
}
