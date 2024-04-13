static void snd_usbmidi_output_standard_packet(struct urb *urb, uint8_t p0,
					       uint8_t p1, uint8_t p2,
					       uint8_t p3)
{

	uint8_t *buf =
		(uint8_t *)urb->transfer_buffer + urb->transfer_buffer_length;
	buf[0] = p0;
	buf[1] = p1;
	buf[2] = p2;
	buf[3] = p3;
	urb->transfer_buffer_length += 4;
}
