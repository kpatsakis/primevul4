static void snd_usbmidi_do_output(struct snd_usb_midi_out_endpoint *ep)
{
	unsigned int urb_index;
	struct urb *urb;
	unsigned long flags;

	spin_lock_irqsave(&ep->buffer_lock, flags);
	if (ep->umidi->disconnected) {
		spin_unlock_irqrestore(&ep->buffer_lock, flags);
		return;
	}

	urb_index = ep->next_urb;
	for (;;) {
		if (!(ep->active_urbs & (1 << urb_index))) {
			urb = ep->urbs[urb_index].urb;
			urb->transfer_buffer_length = 0;
			ep->umidi->usb_protocol_ops->output(ep, urb);
			if (urb->transfer_buffer_length == 0)
				break;

			dump_urb("sending", urb->transfer_buffer,
				 urb->transfer_buffer_length);
			urb->dev = ep->umidi->dev;
			if (snd_usbmidi_submit_urb(urb, GFP_ATOMIC) < 0)
				break;
			ep->active_urbs |= 1 << urb_index;
		}
		if (++urb_index >= OUTPUT_URBS)
			urb_index = 0;
		if (urb_index == ep->next_urb)
			break;
	}
	ep->next_urb = urb_index;
	spin_unlock_irqrestore(&ep->buffer_lock, flags);
}
