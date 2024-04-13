static void snd_usbmidi_standard_output(struct snd_usb_midi_out_endpoint *ep,
					struct urb *urb)
{
	int p;

	/* FIXME: lower-numbered ports can starve higher-numbered ports */
	for (p = 0; p < 0x10; ++p) {
		struct usbmidi_out_port *port = &ep->ports[p];
		if (!port->active)
			continue;
		while (urb->transfer_buffer_length + 3 < ep->max_transfer) {
			uint8_t b;
			if (snd_rawmidi_transmit(port->substream, &b, 1) != 1) {
				port->active = 0;
				break;
			}
			snd_usbmidi_transmit_byte(port, b, urb);
		}
	}
}
