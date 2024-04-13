static int check_guard_bytes(struct usbtest_dev *tdev, struct urb *urb)
{
	u8 *buf = urb->transfer_buffer;
	u8 *guard = buf - buffer_offset(buf);
	unsigned i;

	for (i = 0; guard < buf; i++, guard++) {
		if (*guard != GUARD_BYTE) {
			ERROR(tdev, "guard byte[%d] %d (not %d)\n",
				i, *guard, GUARD_BYTE);
			return -EINVAL;
		}
	}
	return 0;
}
