static void snd_usbmidi_emagic_output(struct snd_usb_midi_out_endpoint *ep,
				      struct urb *urb)
{
	int port0 = ep->current_port;
	uint8_t *buf = urb->transfer_buffer;
	int buf_free = ep->max_transfer;
	int length, i;

	for (i = 0; i < 0x10; ++i) {
		/* round-robin, starting at the last current port */
		int portnum = (port0 + i) & 15;
		struct usbmidi_out_port *port = &ep->ports[portnum];

		if (!port->active)
			continue;
		if (snd_rawmidi_transmit_peek(port->substream, buf, 1) != 1) {
			port->active = 0;
			continue;
		}

		if (portnum != ep->current_port) {
			if (buf_free < 2)
				break;
			ep->current_port = portnum;
			buf[0] = 0xf5;
			buf[1] = (portnum + 1) & 15;
			buf += 2;
			buf_free -= 2;
		}

		if (buf_free < 1)
			break;
		length = snd_rawmidi_transmit(port->substream, buf, buf_free);
		if (length > 0) {
			buf += length;
			buf_free -= length;
			if (buf_free < 1)
				break;
		}
	}
	if (buf_free < ep->max_transfer && buf_free > 0) {
		*buf = 0xff;
		--buf_free;
	}
	urb->transfer_buffer_length = ep->max_transfer - buf_free;
}
