static void snd_usbmidi_input_start_ep(struct snd_usb_midi_in_endpoint *ep)
{
	unsigned int i;

	if (!ep)
		return;
	for (i = 0; i < INPUT_URBS; ++i) {
		struct urb *urb = ep->urbs[i];
		urb->dev = ep->umidi->dev;
		snd_usbmidi_submit_urb(urb, GFP_KERNEL);
	}
}
