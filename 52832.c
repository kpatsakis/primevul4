static int snd_usbmidi_get_ms_info(struct snd_usb_midi *umidi,
				   struct snd_usb_midi_endpoint_info *endpoints)
{
	struct usb_interface *intf;
	struct usb_host_interface *hostif;
	struct usb_interface_descriptor *intfd;
	struct usb_ms_header_descriptor *ms_header;
	struct usb_host_endpoint *hostep;
	struct usb_endpoint_descriptor *ep;
	struct usb_ms_endpoint_descriptor *ms_ep;
	int i, epidx;

	intf = umidi->iface;
	if (!intf)
		return -ENXIO;
	hostif = &intf->altsetting[0];
	intfd = get_iface_desc(hostif);
	ms_header = (struct usb_ms_header_descriptor *)hostif->extra;
	if (hostif->extralen >= 7 &&
	    ms_header->bLength >= 7 &&
	    ms_header->bDescriptorType == USB_DT_CS_INTERFACE &&
	    ms_header->bDescriptorSubtype == UAC_HEADER)
		dev_dbg(&umidi->dev->dev, "MIDIStreaming version %02x.%02x\n",
			    ms_header->bcdMSC[1], ms_header->bcdMSC[0]);
	else
		dev_warn(&umidi->dev->dev,
			 "MIDIStreaming interface descriptor not found\n");

	epidx = 0;
	for (i = 0; i < intfd->bNumEndpoints; ++i) {
		hostep = &hostif->endpoint[i];
		ep = get_ep_desc(hostep);
		if (!usb_endpoint_xfer_bulk(ep) && !usb_endpoint_xfer_int(ep))
			continue;
		ms_ep = (struct usb_ms_endpoint_descriptor *)hostep->extra;
		if (hostep->extralen < 4 ||
		    ms_ep->bLength < 4 ||
		    ms_ep->bDescriptorType != USB_DT_CS_ENDPOINT ||
		    ms_ep->bDescriptorSubtype != UAC_MS_GENERAL)
			continue;
		if (usb_endpoint_dir_out(ep)) {
			if (endpoints[epidx].out_ep) {
				if (++epidx >= MIDI_MAX_ENDPOINTS) {
					dev_warn(&umidi->dev->dev,
						 "too many endpoints\n");
					break;
				}
			}
			endpoints[epidx].out_ep = usb_endpoint_num(ep);
			if (usb_endpoint_xfer_int(ep))
				endpoints[epidx].out_interval = ep->bInterval;
			else if (snd_usb_get_speed(umidi->dev) == USB_SPEED_LOW)
				/*
				 * Low speed bulk transfers don't exist, so
				 * force interrupt transfers for devices like
				 * ESI MIDI Mate that try to use them anyway.
				 */
				endpoints[epidx].out_interval = 1;
			endpoints[epidx].out_cables =
				(1 << ms_ep->bNumEmbMIDIJack) - 1;
			dev_dbg(&umidi->dev->dev, "EP %02X: %d jack(s)\n",
				ep->bEndpointAddress, ms_ep->bNumEmbMIDIJack);
		} else {
			if (endpoints[epidx].in_ep) {
				if (++epidx >= MIDI_MAX_ENDPOINTS) {
					dev_warn(&umidi->dev->dev,
						 "too many endpoints\n");
					break;
				}
			}
			endpoints[epidx].in_ep = usb_endpoint_num(ep);
			if (usb_endpoint_xfer_int(ep))
				endpoints[epidx].in_interval = ep->bInterval;
			else if (snd_usb_get_speed(umidi->dev) == USB_SPEED_LOW)
				endpoints[epidx].in_interval = 1;
			endpoints[epidx].in_cables =
				(1 << ms_ep->bNumEmbMIDIJack) - 1;
			dev_dbg(&umidi->dev->dev, "EP %02X: %d jack(s)\n",
				ep->bEndpointAddress, ms_ep->bNumEmbMIDIJack);
		}
	}
	return 0;
}
