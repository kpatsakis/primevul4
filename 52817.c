static int snd_usbmidi_create_endpoints_midiman(struct snd_usb_midi *umidi,
						struct snd_usb_midi_endpoint_info *endpoint)
{
	struct snd_usb_midi_endpoint_info ep_info;
	struct usb_interface *intf;
	struct usb_host_interface *hostif;
	struct usb_interface_descriptor *intfd;
	struct usb_endpoint_descriptor *epd;
	int cable, err;

	intf = umidi->iface;
	if (!intf)
		return -ENOENT;
	hostif = intf->altsetting;
	intfd = get_iface_desc(hostif);
	/*
	 * The various MidiSport devices have more or less random endpoint
	 * numbers, so we have to identify the endpoints by their index in
	 * the descriptor array, like the driver for that other OS does.
	 *
	 * There is one interrupt input endpoint for all input ports, one
	 * bulk output endpoint for even-numbered ports, and one for odd-
	 * numbered ports.  Both bulk output endpoints have corresponding
	 * input bulk endpoints (at indices 1 and 3) which aren't used.
	 */
	if (intfd->bNumEndpoints < (endpoint->out_cables > 0x0001 ? 5 : 3)) {
		dev_dbg(&umidi->dev->dev, "not enough endpoints\n");
		return -ENOENT;
	}

	epd = get_endpoint(hostif, 0);
	if (!usb_endpoint_dir_in(epd) || !usb_endpoint_xfer_int(epd)) {
		dev_dbg(&umidi->dev->dev, "endpoint[0] isn't interrupt\n");
		return -ENXIO;
	}
	epd = get_endpoint(hostif, 2);
	if (!usb_endpoint_dir_out(epd) || !usb_endpoint_xfer_bulk(epd)) {
		dev_dbg(&umidi->dev->dev, "endpoint[2] isn't bulk output\n");
		return -ENXIO;
	}
	if (endpoint->out_cables > 0x0001) {
		epd = get_endpoint(hostif, 4);
		if (!usb_endpoint_dir_out(epd) ||
		    !usb_endpoint_xfer_bulk(epd)) {
			dev_dbg(&umidi->dev->dev,
				"endpoint[4] isn't bulk output\n");
			return -ENXIO;
		}
	}

	ep_info.out_ep = get_endpoint(hostif, 2)->bEndpointAddress &
		USB_ENDPOINT_NUMBER_MASK;
	ep_info.out_interval = 0;
	ep_info.out_cables = endpoint->out_cables & 0x5555;
	err = snd_usbmidi_out_endpoint_create(umidi, &ep_info,
					      &umidi->endpoints[0]);
	if (err < 0)
		return err;

	ep_info.in_ep = get_endpoint(hostif, 0)->bEndpointAddress &
		USB_ENDPOINT_NUMBER_MASK;
	ep_info.in_interval = get_endpoint(hostif, 0)->bInterval;
	ep_info.in_cables = endpoint->in_cables;
	err = snd_usbmidi_in_endpoint_create(umidi, &ep_info,
					     &umidi->endpoints[0]);
	if (err < 0)
		return err;

	if (endpoint->out_cables > 0x0001) {
		ep_info.out_ep = get_endpoint(hostif, 4)->bEndpointAddress &
			USB_ENDPOINT_NUMBER_MASK;
		ep_info.out_cables = endpoint->out_cables & 0xaaaa;
		err = snd_usbmidi_out_endpoint_create(umidi, &ep_info,
						      &umidi->endpoints[1]);
		if (err < 0)
			return err;
	}

	for (cable = 0; cable < 0x10; ++cable) {
		if (endpoint->out_cables & (1 << cable))
			snd_usbmidi_init_substream(umidi,
						   SNDRV_RAWMIDI_STREAM_OUTPUT,
						   cable,
						   &umidi->endpoints[cable & 1].out->ports[cable].substream);
		if (endpoint->in_cables & (1 << cable))
			snd_usbmidi_init_substream(umidi,
						   SNDRV_RAWMIDI_STREAM_INPUT,
						   cable,
						   &umidi->endpoints[0].in->ports[cable].substream);
	}
	return 0;
}
