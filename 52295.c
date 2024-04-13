usbip_alloc_iso_desc_pdu(struct urb *urb, ssize_t *bufflen)
{
	struct usbip_iso_packet_descriptor *iso;
	int np = urb->number_of_packets;
	ssize_t size = np * sizeof(*iso);
	int i;

	iso = kzalloc(size, GFP_KERNEL);
	if (!iso)
		return NULL;

	for (i = 0; i < np; i++) {
		usbip_pack_iso(&iso[i], &urb->iso_frame_desc[i], 1);
		usbip_iso_packet_correct_endian(&iso[i], 1);
	}

	*bufflen = size;

	return iso;
}
