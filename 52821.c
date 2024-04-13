static int snd_usbmidi_detect_roland(struct snd_usb_midi *umidi,
				     struct snd_usb_midi_endpoint_info *endpoint)
{
	struct usb_interface *intf;
	struct usb_host_interface *hostif;
	u8 *cs_desc;

	intf = umidi->iface;
	if (!intf)
		return -ENOENT;
	hostif = intf->altsetting;
	/*
	 * Some devices have a descriptor <06 24 F1 02 <inputs> <outputs>>,
	 * some have standard class descriptors, or both kinds, or neither.
	 */
	for (cs_desc = hostif->extra;
	     cs_desc < hostif->extra + hostif->extralen && cs_desc[0] >= 2;
	     cs_desc += cs_desc[0]) {
		if (cs_desc[0] >= 6 &&
		    cs_desc[1] == USB_DT_CS_INTERFACE &&
		    cs_desc[2] == 0xf1 &&
		    cs_desc[3] == 0x02) {
			endpoint->in_cables  = (1 << cs_desc[4]) - 1;
			endpoint->out_cables = (1 << cs_desc[5]) - 1;
			return snd_usbmidi_detect_endpoints(umidi, endpoint, 1);
		} else if (cs_desc[0] >= 7 &&
			   cs_desc[1] == USB_DT_CS_INTERFACE &&
			   cs_desc[2] == UAC_HEADER) {
			return snd_usbmidi_get_ms_info(umidi, endpoint);
		}
	}

	return -ENODEV;
}
