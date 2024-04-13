static int snd_usbmidi_in_endpoint_create(struct snd_usb_midi *umidi,
					  struct snd_usb_midi_endpoint_info *ep_info,
					  struct snd_usb_midi_endpoint *rep)
{
	struct snd_usb_midi_in_endpoint *ep;
	void *buffer;
	unsigned int pipe;
	int length;
	unsigned int i;

	rep->in = NULL;
	ep = kzalloc(sizeof(*ep), GFP_KERNEL);
	if (!ep)
		return -ENOMEM;
	ep->umidi = umidi;

	for (i = 0; i < INPUT_URBS; ++i) {
		ep->urbs[i] = usb_alloc_urb(0, GFP_KERNEL);
		if (!ep->urbs[i]) {
			snd_usbmidi_in_endpoint_delete(ep);
			return -ENOMEM;
		}
	}
	if (ep_info->in_interval)
		pipe = usb_rcvintpipe(umidi->dev, ep_info->in_ep);
	else
		pipe = usb_rcvbulkpipe(umidi->dev, ep_info->in_ep);
	length = usb_maxpacket(umidi->dev, pipe, 0);
	for (i = 0; i < INPUT_URBS; ++i) {
		buffer = usb_alloc_coherent(umidi->dev, length, GFP_KERNEL,
					    &ep->urbs[i]->transfer_dma);
		if (!buffer) {
			snd_usbmidi_in_endpoint_delete(ep);
			return -ENOMEM;
		}
		if (ep_info->in_interval)
			usb_fill_int_urb(ep->urbs[i], umidi->dev,
					 pipe, buffer, length,
					 snd_usbmidi_in_urb_complete,
					 ep, ep_info->in_interval);
		else
			usb_fill_bulk_urb(ep->urbs[i], umidi->dev,
					  pipe, buffer, length,
					  snd_usbmidi_in_urb_complete, ep);
		ep->urbs[i]->transfer_flags = URB_NO_TRANSFER_DMA_MAP;
	}

	rep->in = ep;
	return 0;
}
