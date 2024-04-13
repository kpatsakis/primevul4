get_alias_quirk(struct usb_device *dev, unsigned int id)
{
	const struct usb_device_id *p;

	for (p = usb_audio_ids; p->match_flags; p++) {
		/* FIXME: this checks only vendor:product pair in the list */
		if ((p->match_flags & USB_DEVICE_ID_MATCH_DEVICE) ==
		    USB_DEVICE_ID_MATCH_DEVICE &&
		    p->idVendor == USB_ID_VENDOR(id) &&
		    p->idProduct == USB_ID_PRODUCT(id))
			return (const struct snd_usb_audio_quirk *)p->driver_info;
	}

	return NULL;
}
