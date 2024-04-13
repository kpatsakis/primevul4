static void snd_usbmidi_in_urb_complete(struct urb *urb)
{
	struct snd_usb_midi_in_endpoint *ep = urb->context;

	if (urb->status == 0) {
		dump_urb("received", urb->transfer_buffer, urb->actual_length);
		ep->umidi->usb_protocol_ops->input(ep, urb->transfer_buffer,
						   urb->actual_length);
	} else {
		int err = snd_usbmidi_urb_error(urb);
		if (err < 0) {
			if (err != -ENODEV) {
				ep->error_resubmit = 1;
				mod_timer(&ep->umidi->error_timer,
					  jiffies + ERROR_DELAY_JIFFIES);
			}
			return;
		}
	}

	urb->dev = ep->umidi->dev;
	snd_usbmidi_submit_urb(urb, GFP_ATOMIC);
}
