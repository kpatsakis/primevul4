static void simple_fill_buf(struct urb *urb)
{
	unsigned	i;
	u8		*buf = urb->transfer_buffer;
	unsigned	len = urb->transfer_buffer_length;
	unsigned	maxpacket;

	switch (pattern) {
	default:
		/* FALLTHROUGH */
	case 0:
		memset(buf, 0, len);
		break;
	case 1:			/* mod63 */
		maxpacket = get_maxpacket(urb->dev, urb->pipe);
		for (i = 0; i < len; i++)
			*buf++ = (u8) ((i % maxpacket) % 63);
		break;
	}
}
