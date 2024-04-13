static void snoop_urb_data(struct urb *urb, unsigned len)
{
	int i, size;

	len = min(len, usbfs_snoop_max);
	if (!usbfs_snoop || len == 0)
		return;

	if (urb->num_sgs == 0) {
		print_hex_dump(KERN_DEBUG, "data: ", DUMP_PREFIX_NONE, 32, 1,
			urb->transfer_buffer, len, 1);
		return;
	}

	for (i = 0; i < urb->num_sgs && len; i++) {
		size = (len > USB_SG_SIZE) ? USB_SG_SIZE : len;
		print_hex_dump(KERN_DEBUG, "data: ", DUMP_PREFIX_NONE, 32, 1,
			sg_virt(&urb->sg[i]), size, 1);
		len -= size;
	}
}
