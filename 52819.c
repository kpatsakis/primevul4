static int snd_usbmidi_detect_endpoints(struct snd_usb_midi *umidi,
					struct snd_usb_midi_endpoint_info *endpoint,
					int max_endpoints)
{
	struct usb_interface *intf;
	struct usb_host_interface *hostif;
	struct usb_interface_descriptor *intfd;
	struct usb_endpoint_descriptor *epd;
	int i, out_eps = 0, in_eps = 0;

	if (USB_ID_VENDOR(umidi->usb_id) == 0x0582)
		snd_usbmidi_switch_roland_altsetting(umidi);

	if (endpoint[0].out_ep || endpoint[0].in_ep)
		return 0;

	intf = umidi->iface;
	if (!intf || intf->num_altsetting < 1)
		return -ENOENT;
	hostif = intf->cur_altsetting;
	intfd = get_iface_desc(hostif);

	for (i = 0; i < intfd->bNumEndpoints; ++i) {
		epd = get_endpoint(hostif, i);
		if (!usb_endpoint_xfer_bulk(epd) &&
		    !usb_endpoint_xfer_int(epd))
			continue;
		if (out_eps < max_endpoints &&
		    usb_endpoint_dir_out(epd)) {
			endpoint[out_eps].out_ep = usb_endpoint_num(epd);
			if (usb_endpoint_xfer_int(epd))
				endpoint[out_eps].out_interval = epd->bInterval;
			++out_eps;
		}
		if (in_eps < max_endpoints &&
		    usb_endpoint_dir_in(epd)) {
			endpoint[in_eps].in_ep = usb_endpoint_num(epd);
			if (usb_endpoint_xfer_int(epd))
				endpoint[in_eps].in_interval = epd->bInterval;
			++in_eps;
		}
	}
	return (out_eps || in_eps) ? 0 : -ENOENT;
}
