static void snd_usbmidi_out_urb_complete(struct urb *urb)
{
	struct out_urb_context *context = urb->context;
	struct snd_usb_midi_out_endpoint *ep = context->ep;
	unsigned int urb_index;

	spin_lock(&ep->buffer_lock);
	urb_index = context - ep->urbs;
	ep->active_urbs &= ~(1 << urb_index);
	if (unlikely(ep->drain_urbs)) {
		ep->drain_urbs &= ~(1 << urb_index);
		wake_up(&ep->drain_wait);
	}
	spin_unlock(&ep->buffer_lock);
	if (urb->status < 0) {
		int err = snd_usbmidi_urb_error(urb);
		if (err < 0) {
			if (err != -ENODEV)
				mod_timer(&ep->umidi->error_timer,
					  jiffies + ERROR_DELAY_JIFFIES);
			return;
		}
	}
	snd_usbmidi_do_output(ep);
}
