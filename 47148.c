int oz_usb_send_isoc(void *hpd, u8 ep_num, struct urb *urb)
{
	struct oz_usb_ctx *usb_ctx = hpd;
	struct oz_pd *pd = usb_ctx->pd;
	struct oz_elt_buf *eb;
	int i;
	int hdr_size;
	u8 *data;
	struct usb_iso_packet_descriptor *desc;

	if (pd->mode & OZ_F_ISOC_NO_ELTS) {
		for (i = 0; i < urb->number_of_packets; i++) {
			u8 *data;

			desc = &urb->iso_frame_desc[i];
			data = ((u8 *)urb->transfer_buffer)+desc->offset;
			oz_send_isoc_unit(pd, ep_num, data, desc->length);
		}
		return 0;
	}

	hdr_size = sizeof(struct oz_isoc_fixed) - 1;
	eb = &pd->elt_buff;
	i = 0;
	while (i < urb->number_of_packets) {
		struct oz_elt_info *ei = oz_elt_info_alloc(eb);
		struct oz_elt *elt;
		struct oz_isoc_fixed *body;
		int unit_count;
		int unit_size;
		int rem;

		if (ei == NULL)
			return -1;
		rem = MAX_ISOC_FIXED_DATA;
		elt = (struct oz_elt *)ei->data;
		body = (struct oz_isoc_fixed *)(elt + 1);
		body->type = OZ_USB_ENDPOINT_DATA;
		body->endpoint = ep_num;
		body->format = OZ_DATA_F_ISOC_FIXED;
		unit_size = urb->iso_frame_desc[i].length;
		body->unit_size = (u8)unit_size;
		data = ((u8 *)(elt+1)) + hdr_size;
		unit_count = 0;
		while (i < urb->number_of_packets) {
			desc = &urb->iso_frame_desc[i];
			if ((unit_size == desc->length) &&
				(desc->length <= rem)) {
				memcpy(data, ((u8 *)urb->transfer_buffer) +
					desc->offset, unit_size);
				data += unit_size;
				rem -= unit_size;
				unit_count++;
				desc->status = 0;
				desc->actual_length = desc->length;
				i++;
			} else {
				break;
			}
		}
		elt->length = hdr_size + MAX_ISOC_FIXED_DATA - rem;
		/* Store the number of units in body->frame_number for the
		 * moment. This field will be correctly determined before
		 * the element is sent. */
		body->frame_number = (u8)unit_count;
		oz_usb_submit_elt(eb, ei, usb_ctx, ep_num,
			pd->mode & OZ_F_ISOC_ANYTIME);
	}
	return 0;
}
