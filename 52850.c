static int snd_usbmidi_out_endpoint_create(struct snd_usb_midi *umidi,
					   struct snd_usb_midi_endpoint_info *ep_info,
					   struct snd_usb_midi_endpoint *rep)
{
	struct snd_usb_midi_out_endpoint *ep;
	unsigned int i;
	unsigned int pipe;
	void *buffer;

	rep->out = NULL;
	ep = kzalloc(sizeof(*ep), GFP_KERNEL);
	if (!ep)
		return -ENOMEM;
	ep->umidi = umidi;

	for (i = 0; i < OUTPUT_URBS; ++i) {
		ep->urbs[i].urb = usb_alloc_urb(0, GFP_KERNEL);
		if (!ep->urbs[i].urb) {
			snd_usbmidi_out_endpoint_delete(ep);
			return -ENOMEM;
		}
		ep->urbs[i].ep = ep;
	}
	if (ep_info->out_interval)
		pipe = usb_sndintpipe(umidi->dev, ep_info->out_ep);
	else
		pipe = usb_sndbulkpipe(umidi->dev, ep_info->out_ep);
	switch (umidi->usb_id) {
	default:
		ep->max_transfer = usb_maxpacket(umidi->dev, pipe, 1);
		break;
		/*
		 * Various chips declare a packet size larger than 4 bytes, but
		 * do not actually work with larger packets:
		 */
	case USB_ID(0x0a67, 0x5011): /* Medeli DD305 */
	case USB_ID(0x0a92, 0x1020): /* ESI M4U */
	case USB_ID(0x1430, 0x474b): /* RedOctane GH MIDI INTERFACE */
	case USB_ID(0x15ca, 0x0101): /* Textech USB Midi Cable */
	case USB_ID(0x15ca, 0x1806): /* Textech USB Midi Cable */
	case USB_ID(0x1a86, 0x752d): /* QinHeng CH345 "USB2.0-MIDI" */
	case USB_ID(0xfc08, 0x0101): /* Unknown vendor Cable */
		ep->max_transfer = 4;
		break;
		/*
		 * Some devices only work with 9 bytes packet size:
		 */
	case USB_ID(0x0644, 0x800E): /* Tascam US-122L */
	case USB_ID(0x0644, 0x800F): /* Tascam US-144 */
		ep->max_transfer = 9;
		break;
	}
	for (i = 0; i < OUTPUT_URBS; ++i) {
		buffer = usb_alloc_coherent(umidi->dev,
					    ep->max_transfer, GFP_KERNEL,
					    &ep->urbs[i].urb->transfer_dma);
		if (!buffer) {
			snd_usbmidi_out_endpoint_delete(ep);
			return -ENOMEM;
		}
		if (ep_info->out_interval)
			usb_fill_int_urb(ep->urbs[i].urb, umidi->dev,
					 pipe, buffer, ep->max_transfer,
					 snd_usbmidi_out_urb_complete,
					 &ep->urbs[i], ep_info->out_interval);
		else
			usb_fill_bulk_urb(ep->urbs[i].urb, umidi->dev,
					  pipe, buffer, ep->max_transfer,
					  snd_usbmidi_out_urb_complete,
					  &ep->urbs[i]);
		ep->urbs[i].urb->transfer_flags = URB_NO_TRANSFER_DMA_MAP;
	}

	spin_lock_init(&ep->buffer_lock);
	tasklet_init(&ep->tasklet, snd_usbmidi_out_tasklet, (unsigned long)ep);
	init_waitqueue_head(&ep->drain_wait);

	for (i = 0; i < 0x10; ++i)
		if (ep_info->out_cables & (1 << i)) {
			ep->ports[i].ep = ep;
			ep->ports[i].cable = i << 4;
		}

	if (umidi->usb_protocol_ops->init_out_endpoint)
		umidi->usb_protocol_ops->init_out_endpoint(ep);

	rep->out = ep;
	return 0;
}
