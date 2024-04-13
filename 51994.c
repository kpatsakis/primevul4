static int copy_urb_data_to_user(u8 __user *userbuffer, struct urb *urb)
{
	unsigned i, len, size;

	if (urb->number_of_packets > 0)		/* Isochronous */
		len = urb->transfer_buffer_length;
	else					/* Non-Isoc */
		len = urb->actual_length;

	if (urb->num_sgs == 0) {
		if (copy_to_user(userbuffer, urb->transfer_buffer, len))
			return -EFAULT;
		return 0;
	}

	for (i = 0; i < urb->num_sgs && len; i++) {
		size = (len > USB_SG_SIZE) ? USB_SG_SIZE : len;
		if (copy_to_user(userbuffer, sg_virt(&urb->sg[i]), size))
			return -EFAULT;
		userbuffer += size;
		len -= size;
	}

	return 0;
}
