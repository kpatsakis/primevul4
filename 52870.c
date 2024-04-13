static void snd_usbmidi_us122l_output(struct snd_usb_midi_out_endpoint *ep,
				      struct urb *urb)
{
	int count;

	if (!ep->ports[0].active)
		return;
	switch (snd_usb_get_speed(ep->umidi->dev)) {
	case USB_SPEED_HIGH:
	case USB_SPEED_SUPER:
		count = 1;
		break;
	default:
		count = 2;
	}
	count = snd_rawmidi_transmit(ep->ports[0].substream,
				     urb->transfer_buffer,
				     count);
	if (count < 1) {
		ep->ports[0].active = 0;
		return;
	}

	memset(urb->transfer_buffer + count, 0xFD, ep->max_transfer - count);
	urb->transfer_buffer_length = ep->max_transfer;
}
