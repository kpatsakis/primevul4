static void snd_usbmidi_novation_output(struct snd_usb_midi_out_endpoint *ep,
					struct urb *urb)
{
	uint8_t *transfer_buffer;
	int count;

	if (!ep->ports[0].active)
		return;
	transfer_buffer = urb->transfer_buffer;
	count = snd_rawmidi_transmit(ep->ports[0].substream,
				     &transfer_buffer[2],
				     ep->max_transfer - 2);
	if (count < 1) {
		ep->ports[0].active = 0;
		return;
	}
	transfer_buffer[0] = 0;
	transfer_buffer[1] = count;
	urb->transfer_buffer_length = 2 + count;
}
