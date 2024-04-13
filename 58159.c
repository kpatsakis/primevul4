static void dvb_usb_data_complete_204(struct usb_data_stream *stream, u8 *buf,
		size_t len)
{
	struct dvb_usb_adapter *adap = stream->user_priv;
	dvb_dmx_swfilter_204(&adap->demux, buf, len);
}
