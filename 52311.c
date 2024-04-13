static void usbip_pack_ret_submit(struct usbip_header *pdu, struct urb *urb,
				  int pack)
{
	struct usbip_header_ret_submit *rpdu = &pdu->u.ret_submit;

	if (pack) {
		rpdu->status		= urb->status;
		rpdu->actual_length	= urb->actual_length;
		rpdu->start_frame	= urb->start_frame;
		rpdu->number_of_packets = urb->number_of_packets;
		rpdu->error_count	= urb->error_count;
	} else {
		urb->status		= rpdu->status;
		urb->actual_length	= rpdu->actual_length;
		urb->start_frame	= rpdu->start_frame;
		urb->number_of_packets = rpdu->number_of_packets;
		urb->error_count	= rpdu->error_count;
	}
}
