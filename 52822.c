static int snd_usbmidi_detect_yamaha(struct snd_usb_midi *umidi,
				     struct snd_usb_midi_endpoint_info *endpoint)
{
	struct usb_interface *intf;
	struct usb_host_interface *hostif;
	struct usb_interface_descriptor *intfd;
	uint8_t *cs_desc;

	intf = umidi->iface;
	if (!intf)
		return -ENOENT;
	hostif = intf->altsetting;
	intfd = get_iface_desc(hostif);
	if (intfd->bNumEndpoints < 1)
		return -ENOENT;

	/*
	 * For each port there is one MIDI_IN/OUT_JACK descriptor, not
	 * necessarily with any useful contents.  So simply count 'em.
	 */
	for (cs_desc = hostif->extra;
	     cs_desc < hostif->extra + hostif->extralen && cs_desc[0] >= 2;
	     cs_desc += cs_desc[0]) {
		if (cs_desc[1] == USB_DT_CS_INTERFACE) {
			if (cs_desc[2] == UAC_MIDI_IN_JACK)
				endpoint->in_cables =
					(endpoint->in_cables << 1) | 1;
			else if (cs_desc[2] == UAC_MIDI_OUT_JACK)
				endpoint->out_cables =
					(endpoint->out_cables << 1) | 1;
		}
	}
	if (!endpoint->in_cables && !endpoint->out_cables)
		return -ENOENT;

	return snd_usbmidi_detect_endpoints(umidi, endpoint, 1);
}
