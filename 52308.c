static void usbip_pack_cmd_submit(struct usbip_header *pdu, struct urb *urb,
				  int pack)
{
	struct usbip_header_cmd_submit *spdu = &pdu->u.cmd_submit;

	/*
	 * Some members are not still implemented in usbip. I hope this issue
	 * will be discussed when usbip is ported to other operating systems.
	 */
	if (pack) {
		spdu->transfer_flags =
			tweak_transfer_flags(urb->transfer_flags);
		spdu->transfer_buffer_length	= urb->transfer_buffer_length;
		spdu->start_frame		= urb->start_frame;
		spdu->number_of_packets		= urb->number_of_packets;
		spdu->interval			= urb->interval;
	} else  {
		urb->transfer_flags         = spdu->transfer_flags;
		urb->transfer_buffer_length = spdu->transfer_buffer_length;
		urb->start_frame            = spdu->start_frame;
		urb->number_of_packets      = spdu->number_of_packets;
		urb->interval               = spdu->interval;
	}
}
